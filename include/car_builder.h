#pragma once

#include <optional>

#include "car.h"

class Car_Builder {
    short unsigned int _ps{80};
    unsigned int _driven_kilometers{0};
    double _purchase_value{16000};

    Fuel_Type _fuel_type{Fuel_Type::PETROL};
    Car_Types _car_type{Car_Types::HATCHBACK};
    Car_Brands _brand{Car_Brands::VW};

public:

    Car_Builder* convert_car_to_builder(Car);
    Car_Builder* ps(short unsigned int);
    Car_Builder* kw(short unsigned int);
    Car_Builder* driven_kilometers(unsigned int);
    Car_Builder* purchase_value(double);
    Car_Builder* car_type(Car_Types);
    Car_Builder* brand(Car_Brands);
    Car_Builder* fuel_type(Fuel_Type);

    std::optional<Car> build();
};