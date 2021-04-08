#pragma once

#include <optional>
#include <string>

#include "message.pb.h"

#include "car.h"

class Car_Builder {
    short unsigned int _ps{0};
    unsigned int _driven_kilometers{0};
    double _purchase_value{0};

    Fuel_Type _fuel_type{Fuel_Type::NONE};
    Car_Types _car_type{Car_Types::NONE};
    Car_Brands _brand{Car_Brands::NONE};

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

    bool operator==(const Car_Builder) const;
    std::string to_string();

    std::string get_proto_message(std::string, bool);
    void update_car_builder_from_proto_message(Message_Car);
};