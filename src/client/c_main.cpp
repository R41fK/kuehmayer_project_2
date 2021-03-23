
#include <iostream>
#include <thread>
#include <optional>
#include <sys/wait.h>

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

string vaidate_ip_address_for_cli(const string& str) {
    Server server_data;
    server_data.ip = str;
    if (server_data.validate_ip_address()) {
        return "";
    } else {
        return "Invalid Ip-Address for the Server!";
    }
}


int main(int argc, char* argv[]) {

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Server server_data;
    Log_Settings logger_settings;
    logger_settings.log_file = "logs/client.log";
    string config_file_json{};
    string config_file_toml{};
    bool _start_server{};

    CLI::App app("client");

    app.add_option("-p, --port", server_data.port, "port to connect to", true);


    app.add_option("-s, --server-ip", server_data.ip, "server-ip to connect to", true)
                   ->check(vaidate_ip_address_for_cli);


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

    app.add_flag("--start-server"
                , _start_server
                , "Starts the server with the same options that the client has"
                );

    CLI11_PARSE(app, argc, argv);

    int pid{start_server(_start_server, server_data, logger_settings, config_file_json, config_file_toml)};
    
    if (pid == INT_MIN) {
        cerr << "Server Ip has to be an address to the local machine if you want to start the server at the same time!" << endl;
        exit(1);
    }

    if (config_file_json != "") {
        optional<nlohmann::json> o_json{validate_json(config_file_json)};

        if (o_json.has_value()) {
            if (!config_from_json(o_json.value(), ref(server_data), ref(logger_settings))) {
                exit(1);
            }
        } else {
            exit(1);
        }
    } else if (config_file_toml != "") {
        optional<toml::table> o_toml{validate_toml(config_file_toml)};

        if (o_toml.has_value()) {
            if (!config_from_toml(o_toml.value(), ref(server_data), ref(logger_settings))) {
                exit(1);
            }
        } else {
            exit(1);
        }
    }

    logger_settings.config_logger();

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

    if (pid != 0) {
        int status_a{};
        kill(pid, SIGTERM);

        waitpid(pid, &status_a, 0);
    }
}
