#include <cmath>

#include "leasing_calculation.h"

using namespace std;

double Leasing_Calculation::calculate_insurance(){
    return  ((this->purchase_value / 10000)
            + (this->ps / 2)
            + (pow(1.005, this->purchase_value/1000) * 10)
            + (pow(1.01, this->ps/2) * 10))
            * (1 + 0.015 * this->insurance_class)
            * (1 + !this->is_over_24 * 0.075)
        ;
}