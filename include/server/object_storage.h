#pragma once

#include <map>

#include "car.h"
#include "car_builder.h"
#include "car_calculator.h"

class Object_Storage {
    private:

    std::map<std::string, Car> cars{};
    std::map<std::string, Car_Builder> car_builders{};
    std::map<std::string, Car_Calculator> car_calculators{};

    public:
    
    std::string new_action(std::string);
};