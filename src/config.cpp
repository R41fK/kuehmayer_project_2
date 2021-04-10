#include <iostream>
#include <fstream>
#include <optional>

#include <json.hpp>
#include "pystring.h"
#include <rang.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <toml.hpp>

#include "config.h"


using json = nlohmann::json;
using namespace std;

optional<json> config::validate_json(string file_name) {

    try {
        json j;

        ifstream i{file_name};
        i >> j;
        return j;

    } catch (const nlohmann::detail::parse_error& err) {

        string error_message{err.what()};

        cerr << "Not Valid JSON: " + error_message << endl;

        return nullopt;
    }
}


optional<toml::table> config::validate_toml(string file_name) {

    try {
        toml::table t =  toml::parse_file(file_name);

        return t;

    } catch (const toml::parse_error& err) {
        
        string error_message{err.what()};

        cerr << "Not Valid TOML: " + error_message << endl;

        return nullopt;
    }
}


bool config::config_from_json(nlohmann::json json, config::Server& server_data, config::Log_Settings& log_settings) {

    if (json.contains("port")) {
        if (json["port"].is_number_integer() && json["port"].is_number_unsigned()) {
            server_data.port = json["port"];
        } else {
            cerr << "Port has to be a unsinged short integer" << endl;
            return false;
        }
    }

    if (json.contains("server-ip")) {
        if (json["server-ip"].is_string() && json["server-ip"] != "") {
            server_data.ip = json["server-ip"];
        } else {
            cerr << "server-ip has to be a string that is not empty" << endl;
            return false;
        }
    }

    if (json.contains("log-to-file")) {
        if (json["log-to-file"].is_boolean()) {
            log_settings.log_to_file = json["log-to-file"];
        } else {
            cerr << "log-to-file has to be a boolean" << endl;
            return false;
        }
    }

    if (json.contains("log-level-debug") && log_settings.log_to_file) {
        if (json["log-level-debug"].is_boolean()) {
            log_settings.log_level_debug = json["log-level-debug"];
        } else {
            cerr << "log-level-debug has to be a boolean" << endl;
            return false;
        }
    } else if(json.contains("log-level-debug")) {
        cerr << "log-level-debug cant be set without setting log-to-file true" << endl;
        return false;
    }

    if (json.contains("log-file") && log_settings.log_to_file) {
        if (json["log-file"].is_string() && json["log-file"] != "") {
            log_settings.log_file = json["log-file"];
        } else {
            cerr << "log-file has to be a string that is not empty" << endl;
            return false;
        }
    } else if(json.contains("log-file")) {
        cerr << "log-file cant be set without setting log-to-file true" << endl;
        return false;
    }
    return true;
}


bool config::config_from_toml(toml::table toml, config::Server& server_data, config::Log_Settings& log_settings) {

    if (toml["Server"]["port"]) {
        if (toml["Server"]["port"].value<short unsigned int>().has_value()) {
            server_data.port = toml["Server"]["port"].value<short unsigned int>().value();
        } else {
            cerr << "Port has to be a unsinged short integer" << endl;
            return false;
        }
    }

    if (toml["Server"]["server-ip"]) {
        if (toml["Server"]["server-ip"].value<string>().has_value()) {
            server_data.ip = toml["Server"]["server-ip"].value<string>().value();
        } else {
            cerr << "server-ip has to be a string that is not empty" << endl;
            return false;
        }
    }

    if (toml["Logging"]["log-to-file"]) {
        if (toml["Logging"]["log-to-file"].value<bool>().has_value()) {
            log_settings.log_to_file = toml["Logging"]["log-to-file"].value<bool>().value();
        } else {
            cerr << "log-to-file has to be a boolean" << endl;
            return false;
        }
    }

    if (toml["Logging"]["log-level-debug"] && log_settings.log_to_file) {
        if (toml["Logging"]["log-level-debug"].value<bool>().has_value()) {
            log_settings.log_level_debug = toml["Logging"]["log-level-debug"].value<bool>().value();
        } else {
            cerr << "log-level-debug has to be a boolean" << endl;
            return false;
        }
    } else if(toml["Logging"]["log-level-debug"]) {
        cerr << "log-level-debug cant be set without setting log-to-file true" << endl;
        return false;
    }

    if (toml["Logging"]["log-file"] && log_settings.log_to_file) {
        if (toml["Logging"]["log-file"].value<string>().has_value()) {
            log_settings.log_file = toml["Logging"]["log-file"].value<string>().value();
        } else {
            cerr << "log-file has to be a string that is not empty" << endl;
            return false;
        }
    } else if(toml["Logging"]["log-file"]) {
        cerr << "log-file cant be set without setting log-to-file true" << endl;
        return false;
    }

    return true;
}


int config::start_server(bool start_server, Server server_data, Log_Settings logger_settings, string config_file_json, string config_file_toml) {
    int pid{};
    if (start_server) {
        if (server_data.validate_localhost()) {
            pid = fork();

            if (pid == 0) {
                vector<char*> args;

                args.push_back((char*)"server");
                args.push_back((char*)"-p");
                args.push_back((char*)server_data.get_port_as_string().c_str());

                if (logger_settings.log_to_file) {
                    args.push_back((char*)"-l");
                    if (logger_settings.log_level_debug) {
                        args.push_back((char*)"-d");
                    }

                    args.push_back((char*)"--log-file");
                    args.push_back((char*) logger_settings.log_file.c_str());
                }

                if (config_file_json != "") {
                    args.push_back((char*)"-j");
                    args.push_back((char*) config_file_json.c_str());
                } else if (config_file_toml != "") {
                    args.push_back((char*)"-t");
                    args.push_back((char*) config_file_toml.c_str());
                }

                args.push_back((char*)"-e");
                args.push_back(nullptr);

                execv("./server",  &args[0]);
            } else {
                this_thread::sleep_for(chrono::milliseconds(500));
            }
        } else {
            return INT_MIN;
        }          
    }
    
    return pid;
}


void config::Log_Settings::config_logger() {

    if (this->log_to_file) {
        //Create rotating file multi-threaded logger
        try {
            auto file_logger = spdlog::rotating_logger_mt("file_logger", this->log_file, 1048576 * 10, 3);

            file_logger->set_pattern("[%H:%M:%S] [thread %t] [%^%l%$] %v");


            if (this->log_level_debug) {
                file_logger->set_level(spdlog::level::debug);
                file_logger->flush_on(spdlog::level::debug);
            } else {
                file_logger->set_level(spdlog::level::info);
                file_logger->flush_on(spdlog::level::info);
            }

            spdlog::set_default_logger(file_logger);

        } catch (spdlog::spdlog_ex& e) {
            cerr << "Error can't create Logger!" << endl << e.what() << endl;
            exit(1);
        }
    
    } else {
        spdlog::set_level(spdlog::level::off);
        spdlog::flush_on(spdlog::level::off);
    }
}


void config::Log_Settings::print_logger_config() {
    if (this->log_to_file) {
        cout << endl;
        cout << rang::style::bold << rang::fg::yellow
             << "Logging activated" << rang::fg::green
             << " with the " << rang::fg::yellow
             << " logging-level ";
        if (this->log_level_debug) {
            cout << "debug";
        } else {
            cout << "info";
        }
        cout << rang::fg::green << ". The logger writes in the file "
             << rang::fg::yellow << this->log_file;

        cout << rang::style::reset 
         << rang::fg::reset << endl;
    }
    cout << endl;


    for (int i=0; i < 79; i++) {
        cout << "-";
    }

    cout << rang::style::reset 
         << rang::fg::reset 
         << endl;
}



string config::Server::get_port_as_string() {
    return to_string(this->port);
}

string config::Server::get_grpc_port() {
    return to_string(this->port + 1);
}

bool config::Server::validate_ip_address() {

    if (pystring::isalpha(this->ip)) { // return true if it is a domain name... can't check them
        return true;
    }

    if (pystring::count(this->ip, ".") != 3
       || pystring::endswith(this->ip, ".0")
       || pystring::endswith(this->ip, ".255")
       ) {
        return false;
    }

    vector<string> str{};
    pystring::split(this->ip, str, ".");

    for (size_t i=0; i < str.size(); ++i) {
        if (pystring::isdigit(str[i]) && str[i].length() <= 3) {
            int numb{stoi(str[i])};
            if (numb > 255 || numb < 0) {
                return false;
            }
        } else {
            return false;
        }
    }

    return true;
}


bool config::Server::validate_localhost() {
    if (this->ip == "localhost") {
        return true;
    }

    if (!pystring::startswith(this->ip, "127.") 
       || pystring::isalpha(this->ip)
       ) {
        return false;
    }
    
    return this->validate_ip_address();
}