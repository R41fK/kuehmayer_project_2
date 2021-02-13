#pragma once

#include <climits>
#include <cfloat>
#include <optional>

#include "car.h"
#include "car_builder.h"

class Leasing_Calculation {
private:
    Car car{0, 0, 0, Car_Types::NONE, Car_Brands::NONE,  Fuel_Type::NONE};

    short unsigned int leasing_duration{0};
    short unsigned int insurance_class{USHRT_MAX};
    double rest_value{DBL_MAX};
    double deposit{DBL_MAX};
    double annual_interest_rate{0.0201};
    bool _is_over_24{true};

public:

    void set_car(Car);
    void set_leasing_duration(short unsigned int);
    void set_insurance_class(short unsigned int);
    void set_rest_value(double);
    void set_deposit(double);
    void is_under_24();
    void is_over_24();

    std::optional<double> calculate_leasing_rate();
    std::optional<double> calculate_insurance();    
};