#include <iostream>
#include <algorithm>
#include <string>
#include <map>
#include <bitset>
#include <asio.hpp>

#include <fmt/core.h>
#include <fmt/color.h>
#include <peglib.h>
#include "pystring.h"
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

#include "message.pb.h"

#include "message_utility.h"
#include "client/repl.h"
#include "car.h"
#include "car_builder.h"
#include "car_calculator.h"


using namespace std;
using namespace peg;



Repl::Repl(bool& running, Server server_data):
    running{running},
    server_data{server_data}
    {
        
    this->strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

    parser.log = [&](size_t line, size_t col, const string& msg) {
        fmt::print("{}:{}: {}\n", line, col, msg);
        show_help();
    };

    parser.load_grammar(grammar);

    parser["HELP"] = [this](const SemanticValues) {
        this->show_help();
    };


    parser["END"] = [this](const SemanticValues) {
        this->stop();
    };


    parser["CALCULATOR"] = [&](const SemanticValues &vs) {
        switch (vs.choice()) {
            case 0: // 'car_calculator' NAME
                car_calculators.insert_or_assign(any_cast<string>(vs[0]), Car_Calculator());
                this->send_message(car_calculators.at(any_cast<string>(vs[0])).get_proto_message(any_cast<string>(vs[0]), ""));
                break;


            case 1: // NAME 'car =' NAME
                if (car_calculators.find(any_cast<string>(vs[0])) != car_calculators.end()) {
                    this->send_message(car_calculators.at(any_cast<string>(vs[0])).get_proto_message(any_cast<string>(vs[0]), any_cast<string>(vs[1])));

                    if (cars.find(any_cast<string>(vs[1])) != cars.end()) {
                        car_calculators.at(any_cast<string>(vs[0])).set_car(cars.at(any_cast<string>(vs[1])));
                    }
                    
                } else {
                    no_Car_Calculator(any_cast<string>(vs[0]));
                }
                break;


            case 2: // NAME 'leasing_duration =' NUMBER
                if (car_calculators.find(any_cast<string>(vs[0])) != car_calculators.end()) {
                    car_calculators.at(any_cast<string>(vs[0])).set_leasing_duration(any_cast<int>(vs[1]));
                } else {
                    no_Car_Calculator(any_cast<string>(vs[0]));
                }
                break;


            case 3: // NAME 'insurance_class =' NUMBER
                if (car_calculators.find(any_cast<string>(vs[0])) != car_calculators.end()) {
                    car_calculators.at(any_cast<string>(vs[0])).set_insurance_class(any_cast<int>(vs[1]));
                } else {
                    no_Car_Calculator(any_cast<string>(vs[0]));
                }
                break;


            case 4: // NAME 'rest_value =' DNUMBER
                if (car_calculators.find(any_cast<string>(vs[0])) != car_calculators.end()) {
                    car_calculators.at(any_cast<string>(vs[0])).set_rest_value(any_cast<double>(vs[1]));
                } else {
                    no_Car_Calculator(any_cast<string>(vs[0]));
                }
                break;


            case 5: // NAME 'deposit =' DNUMBER
                if (car_calculators.find(any_cast<string>(vs[0])) != car_calculators.end()) {
                    car_calculators.at(any_cast<string>(vs[0])).set_deposit(any_cast<double>(vs[1]));
                } else {
                    no_Car_Calculator(any_cast<string>(vs[0]));
                }
                break;


            case 6: // NAME 'is_under_24'
                if (car_calculators.find(any_cast<string>(vs[0])) != car_calculators.end()) {
                    car_calculators.at(any_cast<string>(vs[0])).is_under_24();
                } else {
                    no_Car_Calculator(any_cast<string>(vs[0]));
                }
                break;


            case 7: // NAME 'is_over_24'
                if (car_calculators.find(any_cast<string>(vs[0])) != car_calculators.end()) {
                    car_calculators.at(any_cast<string>(vs[0])).is_over_24();
                } else {
                    no_Car_Calculator(any_cast<string>(vs[0]));
                }
                break;

            case 8: // NAME 'calculate_leasing'
                if (car_calculators.find(any_cast<string>(vs[0])) != car_calculators.end()) {
                    this->send_message(car_calculators.at(any_cast<string>(vs[0])).get_proto_message(any_cast<string>(vs[0]), ""));

                    Message msg{};

                    msg.set_type(Message::MessageType::Message_MessageType_CALC_LEASING);
                    msg.set_name(any_cast<string>(vs[0]));

                    this->send_message(msg.SerializeAsString());
                } else {
                    no_Car_Calculator(any_cast<string>(vs[0]));
                }
                break;

            case 9: // NAME 'calculate_insurance'
                if (car_calculators.find(any_cast<string>(vs[0])) != car_calculators.end()) {
                    this->send_message(car_calculators.at(any_cast<string>(vs[0])).get_proto_message(any_cast<string>(vs[0]), ""));

                    Message msg{};

                    msg.set_type(Message::MessageType::Message_MessageType_CALC_INSURANCE);
                    msg.set_name(any_cast<string>(vs[0]));

                    this->send_message(msg.SerializeAsString());
                } else {
                    no_Car_Calculator(any_cast<string>(vs[0]));
                }
                break;

            case 10: // NAME 'display'
                if (car_calculators.find(any_cast<string>(vs[0])) != car_calculators.end()) {
                    fmt::print("{}\n", car_calculators.at(any_cast<string>(vs[0])).to_string());
                } else {
                    no_Car_Calculator(any_cast<string>(vs[0]));
                }
                break;
        }
    };


    parser["CAR"] = [&](const SemanticValues &vs) {
        switch (vs.choice()) {
            case 0: // NAME 'show'
                if (cars.find(any_cast<string>(vs[0])) != cars.end()) {
                    fmt::print("{}\n", cars.at(any_cast<string>(vs[0])).to_string());
                } else {
                    no_Car(any_cast<string>(vs[0]));
                }
                break;


            case 1: // NAME 'ps'
                if (cars.find(any_cast<string>(vs[0])) != cars.end()) {
                    fmt::print("ps = {}\n", cars.at(any_cast<string>(vs[0])).get_ps());
                } else {
                    no_Car(any_cast<string>(vs[0]));
                }
                break;


            case 2: // NAME 'kw'
                if (cars.find(any_cast<string>(vs[0])) != cars.end()) {
                    fmt::print("kw = {}\n", cars.at(any_cast<string>(vs[0])).get_kw());
                } else {
                    no_Car(any_cast<string>(vs[0]));
                }
                break;


            case 3: // NAME 'purchase_value'
                if (cars.find(any_cast<string>(vs[0])) != cars.end()) {
                    fmt::print("purchase_value = {}\n", cars.at(any_cast<string>(vs[0])).get_purchase_value());
                } else {
                    no_Car(any_cast<string>(vs[0]));
                }
                break;


            case 4: // NAME 'driven_kilometers'
                if (cars.find(any_cast<string>(vs[0])) != cars.end()) {
                    fmt::print("driven_kilometers = {}\n", cars.at(any_cast<string>(vs[0])).get_driven_kilometers());
                } else {
                    no_Car(any_cast<string>(vs[0]));
                }
                break;


            case 5: // NAME 'car_type'
                if (cars.find(any_cast<string>(vs[0])) != cars.end()) {
                    fmt::print("car_type = {}\n", magic_enum::enum_name(cars.at(any_cast<string>(vs[0])).get_car_type()));
                } else {
                    no_Car(any_cast<string>(vs[0]));
                }
                break;


            case 6: // NAME 'brand'
                if (cars.find(any_cast<string>(vs[0])) != cars.end()) {
                    fmt::print("brand = {}\n", magic_enum::enum_name(cars.at(any_cast<string>(vs[0])).get_brand()));
                } else {
                    no_Car(any_cast<string>(vs[0]));
                }
                break;


            case 7: // NAME 'fuel_type'
                if (cars.find(any_cast<string>(vs[0])) != cars.end()) {
                    fmt::print("fuel_type = {}\n", magic_enum::enum_name(cars.at(any_cast<string>(vs[0])).get_fuel_type()));
                } else {
                    no_Car(any_cast<string>(vs[0]));
                }
                break;
        }
    };


    parser["CAR_BUILDER"] = [&](const SemanticValues &vs) {
        
        switch (vs.choice()) {
            case 0: //'car_builder' NAME 
                car_builders.insert_or_assign(any_cast<string>(vs[0]), Car_Builder());

                this->send_message(car_builders.at(any_cast<string>(vs[0])).get_proto_message(any_cast<string>(vs[0])));
                break;


            case 1: // NAME 'ps =' NUMBER 
                if (car_builders.find(any_cast<string>(vs[0])) != car_builders.end()) {
                    car_builders.at(any_cast<string>(vs[0])).ps(any_cast<int>(vs[1]));
                } else {
                    no_Car_Builder(any_cast<string>(vs[0]));
                }
                break;


            case 2: // NAME 'purchase_value =' DNUMBER
                if (car_builders.find(any_cast<string>(vs[0])) != car_builders.end()) {
                    car_builders.at(any_cast<string>(vs[0])).purchase_value(any_cast<double>(vs[1]));
                } else {
                    no_Car_Builder(any_cast<string>(vs[0]));
                }
                break;


            case 3: // NAME 'driven_kilometer =' NUMBER
                if (car_builders.find(any_cast<string>(vs[0])) != car_builders.end()) {
                    car_builders.at(any_cast<string>(vs[0])).driven_kilometers(any_cast<int>(vs[1]));
                } else {
                    no_Car_Builder(any_cast<string>(vs[0]));
                }
                break;


            case 4: // NAME 'fuel_type =' FUEL_TYPES
                if (car_builders.find(any_cast<string>(vs[0])) != car_builders.end()) {
                    car_builders.at(any_cast<string>(vs[0])).fuel_type(any_cast<Fuel_Type>(vs[1]));
                } else {
                    no_Car_Builder(any_cast<string>(vs[0]));
                }
                break;


            case 5: // NAME 'car_brand =' CAR_BRANDS
                if (car_builders.find(any_cast<string>(vs[0])) != car_builders.end()) {
                    car_builders.at(any_cast<string>(vs[0])).brand(any_cast<Car_Brands>(vs[1]));
                } else {
                    no_Car_Builder(any_cast<string>(vs[0]));
                }
                break;


            case 6: // NAME 'car_type =' CAR_TYPES
                if (car_builders.find(any_cast<string>(vs[0])) != car_builders.end()) {
                    car_builders.at(any_cast<string>(vs[0])).car_type(any_cast<Car_Types>(vs[1]));
                } else {
                    no_Car_Builder(any_cast<string>(vs[0]));
                }
                break;


            case 7: // 'car' NAME '=' NAME 'build'
                if (car_builders.find(any_cast<string>(vs[1])) != car_builders.end()) {
                    optional<Car> o_car{car_builders.at(any_cast<string>(vs[1])).build()};

                    this->send_message(car_builders.at(any_cast<string>(vs[1])).get_proto_message(any_cast<string>(vs[1])));

                    Message msg{};

                    msg.set_type(Message::MessageType::Message_MessageType_BUILD);
                    msg.set_name(any_cast<string>(vs[0]));
                    msg.set_builder(any_cast<string>(vs[1]));

                    this->send_message(msg.SerializeAsString());

                    if (o_car.has_value()) {
                        cars.insert_or_assign(any_cast<string>(vs[0]), o_car.value());
                    } 
                } else {
                    no_Car_Builder(any_cast<string>(vs[0]));
                }
                break;

            case 8: // NAME 'print'
                if (car_builders.find(any_cast<string>(vs[0])) != car_builders.end()) {
                    fmt::print("{}\n", car_builders.at(any_cast<string>(vs[0])).to_string());
                } else {
                    no_Car_Builder(any_cast<string>(vs[0]));
                }
                break;
        }
    };


    parser["FUEL_TYPES"] = [](const SemanticValues &vs) {
        // 'petrol' / 'diesel' / 'natural_gas' / 'electric'
        switch (vs.choice()) {
            case 0:
                return Fuel_Type::PETROL;
            case 1:
                return Fuel_Type::DIESEL;
            case 2:
                return Fuel_Type::NATURAL_GAS;
            case 3:
                return Fuel_Type::ELECTRONIC;
            default:
                return Fuel_Type::NONE;
        }
    };


    parser["CAR_BRANDS"] = [](const SemanticValues &vs) {
        // 'vw' / 'audi' / 'mercedes' / 'bmw' / 'skoda' / 'seat'
        switch (vs.choice()) {
            case 0:
                return Car_Brands::VW;
            case 1:
                return Car_Brands::AUDI;
            case 2:
                return Car_Brands::MERCEDES;
            case 3:
                return Car_Brands::BMW;
            case 4:
                return Car_Brands::SKODA;
            case 5:
                return Car_Brands::SEAT;
            default:
                return Car_Brands::NONE;
        }
    };


    parser["CAR_TYPES"] = [](const SemanticValues &vs) {
        // 'sedan' / 'coupe' / 'sports_car' / 'hatchback' / 'suv' / 'minivan' / 'pickup_truck'
        switch (vs.choice()) {
            case 0:
                return Car_Types::SEDAN;
            case 1:
                return Car_Types::COUPE;
            case 2:
                return Car_Types::SPORTS_CAR;
            case 3:
                return Car_Types::HATCHBACK;
            case 4:
                return Car_Types::SUV;
            case 5:
                return Car_Types::MINIVAN;
            case 6:
                return Car_Types::PICKUP_TRUCK;
            default:
                return Car_Types::NONE;
        }
    };


    parser["NAME"] = [](const SemanticValues &vs) {
        return vs.token_to_string();
    };


    parser["NUMBER"] = [](const SemanticValues &vs) {
        return vs.token_to_number<int>();
    };


    parser["DNUMBER"] = [](const SemanticValues &vs) {
        return vs.token_to_number<double>();
    };

}

void Repl::show_help() {
    fmt::print(
    R"(Commands:
    help | h                                        shows this help message
    end | stop                                      stops the programm

    car_calculator <calculator_name>                creates a car_calculator with the name <calculator_name>
    <calculator_name> print                         shows the car_calculator object for the object with the name <calculator_name>
    <calculator_name> car = <car_name>              sets the car for the car_calculator with the name <calculator_name>
    <calculator_name> leasing_duration = <int>      sets the leasing_duration for the car_calculator with the name <calculator_name> 
    <calculator_name> insurance_class = <int>       sets the insurance_class for the car_calculator with the name <calculator_name>
    <calculator_name> rest_value = <double>         sets the rest_value for the car_calculator with the name <calculator_name>
    <calculator_name> deposit = <double>            sets the deposite for the car_calculator with the name <calculator_name>
    <calculator_name> is_under_24                   sets that the person is under 24 for the car_calculator with the name <calculator_name>
    <calculator_name> is_over_24                    sets that the person is over 24 for the car_calculator with the name <calculator_name>

    car_builder <builder_name>                      creates a car_builder with the name <builder_name>
    <builder_name> print                            shows the car_builder object for the object with the name <builder_name>
    <builder_name> ps = <int>                       sets the ps for the car_builder with the name <builder_name>
    <builder_name> purchase_value = <double>        sets the purchase_value for the car_builder with the name <builder_name>
    <builder_name> driven_kilometer = <int>         sets the driven_kilometer for the car_builder with the name <builder_name>
    <builder_name> fuel_type = <fuel_types>         sets the fuel_type for the car_builder with the name <builder_name>
    <builder_name> car_brand = <car_brands>         sets the car_brand for the car_builder with the name <builder_name>
    <builder_name> car_type = <car_type>            sets the car_type for the car_builder with the name <builder_name>

    car <car_name> = <builder_name> build           creates a car with the name <car_name> from a car_builder with the name <builder_name>
    <car_name> show                                 shows the car object for the object with the name <car_name>
    <car_name> ps                                   shows the ps for the object with the name <car_name>
    <car_name> kw                                   shows the kw for the object with the name <car_name>
    <car_name> purchase_value                       shows the purchase_value for the object with the name <car_name>
    <car_name> driven_kilometers                    shows the driven_kilometers for the object with the name <car_name>
    <car_name> car_type                             shows the car_type for the object with the name <car_name>
    <car_name> brand                                shows the brand for the object with the name <car_name>
    <car_name> fuel_type                            shows the fuel_type for the object with the name <car_name>

    <fuel_types> = petrol | diesel | natural_gas | electric
    <car_brands> = vw | audi | mercedes | bmw | skoda | seat
    <car_type>   = sedan | coupe | sports_car | hatchback | suv | minivan | pickup_truck

)"
    );
}


void Repl::no_Car_Builder(string s) {
    fmt::print("No Car_Builder with this name: {}\n", s);
}

void Repl::no_Car(string s) {
    fmt::print("No Car with this name: {}\n", s);
}

void Repl::no_Car_Calculator(string s) {
    fmt::print("No Car_Calculator with this name: {}\n", s);
}


void Repl::send_message(string msg) {

   if (*strm) {
        *strm << message_utility::to_hex(msg) << endl;

        string data;

        getline(*strm, data);
        data = message_utility::from_hex(data);

        if (data != "") fmt::print("{}\n", data);
        
   } else {

       this->strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};
       this->send_message(msg);
   }
}


void Repl::stop() {
    this->running = false;
    strm->close();
    fmt::print("Stoped\n");
}

void Repl::operator()() {

    string input{};

    while (this->running) {
        
        getline(cin, input);

        input = pystring::lower(input);

        input = pystring::strip(input);

        parser.parse(input.c_str());
        
    }
}