#pragma once

#include <optional>

#include <json.hpp>
#include <toml.hpp>

namespace config {

    struct Server {
        std::string ip{"127.0.0.1"};
        short unsigned int port{1113};
        bool allow_shutdown{false};

        std::string get_port_as_string();
        std::string get_grpc_port();
        bool validate_localhost();
        bool validate_ip_address();
    };

    struct Log_Settings {

        std::string log_file{};
        bool log_to_file{false};
        bool log_level_debug{false};

        void config_logger();
        void print_logger_config();
    };


    std::optional<nlohmann::json> validate_json(std::string);

    std::optional<toml::table> validate_toml(std::string);

    bool config_from_json(nlohmann::json, Server&, Log_Settings&);

    bool config_from_toml(toml::table, Server&, Log_Settings&);

    // creates a new process and starts the server. returns the pid of the new process
    int start_server(bool, Server, Log_Settings, std::string, std::string);
}