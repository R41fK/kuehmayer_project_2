#include <iostream>
#include <algorithm>
#include <string>
#include <map>

#include <fmt/core.h>
#include <fmt/color.h>
#include <peglib.h>
#include "pystring.h"
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

#include "client/repl.h"
#include "car.h"
#include "car_builder.h"
#include "car_calculator.h"

using namespace std;
using namespace peg;


void Repl::show_help() {
    fmt::print(
    R"(Commands:
    help | h                                        shows this help message
    end | stop                                      stops the programm

    car_calculator <calculator_name>                creates a car_calculator with the name <calculator_name>
    <calculator_name> car = <car_name>              sets the car for the car_calculator with the name <calculator_name>
    <calculator_name> leasing_duration = <int>      sets the leasing_duration for the car_calculator with the name <calculator_name> 
    <calculator_name> insurance_class = <int>       sets the insurance_class for the car_calculator with the name <calculator_name>
    <calculator_name> rest_value = <double>         sets the rest_value for the car_calculator with the name <calculator_name>
    <calculator_name> deposit = <double>            sets the deposite for the car_calculator with the name <calculator_name>
    <calculator_name> is_under_24                   sets that the person is under 24 for the car_calculator with the name <calculator_name>
    <calculator_name> is_over_24                    sets that the person is over 24 for the car_calculator with the name <calculator_name>

    car_builder <builder_name>                      creates a car_builder with the name <builder_name>
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




void Repl::stop() {
    this->running = false;
    fmt::print("Stoped\n");
}

void no_Car_Builder(string s) {
    fmt::print("No Car_Builder with this name: {}\n", s);
}

void no_Car(string s) {
    fmt::print("No Car with this name: {}\n", s);
}

void no_Car_Calculator(string s) {
    fmt::print("No Car_Calculator with this name: {}\n", s);
}


void Repl::operator()() {

    string input{};

    map<string, Car> cars{};
    map<string, Car_Builder> car_builders{};
    map<string, Car_Calculator> car_calculators{};

    auto grammar  {R"(
        START       <- HELP / CAR_BUILDER / CALCULATOR / CAR / END

        HELP        <- 'help' / 'h'

        END         <- 'end' / 'stop'

        CALCULATOR  <- 'car_calculator' NAME
                     / NAME 'car =' NAME
                     / NAME 'leasing_duration =' NUMBER
                     / NAME 'insurance_class =' NUMBER
                     / NAME 'rest_value =' DNUMBER
                     / NAME 'deposit =' DNUMBER
                     / NAME 'is_under_24'
                     / NAME 'is_over_24'
        
        CAR         <- NAME 'show' 
                     / NAME 'ps'
                     / NAME 'kw'
                     / NAME 'purchase_value'
                     / NAME 'driven_kilometers'
                     / NAME 'car_type'
                     / NAME 'brand'
                     / NAME 'fuel_type'


        CAR_BUILDER <- 'car_builder' NAME 
                     / NAME 'ps =' NUMBER 
                     / NAME 'purchase_value =' DNUMBER
                     / NAME 'driven_kilometer =' NUMBER
                     / NAME 'fuel_type =' FUEL_TYPES
                     / NAME 'car_brand =' CAR_BRANDS
                     / NAME 'car_type =' CAR_TYPES
                     / 'car' NAME '=' NAME 'build'

        FUEL_TYPES  <- 'petrol' / 'diesel' / 'natural_gas' / 'electric'
        CAR_BRANDS  <- 'vw' / 'audi' / 'mercedes' / 'bmw' / 'skoda' / 'seat'
        CAR_TYPES   <- 'sedan' / 'coupe' / 'sports_car' / 'hatchback' / 'suv' / 'minivan' / 'pickup_truck'
        NUMBER      <- < [0-9]+ >
        DNUMBER     <- < [0-9]+ ('.'[0-9]+)? >
        NAME        <- < [a-z]+ >
        %whitespace <- [ \t]*
    )"};
    

    parser parser;

    parser.log = [&](size_t line, size_t col, const string& msg) {
        fmt::print("{}:{}: {}\n", line, col, msg);
        show_help();
    };

    auto ok = parser.load_grammar(grammar);
    assert(ok);
    assert(static_cast<bool>(parser) == true);


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
                break;


            case 1: // NAME 'car =' NAME
                if (car_calculators.find(any_cast<string>(vs[0])) != car_calculators.end()) {
                    car_calculators.at(any_cast<string>(vs[0])).set_car(cars.at(any_cast<string>(vs[1])));
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
        }
    };


    parser["CAR"] = [&](const SemanticValues &vs) {
        switch (vs.choice()) {
            case 0: // NAME 'show'
                if (cars.find(any_cast<string>(vs[0])) != cars.end()) {
                    cout << any_cast<string>(vs[0]) << endl;
                } else {
                    no_Car(any_cast<string>(vs[0]));
                }
                break;


            case 1: // NAME 'ps'
                if (cars.find(any_cast<string>(vs[0])) != cars.end()) {
                    cout << cars.at(any_cast<string>(vs[0])).get_ps() << endl;
                } else {
                    no_Car(any_cast<string>(vs[0]));
                }
                break;


            case 2: // NAME 'kw'
                if (cars.find(any_cast<string>(vs[0])) != cars.end()) {
                    cout << cars.at(any_cast<string>(vs[0])).get_kw() << endl;
                } else {
                    no_Car(any_cast<string>(vs[0]));
                }
                break;


            case 3: // NAME 'purchase_value'
                if (cars.find(any_cast<string>(vs[0])) != cars.end()) {
                    cout << cars.at(any_cast<string>(vs[0])).get_purchase_value() << endl;
                } else {
                    no_Car(any_cast<string>(vs[0]));
                }
                break;


            case 4: // NAME 'driven_kilometers'
                if (cars.find(any_cast<string>(vs[0])) != cars.end()) {
                    cout << cars.at(any_cast<string>(vs[0])).get_driven_kilometers() << endl;
                } else {
                    no_Car(any_cast<string>(vs[0]));
                }
                break;


            case 5: // NAME 'car_type'
                if (cars.find(any_cast<string>(vs[0])) != cars.end()) {
                    cout << magic_enum::enum_name(cars.at(any_cast<string>(vs[0])).get_car_type()) << endl;
                } else {
                    no_Car(any_cast<string>(vs[0]));
                }
                break;


            case 6: // NAME 'brand'
                if (cars.find(any_cast<string>(vs[0])) != cars.end()) {
                    cout << magic_enum::enum_name(cars.at(any_cast<string>(vs[0])).get_brand()) << endl;
                } else {
                    no_Car(any_cast<string>(vs[0]));
                }
                break;


            case 7: // NAME 'fuel_type'
                if (cars.find(any_cast<string>(vs[0])) != cars.end()) {
                    cout << magic_enum::enum_name(cars.at(any_cast<string>(vs[0])).get_fuel_type()) << endl;
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
                    if (o_car.has_value()) {
                        cars.insert_or_assign(any_cast<string>(vs[0]), o_car.value());
                    } else {

                    }
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



    while (this->running) {
        
        getline(cin, input);

        input = pystring::lower(input);

        input = pystring::strip(input);

        parser.parse(input.c_str());
        
    }
}