enum Car_Types {
    sedan,
    coupe,
    sports_car,
    station_wagon,
    hatchback,
    convertible,
    suv,
    minivan,
    pickup_truck
};


enum Car_Brands {
    vw,
    audi,
    mercedes,
    bmw,
    skoda,
    seat
};

class Leasing_Calculation {

    short unsigned int ps{150};
    short unsigned int leasing_duration;
    short unsigned int insurance_class{0};
    double purchase_value{42000};
    double rest_value;
    double annual_interest_rate{2.2};
    bool is_over_24{true};
       
    Car_Types car_type;
    Car_Brands brand;

    public:

    double calculate_leasing(); //https://www.leasingmarkt.de/ratgeber/leasing-lexikon/leasingrechner
    double calculate_insurance();
};