#include <sstream>

#include "pystring.h"

#include "message_utility.h"

using namespace std;


string message_utility::to_hex(string data)
{
    stringstream sstream;

    for(char ch : data) {
        sstream << " " << std::hex << (int)ch;
    }

    return sstream.str();
}


string message_utility::from_hex(string data) {
    
    data = pystring::lstrip(data);
    stringstream sstream{};
    string new_string{}, temp{};

    sstream << data;

    while (sstream >> temp) {
        new_string.push_back((char) stol(temp, 0, 16));
    }
    
    return new_string;
}