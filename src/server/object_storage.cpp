#include <iostream>
#include <asio.hpp>

#include "pystring.h"
#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <fmt/color.h>

#include "message.pb.h"

#include "message_utility.h"
#include "server/object_storage.h"

using namespace std;

void Object_Storage::print(string output, fmt::color color) {
    if (this->allow_print) {
        fmt::print(fg(color), output + "\n");
    }
}


string Object_Storage::new_action(string data) {

    data = message_utility::from_ascii(data);

    spdlog::debug(fmt::format("Client Message decoded to '{}'", data));    

    if (data == "") { // checking the connection at the begining of the client
        return "ok";
    }

    //cout << data << endl;

    Request msg{};

    if (msg.ParseFromString(data)) {
        spdlog::debug(fmt::format("Message could be Parsed to Proto object"));

        switch (msg.type()) {
            case Request::MessageType::Request_MessageType_BUILDER:
                spdlog::debug(fmt::format("Message is MessageType Builder"));

                if (this->car_builders.find(msg.name()) != this->car_builders.end()) {
                    spdlog::info(fmt::format("Builder {} existsts and will be updated", msg.name()));
                    print(fmt::format("Builder {} existsts and will be updated", msg.name()), fmt::color::medium_sea_green);

                    this->car_builders.at(msg.name()).update_car_builder_from_proto_message(msg.car());
                } else {
                    spdlog::info(fmt::format("Builder {} does not existst and will be created", msg.name()));
                    print(fmt::format("Builder {} does not existst and will be created", msg.name()), fmt::color::medium_sea_green);

                    Car_Builder builder{};
                    builder.update_car_builder_from_proto_message(msg.car());
                    this->car_builders.insert_or_assign(msg.name(), builder);
                }

                return this->car_builders.at(msg.name()).get_proto_message(msg.name(), true);

            case Request::MessageType::Request_MessageType_BUILD:
                spdlog::debug(fmt::format("Message is MessageType Build"));

                if (this->car_builders.find(msg.builder()) != this->car_builders.end()) {
                    if (this->cars.find(msg.name()) != this->cars.end()) {
                        spdlog::info(
                            fmt::format("Car {} existsts and will be updated with Builder {}"
                                        , msg.name()
                                        , msg.builder())
                        );

                        print(fmt::format("Car {} existsts and will be updated with Builder {}"
                                        , msg.name()
                                        , msg.builder())
                            , fmt::color::medium_sea_green);


                        optional<Car> o_car{this->car_builders.at(msg.builder()).build()};
                        if (o_car.has_value()) {
                            this->cars.at(msg.name()) = o_car.value();

                            return this->cars.at(msg.name()).get_proto_message(msg.name());
                        } else {
                            spdlog::info(
                                fmt::format("Builder {} failed building. Not all key components (ps & purchase_value) were set!"
                                            , msg.name()
                                )
                            );

                            print(fmt::format("Builder {} failed building. Not all key components (ps & purchase_value) were set!"
                                            , msg.name())
                            , fmt::color::orange);


                            Reply reply{};

                            reply.set_type(Reply::MessageType::Reply_MessageType_ERROR);
                            reply.set_text(fmt::format("Builder {} failed building. Not all key components (ps & purchase_value) were set!"
                                            , msg.name()));

                            return reply.SerializeAsString();
                        }
                    } else {
                        spdlog::info(
                            fmt::format("Car {} does not exist and will be created with Builder {}"
                                        , msg.name()
                                        , msg.builder()
                            )
                        );

                        print(fmt::format("Car {} does not exist and will be created with Builder {}"
                                        , msg.name()
                                        , msg.builder())
                            , fmt::color::medium_sea_green);

                        optional<Car> o_car{this->car_builders.at(msg.builder()).build()};
                        if (o_car.has_value()) {
                            this->cars.insert_or_assign(msg.name(), o_car.value());

                            return this->cars.at(msg.name()).get_proto_message(msg.name());
                        } else {
                            spdlog::info(
                                fmt::format("Builder {} failed building. Not all key components (ps & purchase_value) were set!"
                                            , msg.builder()
                                )
                            );

                            print(fmt::format("Builder {} failed building. Not all key components (ps & purchase_value) were set!"
                                            , msg.builder())
                            , fmt::color::orange);


                            Reply reply{};

                            reply.set_type(Reply::MessageType::Reply_MessageType_ERROR);
                            reply.set_text(fmt::format("Builder {} failed building. Not all key components (ps & purchase_value) were set!"
                                            , msg.builder()));

                            return reply.SerializeAsString();
                        }
                    }
                } else {
                        spdlog::info(
                            fmt::format("Builder {} does not exist"
                                        , msg.builder()
                            )
                        );

                        print(fmt::format("Builder {} does not exist"
                                        , msg.builder())
                            , fmt::color::orange);


                        Reply reply{};

                        reply.set_type(Reply::MessageType::Reply_MessageType_ERROR);
                        reply.set_text(fmt::format("Builder {} does not exist", msg.builder()));

                        return reply.SerializeAsString();
                    }
                break;

            case Request::MessageType::Request_MessageType_CALC_INSURANCE:
                spdlog::debug(fmt::format("Message is MessageType calc insurance"));

                if (this->car_calculators.find(msg.name()) != this->car_calculators.end()) {
                    optional<double> ob_ins{this->car_calculators.at(msg.name()).calculate_insurance_rate()};
                    if (ob_ins.has_value()) {
                        spdlog::info(fmt::format("Car_Calculator {} calculated a insurance rate of {}", msg.name(), ob_ins.value()));

                        print(fmt::format("Car_Calculator {} calculated a insurance rate of {}", msg.name(), ob_ins.value())
                            , fmt::color::medium_sea_green);

                        Reply reply{};

                        reply.set_type(Reply::MessageType::Reply_MessageType_DOUBLE);
                        reply.set_value(ob_ins.value());
                        reply.set_text("Insurance rate:");

                        return reply.SerializeAsString();
                    } else {
                        spdlog::info(fmt::format("Car_Calculator {} failed calculating the insurance rate. {}" 
                            , msg.name(), "Not all key components (car & insurance_class) were set!")
                        );

                        print(fmt::format("Car_Calculator {} failed calculating the insurance rate. {}" 
                                , msg.name(), "Not all key components (car & insurance_class) were set!")
                            , fmt::color::orange);

                        spdlog::debug(this->car_calculators.at(msg.name()).to_string());

                        Reply reply{};

                        reply.set_type(Reply::MessageType::Reply_MessageType_ERROR);
                        reply.set_text(fmt::format("Car_Calculator {} failed calculating the insurance rate. {}" 
                            , msg.name(), "Not all key components (car & insurance_class) were set!"));

                        return reply.SerializeAsString();
                    }
                } else {
                    spdlog::info(fmt::format("Car_Calculator {} does not exist", msg.name()));

                    print(fmt::format("Car_Calculator {} does not exist", msg.name())
                            , fmt::color::orange);

                    Reply reply{};

                    reply.set_type(Reply::MessageType::Reply_MessageType_ERROR);
                    reply.set_text(fmt::format("Car_Calculator {} does not exist", msg.name()));

                    return reply.SerializeAsString();
                }
                break;

            case Request::MessageType::Request_MessageType_CALC_LEASING:
                spdlog::debug(fmt::format("Message is MessageType calc leasing"));

                if (this->car_calculators.find(msg.name()) != this->car_calculators.end()) {
                     optional<double> ob_lea{this->car_calculators.at(msg.name()).calculate_leasing_rate()};
                    if (ob_lea.has_value()) {
                        spdlog::info(fmt::format("Car_Calculator {} calculated a leasing rate of {}", msg.name(), ob_lea.value()));

                        print(fmt::format("Car_Calculator {} calculated a leasing rate of {}", msg.name(), ob_lea.value())
                            , fmt::color::medium_sea_green);

                        Reply reply{};

                        reply.set_type(Reply::MessageType::Reply_MessageType_DOUBLE);
                        reply.set_value(ob_lea.value());
                        reply.set_text("Leasing rate:");

                        return reply.SerializeAsString();
                    } else {
                        spdlog::info(fmt::format("Car_Calculator {} failed calculating the leasing rate. {}" 
                            , msg.name(), "Not all key components (car, rest_value, leasing_duration & deposite) were set!")
                        );

                        print(fmt::format("Car_Calculator {} failed calculating the leasing rate. {}" 
                                , msg.name(), "Not all key components (car, rest_value, leasing_duration & deposite) were set!")
                            , fmt::color::orange);

                        spdlog::debug(this->car_calculators.at(msg.name()).to_string());


                        Reply reply{};

                        reply.set_type(Reply::MessageType::Reply_MessageType_ERROR);
                        reply.set_text(fmt::format("Car_Calculator {} failed calculating the leasing rate. {}" 
                            , msg.name(), "Not all key components (car, rest_value, leasing_duration & deposite) were set!"));

                        return reply.SerializeAsString();
                    }
                } else {
                    spdlog::info(fmt::format("Car_Calculator {} does not exist", msg.name()));

                    print(fmt::format("Car_Calculator {} does not exist", msg.name())
                            , fmt::color::orange);

                    Reply reply{};

                    reply.set_type(Reply::MessageType::Reply_MessageType_ERROR);
                    reply.set_text(fmt::format("Car_Calculator {} does not exist", msg.name()));

                    return reply.SerializeAsString();
                }
                break;

            case Request::MessageType::Request_MessageType_CALCULATOR:
                spdlog::debug(fmt::format("Message is MessageType calculator"));

                if (this->car_calculators.find(msg.name()) != this->car_calculators.end()) {
                    spdlog::info(fmt::format("Calculator {} existsts and will be updated", msg.name()));

                    print(fmt::format("Calculator {} existsts and will be updated", msg.name())
                            , fmt::color::medium_sea_green);

                    this->car_calculators.at(msg.name()).update_car_calculator_from_proto_message(msg.calculator());

                    if (msg.calculator().car() != "") {
                        if (this->cars.find(msg.calculator().car()) != this->cars.end()) {

                            this->car_calculators.at(msg.name()).set_car(this->cars.at(msg.calculator().car()));

                            return this->car_calculators.at(msg.name()).get_proto_message(msg.name(), msg.calculator().car(), true);
                        } else {
                            spdlog::info(fmt::format("Car {} does not exist", msg.calculator().car()));

                            print(fmt::format("Car {} does not exist", msg.calculator().car())
                            , fmt::color::orange);

                            Reply reply{};

                            reply.set_type(Reply::MessageType::Reply_MessageType_ERROR);
                            reply.set_text(fmt::format("Car {} does not exist", msg.calculator().car()));

                            return reply.SerializeAsString();
                        }
                    } else {
                        return this->car_calculators.at(msg.name()).get_proto_message(msg.name(), "", true);
                    }              

                } else {
                    spdlog::info(fmt::format("Calculator {} does not existst and will be created", msg.name()));

                    print(fmt::format("Calculator {} does not existst and will be created", msg.name())
                            , fmt::color::medium_sea_green);
                    
                    Car_Calculator calc{};
                    calc.update_car_calculator_from_proto_message(msg.calculator());

                    this->car_calculators.insert_or_assign(msg.name(), calc);

                    if (msg.calculator().car() != "") {
                        if (this->cars.find(msg.calculator().car()) != this->cars.end()) {

                            this->car_calculators.at(msg.name()).set_car(this->cars.at(msg.calculator().car()));

                            return this->car_calculators.at(msg.name()).get_proto_message(msg.name(), msg.calculator().car(), true);
                        } else {
                            spdlog::info(fmt::format("Car {} does not exist", msg.calculator().car()));

                            print(fmt::format("Car {} does not exist", msg.calculator().car())
                            , fmt::color::orange);

                            Reply reply{};

                            reply.set_type(Reply::MessageType::Reply_MessageType_ERROR);
                            reply.set_text(fmt::format("Car {} does not exist", msg.calculator().car()));

                            return reply.SerializeAsString();
                        }
                    } else {
                        return this->car_calculators.at(msg.name()).get_proto_message(msg.name(), "", true);
                    }
                }
                break;

            default:
                spdlog::info(fmt::format("Message doesn't match any MessageType. Message = '{}'", msg.name()));

                print(fmt::format("Message doesn't match any MessageType. Message = '{}'", msg.name())
                            , fmt::color::orange);

                Reply msg{};

                msg.set_type(Reply::MessageType::Reply_MessageType_ERROR);
                msg.set_text("Message doesn't match any MessageType!");

                return msg.SerializeAsString();
        }
    } else {
        spdlog::info(fmt::format("Message could not be Parsed to Proto objekt. Message = '{}'", data));
        Reply msg{};

        msg.set_type(Reply::MessageType::Reply_MessageType_ERROR);
        msg.set_text("parse error");

        return msg.SerializeAsString();

    }

    return "ok";
}