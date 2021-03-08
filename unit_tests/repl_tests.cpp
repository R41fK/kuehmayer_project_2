#include <doctest.h>
#include <peglib.h>

#include "client/repl.h"

using namespace std;
using namespace peg;


TEST_CASE("Repl gramar") {

    Repl repl{};

    auto ok = repl.parser.load_grammar(repl.grammar);

    CHECK(ok);
}


TEST_CASE("Repl parse") {

    Repl repl{};

    auto ok = repl.parser.load_grammar(repl.grammar);

    REQUIRE(ok);
    CHECK(repl.parser.parse("car_builder test"));
    CHECK(repl.parser.parse("test ps = 60"));
    CHECK(repl.parser.parse("test purchase_value = 1600.99"));
    CHECK(repl.parser.parse("test driven_kilometer = 17"));
    CHECK(repl.parser.parse("test fuel_type = petrol"));
    CHECK(repl.parser.parse("test fuel_type = diesel"));
    CHECK(repl.parser.parse("test fuel_type = natural_gas"));
    CHECK(repl.parser.parse("test fuel_type = electric"));
    CHECK(repl.parser.parse("test car_brand = vw"));
    CHECK(repl.parser.parse("test car_brand = audi"));
    CHECK(repl.parser.parse("test car_brand = mercedes"));
    CHECK(repl.parser.parse("test car_brand = bmw"));
    CHECK(repl.parser.parse("test car_brand = skoda"));
    CHECK(repl.parser.parse("test car_brand = seat"));
    CHECK(repl.parser.parse("test car_type = sedan"));
    CHECK(repl.parser.parse("test car_type = coupe"));
    CHECK(repl.parser.parse("test car_type = sports_car"));
    CHECK(repl.parser.parse("test car_type = hatchback"));
    CHECK(repl.parser.parse("test car_type = suv"));
    CHECK(repl.parser.parse("test car_type = minivan"));
    CHECK(repl.parser.parse("test car_type = pickup_truck"));
    CHECK(repl.parser.parse("car car = test build"));
    CHECK(repl.parser.parse("car ps"));
    CHECK(repl.parser.parse("car kw"));
    CHECK(repl.parser.parse("car purchase_value"));
    CHECK(repl.parser.parse("car driven_kilometers"));
    CHECK(repl.parser.parse("car car_type"));
    CHECK(repl.parser.parse("car brand"));
    CHECK(repl.parser.parse("car fuel_type"));
    CHECK(repl.parser.parse("car_calculator calc"));
    CHECK(repl.parser.parse("calc car = car"));
    CHECK(repl.parser.parse("calc leasing_duration = 5"));
    CHECK(repl.parser.parse("calc insurance_class = 5"));
    CHECK(repl.parser.parse("calc rest_value = 500.56"));
    CHECK(repl.parser.parse("calc deposit = 5648.41"));
    CHECK(repl.parser.parse("calc is_under_24"));
    CHECK(repl.parser.parse("calc is_over_24"));
    CHECK(repl.parser.parse("calc calculate_leasing"));
    CHECK(repl.parser.parse("calc calculate_insurance"));
    CHECK(repl.parser.parse("end"));
    CHECK(repl.parser.parse("stop"));
    CHECK(repl.parser.parse("help"));
    CHECK(repl.parser.parse("h"));
    CHECK(repl.parser.parse("car show car"));
    CHECK(repl.parser.parse("test show car_builder"));
    CHECK(repl.parser.parse("calc show car_calculator"));
}

TEST_CASE("Repl parse failed") {

    Repl repl{};

    auto ok = repl.parser.load_grammar(repl.grammar);

    REQUIRE(ok);
    CHECK_FALSE(repl.parser.parse("car_builder test dsd"));
    CHECK_FALSE(repl.parser.parse("calc leasing_duration = 5.6"));
    CHECK_FALSE(repl.parser.parse("calc rest_value = as"));
    CHECK_FALSE(repl.parser.parse("test car_brand = bock"));
}