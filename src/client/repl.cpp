#include <iostream>
#include <algorithm>
#include <string>

#include "peglib.h"
#include "spdlog/spdlog.h"
#include "Repl.h"

using namespace std;
using namespace peg;


void Repl::show_help() {
    
}




void Repl::stop() {
    this->running = false;


}

void Repl::operator()() {

    string input{};

    parser parser;

    parser = (R"(
        Start    <- 'help' / Call / Move / Override / 'end'
        Override    <- 'override' Call / 'override' Move
        Call        <- 'call' Number
        Move        <- 'move' Number Number
        Number      <- < [0-9]+ >
        %whitespace <- [ \t]*
    )");
    

    while (this->running) {
        
        getline(cin, input);


        for_each(input.begin(), input.end(), [](char & c) {
            c = ::tolower(c);
        });
        
        input.erase(0, input.find_first_not_of(" \n\r\t"));
        input.erase(input.find_last_not_of(" \n\r\t")+1);

        if (parser.parse(input.c_str())) {
            if (input.rfind("override", 0) == 0) {
                override = true;
                input.erase(0, 9);
            } else {
                override = false;
            }

            if (input.rfind("help", 0) == 0) {
                this->file_logger->debug("User input help");
                show_help();
            } else if (input.rfind("call", 0) == 0) {
                call(input.substr(input.find_first_of("0123456789"), input.find(" ")), override);

            } else if (input.rfind("move", 0) == 0) {
                size_t first_pos{input.find_first_of("0123456789")};
                size_t last_pos{input.find(" ", first_pos)};
                size_t elevator_pos{input.find_first_of("0123456789", last_pos+1)};
                size_t elevator_pos_last{input.find(" ", elevator_pos)};

                move(input.substr(elevator_pos, elevator_pos_last - elevator_pos), input.substr(first_pos, last_pos - first_pos), override);

            } else if (input.rfind("end", 0) == 0) {
                stop();
                spdlog::info("Stoped the Repl. The elevator(s) are finishing there queue and than the program closes");
                this->file_logger->info("User input to stop the program");
            }
        } else {
            this->file_logger->debug("User input " + input + " no command");
            spdlog::warn("No Command: " + input);
            show_help();
        }        
    }
}