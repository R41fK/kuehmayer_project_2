#include <cmath>
#include <optional>
#include <climits>
#include <cfloat>

#include "leasing_calculation.h"

using namespace std;

optional<double> Leasing_Calculation::calculate_insurance() {

    if (!car || this->insurance_class == USHRT_MAX) {
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


optional<double> Leasing_Calculation::calculate_leasing_rate() {

    if (  !car 
       || this->rest_value == DBL_MAX 
       || this->leasing_duration == 0 
       || this->deposit == DBL_MAX) 
    {
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


void Leasing_Calculation::set_car(Car car) {
    this->car = car;
}


void Leasing_Calculation::set_leasing_duration(short unsigned int leasing_duration) {
    this->leasing_duration = leasing_duration;
}


void Leasing_Calculation::set_insurance_class(short unsigned int insurance_class) {
    this->insurance_class = insurance_class;
}


void Leasing_Calculation::set_rest_value(double rest_value) {
    this->rest_value = rest_value;
}


void Leasing_Calculation::set_deposit(double deposit) {
    this->deposit = deposit;
}


void Leasing_Calculation::is_under_24() {
    this->_is_over_24 = false;
}


void Leasing_Calculation::is_over_24() {
    this->_is_over_24 = true;
}