#include <sstream>

#include "pystring.h"

#include "message_utility.h"

using namespace std;


string message_utility::to_ascii(string data)
{
    stringstream sstream;

    for(char ch : data) {
        sstream << " " << (int)ch;
    }
    sstream << "\n";

    return sstream.str();
}


string message_utility::from_ascii(string data) {
    
    data = pystring::lstrip(data);
    stringstream sstream{};
    string new_string{};
    string temp{};

    sstream << data;

    while (sstream >> temp) {
        new_string.push_back((char) stoi(temp));
    }
    
    return new_string;
}