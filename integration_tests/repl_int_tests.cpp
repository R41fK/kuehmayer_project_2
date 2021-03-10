#include <doctest.h>
#include <asio.hpp>
#include <fmt/core.h>

#include "message.pb.h"

#include "message_utility.h"
#include "client/repl.h"
#include "car.h"
#include "car_builder.h"
#include "car_calculator.h"

using namespace std;


string send_message(string msg, Repl repl) {
    string dec_msg{message_utility::to_ascii(msg)};

    *repl.strm << dec_msg;

    string data;

    getline(*repl.strm, data);

    return message_utility::from_ascii(data);
}


TEST_CASE("Repl send message sync Builder") {

    Repl repl{false};
    Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    Car_Builder builder{};
    builder.ps(60)->purchase_value(16000);

    string data{send_message(builder.get_proto_message("test"), repl)};
    CHECK(data == "ok");

    repl.strm->close();
}


TEST_CASE("Repl send message build Car") {
    Repl repl{false};
    Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    Car_Builder builder{};
    builder.ps(60)->purchase_value(16000);

    string data{send_message(builder.get_proto_message("test"), repl)};

    REQUIRE(data == "ok");

    Message msg{};

    msg.set_type(Message::MessageType::Message_MessageType_BUILD);
    msg.set_name("car");
    msg.set_builder("test");

    data = send_message(msg.SerializeAsString(), repl);

    CHECK(data == "ok");

    repl.strm->close();
}


TEST_CASE("Repl send message sync Car_Calculator") {
    Repl repl{false};
    Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    Car_Calculator calc{};
    calc.is_under_24();
    calc.set_deposit(7000);

    string data{send_message(calc.get_proto_message("calc", ""), repl)};

    CHECK(data == "ok");

    repl.strm->close();
}



TEST_CASE("Repl send message calculate insurance") {
    Repl repl{false};
    Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    Car_Builder builder{};
    builder.ps(60)->purchase_value(16000);

    string data{send_message(builder.get_proto_message("test"), repl)};

    REQUIRE(data == "ok");

    Message msg{};

    msg.set_type(Message::MessageType::Message_MessageType_BUILD);
    msg.set_name("car");
    msg.set_builder("test");

    data = send_message(msg.SerializeAsString(), repl);

    REQUIRE(data == "ok");

    Car_Calculator calc1{};
    calc1.set_insurance_class(1);
    calc1.is_over_24();


    data = send_message(calc1.get_proto_message("calc1", "car"), repl);

    REQUIRE(data == "ok");

    Message ms{};

    ms.set_type(Message::MessageType::Message_MessageType_CALC_INSURANCE);
    ms.set_name("calc1");

    data = send_message(ms.SerializeAsString(), repl);

    REQUIRE(builder.build().has_value());

    calc1.set_car(builder.build().value());

    REQUIRE(calc1.calculate_insurance_rate().has_value());

    CHECK(data == fmt::format("Insurance rate: {}", calc1.calculate_insurance_rate().value()));

    repl.strm->close();
}


TEST_CASE("Repl send message calculate leasing") {
    Repl repl{false};
    Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    Car_Builder builder{};
    builder.ps(60)->purchase_value(16000);

    string data{send_message(builder.get_proto_message("test"), repl)};

    REQUIRE(data == "ok");

    Message msg{};

    msg.set_type(Message::MessageType::Message_MessageType_BUILD);
    msg.set_name("car");
    msg.set_builder("test");

    data = send_message(msg.SerializeAsString(), repl);

    REQUIRE(data == "ok");

    Car_Calculator calc{};
    calc.set_deposit(2000);
    calc.set_leasing_duration(5);
    calc.set_rest_value(5000);

    data = send_message(calc.get_proto_message("calc", "car"), repl);

    REQUIRE(data == "ok");

    Message ms{};

    ms.set_type(Message::MessageType::Message_MessageType_CALC_LEASING);
    ms.set_name("calc");

    data = send_message(ms.SerializeAsString(), repl);

    optional<Car> o_car{builder.build()};

    REQUIRE(o_car.has_value());

    calc.set_car(o_car.value());

    REQUIRE(calc.calculate_leasing_rate().has_value());

    CHECK(data == fmt::format("Leasing rate: {}", calc.calculate_leasing_rate().value()));

    repl.strm->close();
}


TEST_CASE("Repl send message calculate leasing calculator not found") {
    Repl repl{false};
    Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    Message ms{};

    ms.set_type(Message::MessageType::Message_MessageType_CALC_LEASING);
    ms.set_name("calc");

    string data{send_message(ms.SerializeAsString(), repl)};

    CHECK(data == "Car_Calculator calc does not exist");

    repl.strm->close();

}


TEST_CASE("Repl send message calculate insurance failed") {
    Repl repl{false};
    Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    Car_Calculator calc{};
    calc.set_deposit(2000);
    calc.set_leasing_duration(5);
    calc.set_rest_value(5000);

    string data{send_message(calc.get_proto_message("calc", ""), repl)};

    REQUIRE(data == "ok");

    Message ms{};

    ms.set_type(Message::MessageType::Message_MessageType_CALC_INSURANCE);
    ms.set_name("calc");

    data = send_message(ms.SerializeAsString(), repl);

    CHECK(data == "Car_Calculator calc failed calculating the insurance rate. Not all key components (car & insurance_class) were set!");

    repl.strm->close();

}



TEST_CASE("Repl send message calculate leasing failed") {
    Repl repl{false};
    Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    Car_Calculator calc{};
    calc.set_deposit(2000);

    string data{send_message(calc.get_proto_message("calc", ""), repl)};

    REQUIRE(data == "ok");

    Message ms{};

    ms.set_type(Message::MessageType::Message_MessageType_CALC_LEASING);
    ms.set_name("calc");

    data = send_message(ms.SerializeAsString(), repl);

    CHECK(data == "Car_Calculator calc failed calculating the leasing rate. Not all key components (car, rest_value, leasing_duration & deposite) were set!");

    repl.strm->close();
}


TEST_CASE("Repl send message build Car no Builder") {
    Repl repl{false};
    Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    Message msg{};

    msg.set_type(Message::MessageType::Message_MessageType_BUILD);
    msg.set_name("car");
    msg.set_builder("test");

    string data{send_message(msg.SerializeAsString(), repl)};

    CHECK(data == "Builder test does not exist");

    repl.strm->close();
}



TEST_CASE("Repl send message build Car. Builder failed") {
    Repl repl{false};
    Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    Car_Builder builder{};
    builder.ps(60);

    string data{send_message(builder.get_proto_message("test"), repl)};

    REQUIRE(data == "ok");

    Message msg{};

    msg.set_type(Message::MessageType::Message_MessageType_BUILD);
    msg.set_name("car");
    msg.set_builder("test");

    data = send_message(msg.SerializeAsString(), repl);

    CHECK(data == "Builder test failed building. Not all key components (ps & purchase_value) were set!");

    repl.strm->close();
}


TEST_CASE("Repl sync calculator, no car found") {
    Repl repl{false};
    Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    Car_Calculator calc{};
    calc.set_deposit(2000);
    calc.set_leasing_duration(5);
    calc.set_rest_value(5000);

    string data{send_message(calc.get_proto_message("calc", "car"), repl)};

    REQUIRE(data == "Car car does not exist");
}