#include <cmath>
#include <optional>
#include <climits>
#include <cfloat>
#include <sstream>

#include <fmt/core.h>
#include "message.pb.h"

#include "car_calculator.h"

using namespace std;

optional<double> Car_Calculator::calculate_insurance_rate() {

    if (  !car 
       || this->insurance_class == USHRT_MAX
       ) {
        return nullopt;
    } else {
        return  ((this->car.get_purchase_value() / 8500)
            + (this->car.get_ps() / 2)
            + (pow(1.005, this->car.get_purchase_value() / 1000) * 10)
            + (pow(1.01, this->car.get_ps() / 2) * 10))
            * (1 + 0.015 * this->insurance_class)
            * (1 + !this->_is_over_24 * 0.075)
        ;
    }
}


optional<double> Car_Calculator::calculate_leasing_rate() {

    if (  !car 
       || this->rest_value == DBL_MAX 
       || this->leasing_duration == 0 
       || this->deposit == DBL_MAX
       ) {
        return nullopt;
    } else {
        double mont_amor{
            (car.get_purchase_value() - this->deposit - this->rest_value) 
            / (this->leasing_duration * 12)
        };

        double mont_interest{
            ((car.get_purchase_value() + this->rest_value) / 2)
            * (this->annual_interest_rate / 12)
        };

        return mont_amor + mont_interest;
    }
}


void Car_Calculator::set_car(Car car) {
    this->car = car;
}


void Car_Calculator::set_leasing_duration(short unsigned int leasing_duration) {
    this->leasing_duration = leasing_duration;
}


void Car_Calculator::set_insurance_class(short unsigned int insurance_class) {
    this->insurance_class = insurance_class;
}


void Car_Calculator::set_rest_value(double rest_value) {
    this->rest_value = rest_value;
}


void Car_Calculator::set_deposit(double deposit) {
    this->deposit = deposit;
}


void Car_Calculator::is_under_24() {
    this->_is_over_24 = false;
}


void Car_Calculator::is_over_24() {
    this->_is_over_24 = true;
}


string Car_Calculator::to_string() {
    stringstream strm{};
    strm << "Car_Calculator(Car=" << car.to_string()
         << ", leasing_duration=" << leasing_duration
         << ", insurance_class=" << insurance_class
         << ", rest_value=" << rest_value
         << ", deposit=" << deposit
         << ", annual_interest_rate=" << annual_interest_rate
         << ", is_over_24=" << _is_over_24
         << ")";

    return strm.str();
}


string Car_Calculator::get_proto_message(std::string name) {

    Message_Car* msg_car = new Message_Car();

    msg_car->set_driven_kilometers(this->car.get_driven_kilometers());

    msg_car->set_ps(this->car.get_ps());

    msg_car->set_purchase_value(this->car.get_purchase_value());

    switch (this->car.get_brand()) {
        case Car_Brands::AUDI:
            msg_car->set_brand(Message_Car_Brands::AUDI);
            break;
        
        case Car_Brands::BMW:
            msg_car->set_brand(Message_Car_Brands::BMW);
            break;
        
        case Car_Brands::MERCEDES:
            msg_car->set_brand(Message_Car_Brands::MERCEDES);
            break;

        case Car_Brands::SEAT:
            msg_car->set_brand(Message_Car_Brands::SEAT);
            break;

        case Car_Brands::SKODA:
            msg_car->set_brand(Message_Car_Brands::SKODA);
            break;

        case Car_Brands::VW:
            msg_car->set_brand(Message_Car_Brands::VW);
            break;

        default:
            msg_car->set_brand(Message_Car_Brands::BRAND_NONE);
    }

    switch (this->car.get_car_type()) {
        case Car_Types::COUPE:
            msg_car->set_car_type(Message_Car_Types::COUPE);
            break;

        case Car_Types::HATCHBACK:
            msg_car->set_car_type(Message_Car_Types::HATCHBACK);
            break;

        case Car_Types::MINIVAN:
            msg_car->set_car_type(Message_Car_Types::MINIVAN);
            break;

        case Car_Types::PICKUP_TRUCK:
            msg_car->set_car_type(Message_Car_Types::PICKUP_TRUCK);
            break;

        case Car_Types::SEDAN:
            msg_car->set_car_type(Message_Car_Types::SEDAN);
            break;

        case Car_Types::SPORTS_CAR:
            msg_car->set_car_type(Message_Car_Types::SPORTS_CAR);
            break;

        case Car_Types::SUV:
            msg_car->set_car_type(Message_Car_Types::SUV);
            break;

        default:
            msg_car->set_car_type(Message_Car_Types::CAR_NONE);
    }

    switch (this->car.get_fuel_type()) {
        case Fuel_Type::DIESEL:
            msg_car->set_fuel_type(Message_Fuel_Type::DIESEL);
            break;

        case Fuel_Type::ELECTRONIC:
            msg_car->set_fuel_type(Message_Fuel_Type::ELECTRONIC);
            break;

        case Fuel_Type::NATURAL_GAS:
            msg_car->set_fuel_type(Message_Fuel_Type::NATURAL_GAS);
            break;

        case Fuel_Type::PETROL:
            msg_car->set_fuel_type(Message_Fuel_Type::PETROL);
            break;

        default:
            msg_car->set_fuel_type(Message_Fuel_Type::FUEL_NONE);
    }

    Message_Car_Calculator* msg_calculator = new Message_Car_Calculator();

    msg_calculator->set_deposit(this->deposit);

    msg_calculator->set_insurance_class(this->insurance_class);

    msg_calculator->set_annual_interest_rate(this->annual_interest_rate);

    msg_calculator->set_is_over_24(this->_is_over_24);

    msg_calculator->set_leasing_duration(this->leasing_duration);

    msg_calculator->set_rest_value(this->rest_value);

    msg_calculator->set_allocated_car(msg_car);

    Message msg{};

    msg.set_type(Message::MessageType::Message_MessageType_CALCULATOR);

    msg.set_name(name);

    msg.set_allocated_calculator(msg_calculator);

    return msg.SerializeAsString();
}


void Car_Calculator::update_car_calculator_from_proto_message(Message_Car_Calculator msg) {
    this->deposit = msg.deposit();
    
    this->insurance_class = msg.insurance_class();

    this->leasing_duration = msg.leasing_duration();

    this->annual_interest_rate = msg.annual_interest_rate();

    this->rest_value = msg.rest_value();

    this->_is_over_24 = msg.is_over_24();

    this->car = Car(msg.car());
}