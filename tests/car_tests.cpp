#include <doctest.h>

#include "car_builder.h"
#include "car.h"

using namespace std;


TEST_CASE("building of a Car") {

    optional<Car> o_car{Car_Builder().ps(100)->purchase_value(20000)->build()};

    Car car{100, 20000, 0, Car_Types::NONE, Car_Brands::NONE,  Fuel_Type::NONE};

    REQUIRE(o_car.has_value());
    CHECK(car == o_car.value());
}


TEST_CASE("failed building of a Car") {

    optional<Car> o_car{Car_Builder().build()};

    CHECK_FALSE((bool) o_car.has_value());
}


TEST_CASE("bool operator for a Car") {

    Car car{100, 20000, 0, Car_Types::NONE, Car_Brands::NONE,  Fuel_Type::NONE};

    CHECK((bool) car);
}


TEST_CASE("failed bool operator for a Car") {

    Car car_1{0, 20000, 0, Car_Types::NONE, Car_Brands::NONE,  Fuel_Type::NONE};
    Car car_2{100, 0, 0, Car_Types::NONE, Car_Brands::NONE,  Fuel_Type::NONE};
    Car car_3{0, 0, 0, Car_Types::NONE, Car_Brands::NONE,  Fuel_Type::NONE};

    CHECK_FALSE((bool) car_1);
    CHECK_FALSE((bool) car_2);
    CHECK_FALSE((bool) car_3);
}


TEST_CASE("car to builder") {

    Car car{100, 20000, 0, Car_Types::NONE, Car_Brands::NONE,  Fuel_Type::NONE};

    Car_Builder* o_car_1{Car_Builder().ps(100)->purchase_value(20000)};
    Car_Builder* o_car_2{Car_Builder().convert_car_to_builder(car)};

    CHECK(*o_car_1 == *o_car_2);
}