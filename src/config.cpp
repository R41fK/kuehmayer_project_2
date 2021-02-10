
#include <json.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <toml.hpp>

#include "config.h"

using namespace std;

void Log_Settings::config_logger(){

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


string Server::get_port_as_string(){
    return to_string(this->port);
}