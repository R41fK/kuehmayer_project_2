#pragma once

#include <optional>

#include <json.hpp>
#include <toml.hpp>

std::optional<nlohmann::json> validate_json(std::string);

std::optional<toml::table> validate_toml(std::string);


struct Server {
    std::string ip{"127.0.0.1"};
    short unsigned int port{1113};

    std::string get_port_as_string();
};

struct Log_Settings {

    std::string log_file{"logs/control.log"};
    bool log_to_file{false};
    bool log_level_debug{false};

    void config_logger();
};