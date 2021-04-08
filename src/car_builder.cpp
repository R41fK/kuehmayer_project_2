#include <sstream>

#include <magic_enum.hpp>

#include "message.pb.h"

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


bool Car_Builder::operator==(const Car_Builder car_builder) const {
    return this->_ps == car_builder._ps
        && this->_purchase_value == car_builder._purchase_value
        && this->_driven_kilometers == car_builder._driven_kilometers
        && this->_fuel_type == car_builder._fuel_type
        && this->_car_type == car_builder._car_type
        && this->_brand == car_builder._brand
        ;
}

std::string Car_Builder::to_string() {

    stringstream strm{};
    strm << "Car_Builder(ps=" << _ps
         << ", purchase_value=" << _purchase_value
         << ", driven_kilometers=" << _driven_kilometers
         << ", brand=" << magic_enum::enum_name(_brand)
         << ", fuel_type=" << magic_enum::enum_name(_fuel_type)
         << ", car_type=" << magic_enum::enum_name(_car_type)
         << ")";

    return strm.str();
}


std::string Car_Builder::get_proto_message(std::string name, bool reply) {
    Message_Car* msg_builder = new Message_Car();

    msg_builder->set_driven_kilometers(this->_driven_kilometers);

    msg_builder->set_ps(this->_ps);

    msg_builder->set_purchase_value(this->_purchase_value);

    switch (this->_brand) {
        case Car_Brands::AUDI:
            msg_builder->set_brand(Message_Car_Brands::AUDI);
            break;
        
        case Car_Brands::BMW:
            msg_builder->set_brand(Message_Car_Brands::BMW);
            break;
        
        case Car_Brands::MERCEDES:
            msg_builder->set_brand(Message_Car_Brands::MERCEDES);
            break;

        case Car_Brands::SEAT:
            msg_builder->set_brand(Message_Car_Brands::SEAT);
            break;

        case Car_Brands::SKODA:
            msg_builder->set_brand(Message_Car_Brands::SKODA);
            break;

        case Car_Brands::VW:
            msg_builder->set_brand(Message_Car_Brands::VW);
            break;

        default:
            msg_builder->set_brand(Message_Car_Brands::BRAND_NONE);
    }

    switch (this->_car_type) {
        case Car_Types::COUPE:
            msg_builder->set_car_type(Message_Car_Types::COUPE);
            break;

        case Car_Types::HATCHBACK:
            msg_builder->set_car_type(Message_Car_Types::HATCHBACK);
            break;

        case Car_Types::MINIVAN:
            msg_builder->set_car_type(Message_Car_Types::MINIVAN);
            break;

        case Car_Types::PICKUP_TRUCK:
            msg_builder->set_car_type(Message_Car_Types::PICKUP_TRUCK);
            break;

        case Car_Types::SEDAN:
            msg_builder->set_car_type(Message_Car_Types::SEDAN);
            break;

        case Car_Types::SPORTS_CAR:
            msg_builder->set_car_type(Message_Car_Types::SPORTS_CAR);
            break;

        case Car_Types::SUV:
            msg_builder->set_car_type(Message_Car_Types::SUV);
            break;

        default:
            msg_builder->set_car_type(Message_Car_Types::CAR_NONE);
    }

    switch (this->_fuel_type) {
        case Fuel_Type::DIESEL:
            msg_builder->set_fuel_type(Message_Fuel_Type::DIESEL);
            break;

        case Fuel_Type::ELECTRONIC:
            msg_builder->set_fuel_type(Message_Fuel_Type::ELECTRONIC);
            break;

        case Fuel_Type::NATURAL_GAS:
            msg_builder->set_fuel_type(Message_Fuel_Type::NATURAL_GAS);
            break;

        case Fuel_Type::PETROL:
            msg_builder->set_fuel_type(Message_Fuel_Type::PETROL);
            break;

        default:
            msg_builder->set_fuel_type(Message_Fuel_Type::FUEL_NONE);
    }

    if (reply) {
        Reply msg{};

        msg.set_type(Reply::MessageType::Reply_MessageType_BUILDER);

        msg.set_text(name);

        msg.set_allocated_car(msg_builder);

        return msg.SerializeAsString();
    } else {
        Request msg{};

        msg.set_type(Request::MessageType::Request_MessageType_BUILDER);

        msg.set_name(name);

        msg.set_allocated_car(msg_builder);

        return msg.SerializeAsString();
    }
}

void Car_Builder::update_car_builder_from_proto_message(Message_Car msg) {

    this->_driven_kilometers = msg.driven_kilometers();

    this->_ps = msg.ps();

    this->_purchase_value = msg.purchase_value();

    switch (msg.brand()) {
        case Message_Car_Brands::AUDI:
            this->_brand = Car_Brands::AUDI;
            break;
        
        case Message_Car_Brands::BMW:
            this->_brand = Car_Brands::BMW;
            break;
        
        case Message_Car_Brands::MERCEDES:
            this->_brand = Car_Brands::MERCEDES;
            break;

        case Message_Car_Brands::SEAT:
            this->_brand = Car_Brands::SEAT;
            break;

        case Message_Car_Brands::SKODA:
            this->_brand = Car_Brands::SKODA;
            break;

        case Message_Car_Brands::VW:
           this->_brand = Car_Brands::VW;
            break;

        default:
           this->_brand = Car_Brands::NONE;
    }

    switch (msg.car_type()) {
        case Message_Car_Types::COUPE:
            this->_car_type = Car_Types::COUPE;
            break;

        case Message_Car_Types::HATCHBACK:
            this->_car_type = Car_Types::HATCHBACK;
            break;

        case Message_Car_Types::MINIVAN:
            this->_car_type = Car_Types::MINIVAN;
            break;

        case Message_Car_Types::PICKUP_TRUCK:
            this->_car_type = Car_Types::PICKUP_TRUCK;
            break;

        case Message_Car_Types::SEDAN:
            this->_car_type = Car_Types::SEDAN;
            break;

        case Message_Car_Types::SPORTS_CAR:
            this->_car_type = Car_Types::SPORTS_CAR;
            break;

        case Message_Car_Types::SUV:
            this->_car_type = Car_Types::SUV;
            break;

        default:
            this->_car_type = Car_Types::NONE;
    }

    switch (msg.fuel_type()) {
        case Message_Fuel_Type::DIESEL:
            this->_fuel_type = Fuel_Type::DIESEL;
            break;

        case Message_Fuel_Type::ELECTRONIC:
            this->_fuel_type = Fuel_Type::ELECTRONIC;
            break;

        case Message_Fuel_Type::NATURAL_GAS:
            this->_fuel_type = Fuel_Type::NATURAL_GAS;
            break;

        case Message_Fuel_Type::PETROL:
            this->_fuel_type = Fuel_Type::PETROL;
            break;

        default:
            this->_fuel_type = Fuel_Type::NONE;
    }
}