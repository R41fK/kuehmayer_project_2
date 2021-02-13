#include <iostream>
#include <fstream>
#include <optional>

#include <json.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <toml.hpp>

#include "config.h"


using json = nlohmann::json;
using namespace std;

optional<json> validate_json(string file_name) {

    try {
        json j;

        ifstream i{file_name};
        i >> j;
        return j;

    } catch (const nlohmann::detail::parse_error& err) {

        string error_message{err.what()};

        cerr << "Not Valid JSON: " + error_message << endl;
        spdlog::error("Not Valid JSON: " + error_message);

        return nullopt;
    }
}


optional<toml::table> validate_toml(string file_name) {

    try {
        toml::table t =  toml::parse_file(file_name);

        return t;

    } catch (const toml::parse_error& err) {
        
        string error_message{err.what()};

        cerr << "Not Valid TOML: " + error_message << endl;
        spdlog::error("Not Valid TOML: " + error_message);

        return nullopt;
    }
}

void Log_Settings::config_logger() {

    //Create rotating file multi-threaded logger
    auto file_logger = spdlog::rotating_logger_mt("file_logger", this->log_file, 1048576 * 10, 3);
    
    file_logger->set_pattern("[%H:%M:%S] [thread %t] [%^%l%$] %v");

    if (this->log_to_file) {
        if (this->log_level_debug) {
            file_logger->set_level(spdlog::level::debug);
            file_logger->flush_on(spdlog::level::debug);
        } else {
            file_logger->set_level(spdlog::level::info);
            file_logger->flush_on(spdlog::level::info);
        }
    } else {
        file_logger->set_level(spdlog::level::off);
        file_logger->flush_on(spdlog::level::off);
    }

    spdlog::set_default_logger(file_logger);
}


string Server::get_port_as_string() {
    return to_string(this->port);
}