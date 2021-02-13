

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