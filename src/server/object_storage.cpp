#include <iostream>
#include <asio.hpp>

#include "pystring.h"
#include <spdlog/spdlog.h>
#include <fmt/core.h>

#include "message.pb.h"

#include "message_utility.h"
#include "server/object_storage.h"

using namespace std;


string Object_Storage::new_action(string data) {

    data = message_utility::from_ascii(data);

    spdlog::debug(fmt::format("Client Message decoded to '{}'", data));    

    //cout << data << endl;

    Message msg{};

    if (msg.ParseFromString(data)) {
        spdlog::info(fmt::format("Message could be Parsed to Proto objekt"));

        switch (msg.type()) {
            case Message::MessageType::Message_MessageType_BUILDER:
                spdlog::debug(fmt::format("Message is MessageType Builder"));

                if (this->car_builders.find(msg.name()) != this->car_builders.end()) {
                    spdlog::debug(fmt::format("Builder {} existsts and will be updated", msg.name()));

                    this->car_builders.at(msg.name()).update_car_builder_from_proto_message(msg.car());
                } else {
                    spdlog::debug(fmt::format("Builder {} does not existst and will be created", msg.name()));

                    Car_Builder builder{};
                    builder.update_car_builder_from_proto_message(msg.car());
                    this->car_builders.insert_or_assign(msg.name(), builder);
                }
                break;

            case Message::MessageType::Message_MessageType_BUILD:
                spdlog::debug(fmt::format("Message is MessageType Build"));

                if (this->car_builders.find(msg.builder()) != this->car_builders.end()) {
                    if (this->cars.find(msg.name()) != this->cars.end()) {
                        spdlog::debug(
                            fmt::format("Car {} existsts and will be updated with Builder {}"
                                        , msg.name()
                                        , msg.builder())
                        );


                        optional<Car> o_car{this->car_builders.at(msg.builder()).build()};
                        if (o_car.has_value()) {
                            this->cars.at(msg.name()) = o_car.value();
                        } else {
                            spdlog::info(
                                fmt::format("Builder {} failed building. Not all key components (ps & purchase_value) were set!"
                                            , msg.name()
                                )
                            );

                             return fmt::format("Builder {} failed building. Not all key components (ps & purchase_value) were set!"
                                            , msg.name()
                                    );
                        }
                    } else {
                        spdlog::debug(
                            fmt::format("Car {} does not exist and will be created with Builder {}"
                                        , msg.name()
                                        , msg.builder()
                            )
                        );

                        optional<Car> o_car{this->car_builders.at(msg.builder()).build()};
                        if (o_car.has_value()) {
                            this->cars.insert_or_assign(msg.name(), o_car.value());
                        } else {
                            spdlog::info(
                                fmt::format("Builder {} failed building. Not all key components (ps & purchase_value) were set!"
                                            , msg.builder()
                                )
                            );

                             return fmt::format("Builder {} failed building. Not all key components (ps & purchase_value) were set!"
                                            , msg.builder()
                                    );
                        }
                    }
                } else {
                        spdlog::info(
                            fmt::format("Builder {} does not exist"
                                        , msg.builder()
                            )
                        );

                        return fmt::format("Builder {} does not exist", msg.builder());
                    }
                break;

            case Message::MessageType::Message_MessageType_CALC_INSURANCE:
                spdlog::debug(fmt::format("Message is MessageType calc insurance"));

                if (this->car_calculators.find(msg.name()) != this->car_calculators.end()) {
                    optional<double> ob_ins{this->car_calculators.at(msg.name()).calculate_insurance_rate()};
                    if (ob_ins.has_value()) {
                        spdlog::debug(fmt::format("Car_Calculator {} calculated a insurance rate of {}", msg.name(), ob_ins.value()));

                        return fmt::format("Insurance rate: {}", ob_ins.value());
                    } else {
                        spdlog::info(fmt::format("Car_Calculator {} failed calculating the insurance rate. {}" 
                            , msg.name(), "Not all key components (car & insurance_class) were set!")
                        );

                        spdlog::debug(this->car_calculators.at(msg.name()).to_string());

                        return fmt::format("Car_Calculator {} failed calculating the insurance rate. {}" 
                            , msg.name(), "Not all key components (car & insurance_class) were set!");
                    }
                } else {
                    spdlog::info(fmt::format("Car_Calculator {} does not exist", msg.name()));

                    return fmt::format("Car_Calculator {} does not exist", msg.name());
                }
                break;

            case Message::MessageType::Message_MessageType_CALC_LEASING:
                spdlog::debug(fmt::format("Message is MessageType calc leasing"));

                if (this->car_calculators.find(msg.name()) != this->car_calculators.end()) {
                     optional<double> ob_lea{this->car_calculators.at(msg.name()).calculate_leasing_rate()};
                    if (ob_lea.has_value()) {
                        spdlog::debug(fmt::format("Car_Calculator {} calculated a leasing rate of {}", msg.name(), ob_lea.value()));

                        return fmt::format("Leasing rate: {}", ob_lea.value());
                    } else {
                        spdlog::info(fmt::format("Car_Calculator {} failed calculating the leasing rate. {}" 
                            , msg.name(), "Not all key components (car, rest_value, leasing_duration & deposite) were set!")
                        );

                        spdlog::debug(this->car_calculators.at(msg.name()).to_string());

                        return fmt::format("Car_Calculator {} failed calculating the leasing rate. {}" 
                            , msg.name(), "Not all key components (car, rest_value, leasing_duration & deposite) were set!");
                    }
                } else {
                    spdlog::info(fmt::format("Car_Calculator {} does not exist", msg.name()));

                    return fmt::format("Car_Calculator {} does not exist", msg.name());
                }
                break;

            case Message::MessageType::Message_MessageType_CALCULATOR:
                spdlog::debug(fmt::format("Message is MessageType calculator"));

                if (this->car_calculators.find(msg.name()) != this->car_calculators.end()) {
                    spdlog::debug(fmt::format("Calculator {} existsts and will be updated", msg.name()));

                    this->car_calculators.at(msg.name()).update_car_calculator_from_proto_message(msg.calculator());

                    if (msg.calculator().car() != "") {
                        if (this->cars.find(msg.calculator().car()) != this->cars.end()) {

                            this->car_calculators.at(msg.name()).set_car(this->cars.at(msg.calculator().car()));
                        } else {
                            spdlog::info(fmt::format("Car {} does not exist", msg.calculator().car()));

                            return fmt::format("Car {} does not exist", msg.calculator().car());
                        }
                    }                    

                } else {
                    spdlog::debug(fmt::format("Calculator {} does not existst and will be created", msg.name()));
                    
                    Car_Calculator calc{};
                    calc.update_car_calculator_from_proto_message(msg.calculator());

                    this->car_calculators.insert_or_assign(msg.name(), calc);

                    if (msg.calculator().car() != "") {
                        if (this->cars.find(msg.calculator().car()) != this->cars.end()) {

                            this->car_calculators.at(msg.name()).set_car(this->cars.at(msg.calculator().car()));
                        } else {
                            spdlog::info(fmt::format("Car {} does not exist", msg.calculator().car()));

                            return fmt::format("Car {} does not exist", msg.calculator().car());
                        }
                    }
                }
                break;

            default:
                spdlog::debug(fmt::format("Message doesn't match any MessageType. Message = '{}'", msg.name()));
                return "ok";
        }
    } else {
        spdlog::info(fmt::format("Message could not be Parsed to Proto objekt. Message = '{}'", data));
        return "pars error";
    }

    return "ok";
}