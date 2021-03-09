#include <doctest.h>

#include "message_utility.h"

using namespace std;


TEST_CASE("To Ascii") {

    string data{message_utility::to_ascii("abcABC %")};

    CHECK(data == " 97 98 99 65 66 67 32 37\n");
}


TEST_CASE("From Ascii") {

    string data{message_utility::from_ascii(" 97 98 99 65 66 67 32 37")};

    CHECK(data == "abcABC %");
}