#pragma once

#include <peglib.h>


#include "car.h"
#include "car_builder.h"
#include "car_calculator.h"
#include "config.h"


class Repl {
private:
    bool& running;

    Server server_data;

    std::map<std::string, Car> cars{};
    std::map<std::string, Car_Builder> car_builders{};
    std::map<std::string, Car_Calculator> car_calculators{};

    std::string grammar{R"(
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
                     / NAME 'calculate_leasing'
                     / NAME 'calculate_insurance'
                     / NAME 'display'
        
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
                     / NAME 'print' 

        FUEL_TYPES  <- 'petrol' / 'diesel' / 'natural_gas' / 'electric'
        CAR_BRANDS  <- 'vw' / 'audi' / 'mercedes' / 'bmw' / 'skoda' / 'seat'
        CAR_TYPES   <- 'sedan' / 'coupe' / 'sports_car' / 'hatchback' / 'suv' / 'minivan' / 'pickup_truck'
        NUMBER      <- < [0-9]+ >
        DNUMBER     <- < [0-9]+ ('.'[0-9]+)? >
        NAME        <- < [a-z]+ >
        %whitespace <- [ \t]*
    )"};
    
    peg::parser parser;

    // show the commands for the repl
    void show_help();

    // stop all threads
    void stop();

    void no_Car(std::string);

    void no_Car_Builder(std::string);

    void no_Car_Calculator(std::string);

    void send_message(std::string);

public:
    Repl(bool&, Server);

    // a method that starts the repl, should be started in its own thread
    void operator()();
};