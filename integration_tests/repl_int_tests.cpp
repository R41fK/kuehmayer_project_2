#include "json.hpp"
using namespace nlohmann;

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


string send_message(string msg, asio::ip::tcp::iostream* strm) {
    string dec_msg{message_utility::to_ascii(msg)};

    *strm << dec_msg;

    string data;

    getline(*strm, data);

    // cout << data << endl;
    return message_utility::from_ascii(data);
}


TEST_CASE("Repl send message sync Builder") {

    Repl repl{false};
    config::Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    REQUIRE(*repl.strm);

    Car_Builder builder{};
    builder.ps(60)->purchase_value(16000);

    string data{send_message(builder.get_proto_message("test", false), repl.strm)};
    CHECK(data == builder.get_proto_message("test", true));

    repl.strm->close();
}


TEST_CASE("Repl send message build Car") {
    Repl repl{false};
    config::Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    REQUIRE(*repl.strm);

    Car_Builder builder{};
    builder.ps(60)->purchase_value(16000);

    string data{send_message(builder.get_proto_message("test", false), repl.strm)};

    REQUIRE(data == builder.get_proto_message("test", true));

    Reply reply{};

    REQUIRE(reply.ParseFromString(data));

    Car car{reply.car()};

    Request msg{};

    msg.set_type(Request::MessageType::Request_MessageType_BUILD);
    msg.set_name("car");
    msg.set_builder("test");

    REQUIRE(*repl.strm);

    data = send_message(msg.SerializeAsString(), repl.strm);

    CHECK(data == car.get_proto_message("car"));

    repl.strm->close();
}


TEST_CASE("Repl send message sync Car_Calculator") {
    Repl repl{false};
    config::Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    REQUIRE(*repl.strm);

    Car_Calculator calc{};
    calc.is_under_24();
    calc.set_deposit(7000);

    string data{send_message(calc.get_proto_message("calc", "", false), repl.strm)};

    CHECK(data == calc.get_proto_message("calc", "", true));

    repl.strm->close();
}



TEST_CASE("Repl send message calculate insurance") {
    Repl repl{false};
    config::Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    REQUIRE(*repl.strm);

    Car_Builder builder{};
    builder.ps(60)->purchase_value(16000);

    string data{send_message(builder.get_proto_message("test", false), repl.strm)};

    REQUIRE(data == builder.get_proto_message("test", true));

    Reply reply{};

    REQUIRE(reply.ParseFromString(data));

    Car car{reply.car()};

    Request msg{};

    msg.set_type(Request::MessageType::Request_MessageType_BUILD);
    msg.set_name("car");
    msg.set_builder("test");

    REQUIRE(*repl.strm);

    data = send_message(msg.SerializeAsString(), repl.strm);

    REQUIRE(data == car.get_proto_message("car"));

    Car_Calculator calc1{};
    calc1.set_insurance_class(1);
    calc1.is_over_24();

    REQUIRE(*repl.strm);

    data = send_message(calc1.get_proto_message("calc1", "car", false), repl.strm);

    REQUIRE(data == calc1.get_proto_message("calc1", "car", true));

    Request ms{};

    ms.set_type(Request::MessageType::Request_MessageType_CALC_INSURANCE);
    ms.set_name("calc1");

    REQUIRE(*repl.strm);

    data = send_message(ms.SerializeAsString(), repl.strm);

    REQUIRE(builder.build().has_value());

    calc1.set_car(builder.build().value());

    REQUIRE(calc1.calculate_insurance_rate().has_value());

    reply = Reply();
    reply.set_type(Reply::MessageType::Reply_MessageType_DOUBLE);
    reply.set_value(calc1.calculate_insurance_rate().value());
    reply.set_text("Insurance rate:");

    CHECK(data == reply.SerializeAsString());

    repl.strm->close();
}


TEST_CASE("Repl send message calculate leasing") {
    Repl repl{false};
    config::Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    REQUIRE(*repl.strm);

    Car_Builder builder{};
    builder.ps(60)->purchase_value(16000);

    string data{send_message(builder.get_proto_message("test", false), repl.strm)};

    REQUIRE(data == builder.get_proto_message("test", true));

    Reply reply{};
    
    REQUIRE(reply.ParseFromString(data));

    Car car{reply.car()};

    Request msg{};

    msg.set_type(Request::MessageType::Request_MessageType_BUILD);
    msg.set_name("car");
    msg.set_builder("test");

    REQUIRE(*repl.strm);

    data = send_message(msg.SerializeAsString(), repl.strm);

    REQUIRE(data == car.get_proto_message("car"));

    Car_Calculator calc{};
    calc.set_deposit(2000);
    calc.set_leasing_duration(5);
    calc.set_rest_value(5000);

    REQUIRE(*repl.strm);

    data = send_message(calc.get_proto_message("calc", "car", false), repl.strm);

    REQUIRE(data == calc.get_proto_message("calc", "car", true));

    Request ms{};

    ms.set_type(Request::MessageType::Request_MessageType_CALC_LEASING);
    ms.set_name("calc");
    
    REQUIRE(*repl.strm);

    data = send_message(ms.SerializeAsString(), repl.strm);

    optional<Car> o_car{builder.build()};

    REQUIRE(o_car.has_value());

    calc.set_car(o_car.value());

    REQUIRE(calc.calculate_leasing_rate().has_value());

    reply = Reply();

    reply.set_type(Reply::MessageType::Reply_MessageType_DOUBLE);
    reply.set_value(calc.calculate_leasing_rate().value());
    reply.set_text("Leasing rate:");

    CHECK(data == reply.SerializeAsString());

    repl.strm->close();
}


TEST_CASE("Repl send message calculate leasing calculator not found") {
    Repl repl{false};
    config::Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    Request ms{};

    ms.set_type(Request::MessageType::Request_MessageType_CALC_LEASING);
    ms.set_name("calc");

    REQUIRE(*repl.strm);

    string data{send_message(ms.SerializeAsString(), repl.strm)};

    Reply reply{};
    reply.set_type(Reply::MessageType::Reply_MessageType_ERROR);
    reply.set_text("Car_Calculator calc does not exist");

    CHECK(data == reply.SerializeAsString());

    repl.strm->close();

}


TEST_CASE("Repl send message calculate insurance failed") {
    Repl repl{false};
    config::Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    Car_Calculator calc{};
    calc.set_deposit(2000);
    calc.set_leasing_duration(5);
    calc.set_rest_value(5000);

    REQUIRE(*repl.strm);

    string data{send_message(calc.get_proto_message("calc", "", false), repl.strm)};

    REQUIRE(data == calc.get_proto_message("calc", "", true));

    Request ms{};

    ms.set_type(Request::MessageType::Request_MessageType_CALC_INSURANCE);
    ms.set_name("calc");

    REQUIRE(*repl.strm);

    data = send_message(ms.SerializeAsString(), repl.strm);

    Reply reply{};
    reply.set_type(Reply::MessageType::Reply_MessageType_ERROR);
    reply.set_text("Car_Calculator calc failed calculating the insurance rate."
    " Not all key components (car & insurance_class) were set!");

    CHECK(data == reply.SerializeAsString());

    repl.strm->close();

}



TEST_CASE("Repl send message calculate leasing failed") {
    Repl repl{false};
    config::Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    Car_Calculator calc{};
    calc.set_deposit(2000);

    REQUIRE(*repl.strm);

    string data{send_message(calc.get_proto_message("calc", "", false), repl.strm)};

    REQUIRE(data == calc.get_proto_message("calc", "", true));

    Request ms{};

    ms.set_type(Request::MessageType::Request_MessageType_CALC_LEASING);
    ms.set_name("calc");

    REQUIRE(*repl.strm);

    data = send_message(ms.SerializeAsString(), repl.strm);

    Reply reply{};
    reply.set_type(Reply::MessageType::Reply_MessageType_ERROR);
    reply.set_text("Car_Calculator calc failed calculating the leasing rate."
    " Not all key components (car, rest_value, leasing_duration & deposite) were set!");

    CHECK(data == reply.SerializeAsString());

    repl.strm->close();
}


TEST_CASE("Repl send message build Car no Builder") {
    Repl repl{false};
    config::Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    Request msg{};

    msg.set_type(Request::MessageType::Request_MessageType_BUILD);
    msg.set_name("car");
    msg.set_builder("test");

    REQUIRE(*repl.strm);

    string data{send_message(msg.SerializeAsString(), repl.strm)};

    Reply reply{};
    reply.set_type(Reply::MessageType::Reply_MessageType_ERROR);
    reply.set_text("Builder test does not exist");

    CHECK(data == reply.SerializeAsString());

    repl.strm->close();
}



TEST_CASE("Repl send message build Car. Builder failed") {
    Repl repl{false};
    config::Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    Car_Builder builder{};
    builder.ps(60);

    REQUIRE(*repl.strm);

    string data{send_message(builder.get_proto_message("test", false), repl.strm)};

    REQUIRE(data == builder.get_proto_message("test", true));

    Request msg{};

    msg.set_type(Request::MessageType::Request_MessageType_BUILD);
    msg.set_name("car");
    msg.set_builder("test");

    REQUIRE(*repl.strm);

    data = send_message(msg.SerializeAsString(), repl.strm);

    Reply reply{};
    reply.set_type(Reply::MessageType::Reply_MessageType_ERROR);
    reply.set_text("Builder test failed building."
    " Not all key components (ps & purchase_value) were set!");

    CHECK(data == reply.SerializeAsString());

    repl.strm->close();
}


TEST_CASE("Repl sync calculator, no car found") {
    Repl repl{false};
    config::Server server_data{};
    repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    Car_Calculator calc{};
    calc.set_deposit(2000);
    calc.set_leasing_duration(5);
    calc.set_rest_value(5000);

    REQUIRE(*repl.strm);

    string data{send_message(calc.get_proto_message("calc", "car", false), repl.strm)};

    Reply reply{};
    reply.set_text("Car car does not exist");
    reply.set_type(Reply::MessageType::Reply_MessageType_ERROR);

    CHECK(data == reply.SerializeAsString());

    repl.strm->close();
}