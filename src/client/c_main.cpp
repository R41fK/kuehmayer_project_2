
#include <iostream>
#include <thread>
#include <optional>

#include <CLI11.hpp>
#include <fmt/core.h>
#include <json.hpp>
#include <rang.hpp>
#include <spdlog/spdlog.h>
#include <toml.hpp>

#include "config.h"
#include "car_calculator.h"
#include "car.h"
#include "car_builder.h"
#include "client/repl.h"

#include "message.pb.h"

using namespace asio;
using namespace std;

int main(int argc, char* argv[]) {

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Server server_data;
    Log_Settings logger_settings;
    logger_settings.log_file = "logs/client.log";
    string config_file_json{};
    string config_file_toml{};

    CLI::App app("client");

    app.add_option("-p, --port", server_data.port, "port to connect to", true);


    app.add_option("-s, --server-ip", server_data.ip, "server-ip to connect to", true);


    auto flag_l{app.add_flag("-l, --log-to-file"
                            , logger_settings.log_to_file
                            , "Set if the program should log to a file")};


    app.add_flag("-d, --log-level-debug"
                , logger_settings.log_level_debug
                , "Sets the loginglevel for log-to-file to debug (default=info)")
                ->needs(flag_l);


    app.add_option("--log-file"
                  , logger_settings.log_file
                  , "Define a file in that the logs are written"
                  , true)->needs(flag_l);


    auto option_j{app.add_option("-j, --config-file-json"
                                , config_file_json
                                , "Get the configuration of the program from a JSON file."
                                )->check(CLI::ExistingFile)};


    app.add_option("-t, --config-file-toml"
                  , config_file_toml
                  , "Get the configuration of the program from a TOML file."
                  )->excludes(option_j)
                   ->check(CLI::ExistingFile);


    CLI11_PARSE(app, argc, argv);

    logger_settings.config_logger();

    if (config_file_json != "") {
        optional<nlohmann::json> o_json{validate_json(config_file_json)};

        if (o_json.has_value()) {
            nlohmann::json json = o_json.value();
        } else {
            exit(1);
        }
    } else if (config_file_toml != "") {
        optional<toml::table> o_toml{validate_toml(config_file_toml)};

        if (o_toml.has_value()) {
            toml::table toml = o_toml.value();
        } else {
            exit(1);
        }
    }

    spdlog::info(fmt::format("Started Client"));

    cout << endl;

    cout << rang::fg::green
         << rang::style::bold  << "Started Client! Server ip: " 
         << rang::fg::yellow   << server_data.ip
         << rang::fg::green    << " port: "
         << rang::fg::yellow   << server_data.port 
         << endl
        ;

    logger_settings.print_logger_config();


    std::thread tr{Repl(true, server_data)};

    tr.join();

    google::protobuf::ShutdownProtobufLibrary();

    spdlog::info(fmt::format("Client stoped"));
}
