#pragma once

#include <map>

#include <fmt/color.h>

#include "car.h"
#include "car_builder.h"
#include "car_calculator.h"

class Object_Storage {
    private:
    bool allow_print{false};

    std::map<std::string, Car> cars{};
    std::map<std::string, Car_Builder> car_builders{};
    std::map<std::string, Car_Calculator> car_calculators{};

    // prints a message to the consol if allowed
    void print(std::string, fmt::color);

    public:

    Object_Storage(bool allow_print) : allow_print(allow_print) {};
    
    // interprets the message from the client and creates the response
    std::string new_action(std::string);
};