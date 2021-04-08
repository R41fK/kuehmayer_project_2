#include "json.hpp"
using namespace nlohmann;

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <map>
#include <bitset>
#include <asio.hpp>
#include <thread>

#include <fmt/core.h>
#include <fmt/color.h>
#include <peglib.h>
#include "pystring.h"
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

#include "message.pb.h"

#include "message_utility.h"
#include "client/repl.h"
#include "car.h"
#include "car_builder.h"
#include "car_calculator.h"


using namespace std;
using namespace peg;


void Repl::show_help() {
    fmt::print(
    R"(Commands: All commands are caseinsensitive!
    help | h                                        shows this help message
    end | stop | exit                               stops the programm
    ls | show | show all                            shows alle objectnames
    file                                            run a skript with command
    shutdown                                        closes the client and if allowed, the server

    car_calculator <calculator_name>                creates a car_calculator with the name <calculator_name>
    <calculator_name> show car_calculator           shows the car_calculator object for the object with the name <calculator_name>
    <calculator_name> car = <car_name>              sets the car for the car_calculator with the name <calculator_name>
    <calculator_name> leasing_duration = <int>      sets the leasing_duration for the car_calculator with the name <calculator_name> 
    <calculator_name> insurance_class = <int>       sets the insurance_class for the car_calculator with the name <calculator_name>
    <calculator_name> rest_value = <double>         sets the rest_value for the car_calculator with the name <calculator_name>
    <calculator_name> deposit = <double>            sets the deposite for the car_calculator with the name <calculator_name>
    <calculator_name> is_under_24                   sets that the person is under 24 for the car_calculator with the name <calculator_name>
    <calculator_name> is_over_24                    sets that the person is over 24 for the car_calculator with the name <calculator_name>
    <calculator_name> calculate_leasing             calculates the leasint rate. Needs: car, leasing_duration, rest_value, deposit
    <calculator_name> calculate_insurance           calculates the insurance rate. Needs: car, insurance_class, is_under_24 or is_over_24

    car_builder <builder_name>                      creates a car_builder with the name <builder_name>
    <builder_name> show car_builder                 shows the car_builder object for the object with the name <builder_name>
    <builder_name> ps = <int>                       sets the ps for the car_builder with the name <builder_name>
    <builder_name> purchase_value = <double>        sets the purchase_value for the car_builder with the name <builder_name>
    <builder_name> driven_kilometer = <int>         sets the driven_kilometer for the car_builder with the name <builder_name>
    <builder_name> fuel_type = <fuel_types>         sets the fuel_type for the car_builder with the name <builder_name>
    <builder_name> car_brand = <car_brands>         sets the car_brand for the car_builder with the name <builder_name>
    <builder_name> car_type = <car_type>            sets the car_type for the car_builder with the name <builder_name>

    car <car_name> = <builder_name> build           creates a car with the name <car_name> from a car_builder with the name <builder_name>
    <car_name> show car                             shows the car object for the object with the name <car_name>
    <car_name> ps                                   shows the ps for the object with the name <car_name>
    <car_name> kw                                   shows the kw for the object with the name <car_name>
    <car_name> purchase_value                       shows the purchase_value for the object with the name <car_name>
    <car_name> driven_kilometers                    shows the driven_kilometers for the object with the name <car_name>
    <car_name> car_type                             shows the car_type for the object with the name <car_name>
    <car_name> brand                                shows the brand for the object with the name <car_name>
    <car_name> fuel_type                            shows the fuel_type for the object with the name <car_name>

    <fuel_types> = petrol | diesel | natural_gas | electric
    <car_brands> = vw | audi | mercedes | bmw | skoda | seat
    <car_type>   = sedan | coupe | sports_car | hatchback | suv | minivan | pickup_truck

)"
    );
}


void Repl::no_Car_Builder(string s) {
    spdlog::debug(fmt::format("No Car_Builder with this name: {}", s));
    fmt::print(fg(fmt::color::orange), "No Car_Builder with this name: {}\n", s);
}

void Repl::no_Car(string s) {
    spdlog::debug(fmt::format("No Car with this name: {}", s));
    fmt::print(fg(fmt::color::orange), "No Car with this name: {}\n", s);
}

void Repl::no_Car_Calculator(string s) {
    spdlog::debug(fmt::format("No Car_Calculator with this name: {}", s));
    fmt::print(fg(fmt::color::orange), "No Car_Calculator with this name: {}\n", s);
}


void Repl::file() {
    string input{};

    fmt::print("Filepath: ");
    getline(cin, input);

    ifstream infile(input.c_str());

    if (infile.good()) {

        string line{};
        while (getline(infile, line) && this->running) {
            fmt::print("{}\n", line);

            line = pystring::lower(line);

            line = pystring::strip(line);

            spdlog::debug(fmt::format("File input: {}", line));

            this->parser.parse(line.c_str());

            this_thread::sleep_for(chrono::seconds(1));
        }
    } else {
        fmt::print("File {} does not exist\n", input);

        spdlog::info(fmt::format("File {} does not exist", input));
    }
}


void Repl::send_message(string msg) {

    if (*strm) {
    
        spdlog::debug(fmt::format("Client encodes message '{}'", msg));

        string dec_msg{message_utility::to_ascii(msg)};

        spdlog::debug(fmt::format("Client sends encoded message '{}'", dec_msg));

        this->messages.push_back(dec_msg);

        *strm << dec_msg;

        string data;

        getline(*strm, data);

        spdlog::debug(fmt::format("Client got message '{}'", data));

        data = message_utility::from_ascii(data);

        spdlog::debug(fmt::format("Client decoded message '{}'", data));

        if (data != "ok" && data != "") {
            fmt::print(fg(fmt::color::alice_blue), "{}\n", data);
        }
        
    } else {

        spdlog::info(fmt::format("Connection to the server with ip: {} and port: {} closed.", server_data.ip, server_data.port));
        fmt::print(fg(fmt::color::orange), "Connection to the server closed. Reastablish connection? (y,n): ");

        string input{};

        getline(cin, input);

        input = pystring::lower(input);

        input = pystring::strip(input);

        if (pystring::startswith(input, "y")) {
            spdlog::info(fmt::format("Create a connection on ip: {} with port: {}", server_data.ip, server_data.port));
            this->strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};

            if (*this->strm) {
                fmt::print(fg(fmt::color::orange), "Connection reastablished! Updating Server...\n");
            
                for (string st : messages) {
                    if (*this->strm) {
                        *this->strm << st;

                        getline(*this->strm, input);

                    } else {
                        spdlog::info(fmt::format("Connecting to the server with ip: {} and port: {} failed"
                                        , server_data.ip
                                        , server_data.port)
                                    );
                        fmt::print(fg(fmt::color::red), "Reconnecting failed! Stopping client!\n");
                        this->stop();

                        break;
                    }
                }

                fmt::print(fg(fmt::color::aquamarine), "Server updated\n");

            } else {
                spdlog::info(fmt::format("Connecting to the server with ip: {} and port: {} failed"
                                        , server_data.ip
                                        , server_data.port)
                            );
                fmt::print(fg(fmt::color::red), "Reconnection failed! Stopping client!\n");
                this->stop();
            }
        } else {
            fmt::print(fg(fmt::color::red), "No connection to server. Stopping client!\n");
            this->stop();
        }   
    }
}


void Repl::stop() {
    fmt::print(fg(fmt::color::orange), "Stoped\n");
    this->running = false;
    strm->close();
    spdlog::info(fmt::format("User send stop signal"));
}

void Repl::operator()() {

    string input{};

    send_message(input);

    if (this->running) {
        fmt::print("Started ... for the comands type help or h. All commands are caseinsensitive!\n");
    }

    while (this->running) {
        
        getline(cin, input);

        input = pystring::lower(input);

        input = pystring::strip(input);

        spdlog::debug(fmt::format("User input: {}", input));

        if (input == "file") {
            file();
        } else {
            parser.parse(input.c_str());
        }
    }
}