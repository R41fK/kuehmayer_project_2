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


string Car_Calculator::get_proto_message(string name, string car) {

    Message_Car_Calculator* msg_calculator = new Message_Car_Calculator();

    msg_calculator->set_deposit(this->deposit);

    msg_calculator->set_insurance_class(this->insurance_class);

    msg_calculator->set_annual_interest_rate(this->annual_interest_rate);

    msg_calculator->set_is_over_24(this->_is_over_24);

    msg_calculator->set_leasing_duration(this->leasing_duration);

    msg_calculator->set_rest_value(this->rest_value);

    msg_calculator->set_car(car);

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
}