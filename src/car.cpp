
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

string Car::get_proto_message(string name) {

    Message_Car* msg_car = new Message_Car();

    msg_car->set_driven_kilometers(this->driven_kilometers);

    msg_car->set_ps(this->ps);

    msg_car->set_purchase_value(this->purchase_value);

    switch (this->brand) {
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

    switch (this->car_type) {
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

    switch (this->fuel_type) {
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

    Message msg{};

    msg.set_type(Message::MessageType::Message_MessageType_CAR);

    msg.set_name(name);

    msg.set_allocated_car(msg_car);

    return msg.SerializeAsString();
}

void Car::update_car_from_proto_message(Message_Car msg) {

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