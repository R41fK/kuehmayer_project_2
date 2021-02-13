#pragma once

enum class Car_Types {
    SEDAN,
    COUPE,
    SPORTS_CAR,
    STATION_WAGON,
    HATCHBACK,
    CONVERTIBLE,
    SUV,
    MINIVAN,
    PICKUP_TRUCK,
    NONE
};


enum class Car_Brands {
    VW,
    AUDI,
    MERCEDES,
    BMW,
    SKODA,
    SEAT,
    NONE
};

enum class Fuel_Type {
    PETROL,
    DIESEL,
    NATURAL_GAS,
    ELECTRONIC,
    NONE
};

class Car {
private:
    short unsigned int ps;
    unsigned int driven_kilometers;
    double purchase_value;

    Fuel_Type fuel_type;
    Car_Types car_type;
    Car_Brands brand;

public:

    Car(short unsigned int ps, double purchase_value, unsigned int driven_kilometers
            , Car_Types car_type, Car_Brands brand, Fuel_Type fuel_type) :
        ps{ps},
        driven_kilometers{driven_kilometers},
        purchase_value{purchase_value},
        fuel_type{fuel_type},
        car_type{car_type},
        brand{brand}
        {};

    short unsigned int get_ps();
    short unsigned int get_kw();
    double get_purchase_value();
    unsigned int get_driven_kilometers();
    Car_Types get_car_type();
    Car_Brands get_brand();
    Fuel_Type get_fuel_type();

    explicit operator bool() {
        return ps != 0 && purchase_value != 0;
    }
};