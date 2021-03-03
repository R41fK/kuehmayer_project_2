#include <sstream>

#include <magic_enum.hpp>

#include "message.pb.h"

#include "car.h"

using namespace std;

short unsigned int Car::get_ps() {
    return this->ps;
}


short unsigned int Car::get_kw() {
    return this->ps / 1.36;
}


double Car::get_purchase_value() {
    return this->purchase_value;
}


unsigned int Car::get_driven_kilometers() {
    return this->driven_kilometers;
}


Car_Types Car::get_car_type() {
    return this->car_type;
}


Car_Brands Car::get_brand() {
    return this->brand;
}


Fuel_Type Car::get_fuel_type() {
    return this->fuel_type;
}

Car::operator bool() {
    return ps != 0 && purchase_value != 0;
}

bool Car::operator==(const Car car) const {
    return this->ps == car.ps
        && this->purchase_value == car.purchase_value
        && this->driven_kilometers == car.driven_kilometers
        && this->brand == car.brand
        && this->fuel_type == car.fuel_type
        && this->car_type == car.car_type
        ;
}

string Car::to_string() {
    stringstream strm{};
    strm << "Car(ps=" << ps
         << ", purchase_value=" << purchase_value
         << ", driven_kilometers=" << driven_kilometers
         << ", brand=" << magic_enum::enum_name(brand)
         << ", fuel_type=" << magic_enum::enum_name(fuel_type)
         << ", car_type=" << magic_enum::enum_name(car_type)
         << ")";

    return strm.str();
}

Car::Car(Message_Car msg) {

    this->driven_kilometers = msg.driven_kilometers();

    this->ps = msg.ps();

    this->purchase_value = msg.purchase_value();

    switch (msg.brand()) {
        case Message_Car_Brands::AUDI:
            this->brand = Car_Brands::AUDI;
            break;
        
        case Message_Car_Brands::BMW:
            this->brand = Car_Brands::BMW;
            break;
        
        case Message_Car_Brands::MERCEDES:
            this->brand = Car_Brands::MERCEDES;
            break;

        case Message_Car_Brands::SEAT:
            this->brand = Car_Brands::SEAT;
            break;

        case Message_Car_Brands::SKODA:
            this->brand = Car_Brands::SKODA;
            break;

        case Message_Car_Brands::VW:
           this->brand = Car_Brands::VW;
            break;

        default:
           this->brand = Car_Brands::NONE;
    }

    switch (msg.car_type()) {
        case Message_Car_Types::COUPE:
            this->car_type = Car_Types::COUPE;
            break;

        case Message_Car_Types::HATCHBACK:
            this->car_type = Car_Types::HATCHBACK;
            break;

        case Message_Car_Types::MINIVAN:
            this->car_type = Car_Types::MINIVAN;
            break;

        case Message_Car_Types::PICKUP_TRUCK:
            this->car_type = Car_Types::PICKUP_TRUCK;
            break;

        case Message_Car_Types::SEDAN:
            this->car_type = Car_Types::SEDAN;
            break;

        case Message_Car_Types::SPORTS_CAR:
            this->car_type = Car_Types::SPORTS_CAR;
            break;

        case Message_Car_Types::SUV:
            this->car_type = Car_Types::SUV;
            break;

        default:
            this->car_type = Car_Types::NONE;
    }

    switch (msg.fuel_type()) {
        case Message_Fuel_Type::DIESEL:
            this->fuel_type = Fuel_Type::DIESEL;
            break;

        case Message_Fuel_Type::ELECTRONIC:
            this->fuel_type = Fuel_Type::ELECTRONIC;
            break;

        case Message_Fuel_Type::NATURAL_GAS:
            this->fuel_type = Fuel_Type::NATURAL_GAS;
            break;

        case Message_Fuel_Type::PETROL:
            this->fuel_type = Fuel_Type::PETROL;
            break;

        default:
            this->fuel_type = Fuel_Type::NONE;
    }
}