
#include <exception>

#include "car.h"
#include "car_builder.h"

using namespace std;

Car_Builder* Car_Builder::ps(short unsigned int ps){
    this->_ps = ps;
    return this;
}


Car_Builder* Car_Builder::kw(short unsigned int kw){
    this->_ps = kw * 1.36;
    return this;
}


Car_Builder* Car_Builder::driven_kilometers(unsigned int km){
    this->_driven_kilometers = km;
    return this;
} 


Car_Builder* Car_Builder::purchase_value(double value){
    this->_purchase_value = value;
    return this;
} 


Car_Builder* Car_Builder::fuel_type(Fuel_Type fuel_type){
    this->_fuel_type = fuel_type;
    return this;
} 


Car_Builder* Car_Builder::car_type(Car_Types car_type){
    this->_car_type = car_type;
    return this;
} 


Car_Builder* Car_Builder::brand(Car_Brands brand){
    this->_brand = brand;
    return this;
} 


Car_Builder* Car_Builder::convert_car_to_builder(Car car) {
    this->_ps = car.get_ps();
    this->_driven_kilometers = car.get_driven_kilometers();
    this->_purchase_value = car.get_purchase_value();
    this->_car_type = car.get_car_type();
    this->_brand = car.get_brand();
    this->_fuel_type = car.get_fuel_type();

    return this;
} 


optional<Car> Car_Builder::build(){
    if (   this->_ps == 0
        || this->_purchase_value == 0
    ) {
        return nullopt;
    } else {
        return Car(this->_ps, this->_purchase_value, this->_driven_kilometers,
               this->_car_type, this->_brand, this->_fuel_type);
    }
}