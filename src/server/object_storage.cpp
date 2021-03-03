#include <iostream>
#include <asio.hpp>

#include "pystring.h"

#include "message.pb.h"

#include "server/object_storage.h"

using namespace std;


string string_from_hex(string data) {
    
    data = pystring::lstrip(data);
    stringstream sstream{};
    string new_string{}, temp{};

    sstream << data;

    while (sstream >> temp) {
        new_string.push_back((char) stol(temp, 0, 16));
    }
    
    return new_string;
}


string Object_Storage::new_action(string data) {

    data = string_from_hex(data);

    Message msg{};

    if (msg.ParseFromString(data)) {
        switch (msg.type()) {
            case Message::MessageType::Message_MessageType_BUILDER:
                if (this->car_builders.find(msg.name()) != this->car_builders.end()) {
                    this->car_builders.at(msg.name()).update_car_builder_from_proto_message(msg.car());
                } else {
                    Car_Builder builder{};
                    builder.update_car_builder_from_proto_message(msg.car());
                    this->car_builders.insert_or_assign(msg.name(), builder);
                }
                break;

            case Message::MessageType::Message_MessageType_BUILD:
                if (this->car_builders.find(msg.builder()) != this->car_builders.end()) {
                    if (this->cars.find(msg.name()) != this->cars.end()) {
                        optional<Car> o_car{this->car_builders.at(msg.builder()).build()};
                        if (o_car.has_value()) {
                            this->cars.at(msg.name()) = o_car.value();
                        }
                    } else {
                        optional<Car> o_car{this->car_builders.at(msg.builder()).build()};
                        if (o_car.has_value()) {
                            this->cars.insert_or_assign(msg.name(), o_car.value());
                        }
                    }
                }
                break;

            case Message::MessageType::Message_MessageType_CALC_INSURANCE:
                if (this->car_calculators.find(msg.name()) != this->car_calculators.end()) {
                    optional<double> ob_ins{this->car_calculators.at(msg.name()).calculate_insurance_rate()};
                    if (ob_ins.has_value()) {
                        return to_string(ob_ins.value());
                    }
                }
                break;

            case Message::MessageType::Message_MessageType_CALC_LEASING:
                if (this->car_calculators.find(msg.name()) != this->car_calculators.end()) {
                     optional<double> ob_lea{this->car_calculators.at(msg.name()).calculate_leasing_rate()};
                    if (ob_lea.has_value()) {
                        return to_string(ob_lea.value());
                    }
                }
                break;

            case Message::MessageType::Message_MessageType_CALCULATOR:
                if (this->car_calculators.find(msg.name()) != this->car_calculators.end()) {
                    this->car_calculators.at(msg.name()).update_car_calculator_from_proto_message(msg.calculator());
                } else {
                    Car_Calculator calc{};
                    calc.update_car_calculator_from_proto_message(msg.calculator());
                    this->car_calculators.insert_or_assign(msg.name(), calc);
                }
                break;

            default:
                return "Wrong input?!?";
        }
    }

    return "";
}