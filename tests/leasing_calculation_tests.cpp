#include <doctest.h>

#include "leasing_calculation.h"
#include "car.h"

using namespace std;


TEST_CASE("calculate leasing_rate") {

    Car car{100, 20000, 0, Car_Types::NONE, Car_Brands::NONE,  Fuel_Type::NONE};

    Leasing_Calculation lc{};
    lc.set_car(car);
    lc.set_rest_value(5000);
    lc.set_leasing_duration(5);
    lc.set_deposit(1000);

    optional<double> o_leasing_rate{lc.calculate_leasing_rate()};

    CHECK(o_leasing_rate.has_value());
}


TEST_CASE("failed calculate leasing_rate") {

    Car car{100, 20000, 0, Car_Types::NONE, Car_Brands::NONE,  Fuel_Type::NONE};

    Leasing_Calculation lc{};
    lc.set_car(car);
    lc.set_rest_value(5000);

    optional<double> o_leasing_rate{lc.calculate_leasing_rate()};

    CHECK_FALSE(o_leasing_rate.has_value());
}


TEST_CASE("calculate insurance_rate") {

    Car car{100, 20000, 0, Car_Types::NONE, Car_Brands::NONE,  Fuel_Type::NONE};

    Leasing_Calculation lc{};
    lc.set_car(car);
    lc.set_insurance_class(9);

    optional<double> o_leasing_rate{lc.calculate_insurance_rate()};

    CHECK(o_leasing_rate.has_value());
}


TEST_CASE("failed calculate insurance_rate") {

    Car car{100, 20000, 0, Car_Types::NONE, Car_Brands::NONE,  Fuel_Type::NONE};

    Leasing_Calculation lc{};
    lc.set_insurance_class(0);

    optional<double> o_leasing_rate{lc.calculate_insurance_rate()};

    CHECK_FALSE(o_leasing_rate.has_value());
}