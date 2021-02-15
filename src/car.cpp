

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