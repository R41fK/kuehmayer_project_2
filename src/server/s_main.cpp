#include <iostream>
#include <thread>

#include <asio.hpp>
#include <CLI11.hpp>
#include <fmt/core.h>
#include <json.hpp>
#include "pystring.h"
#include <rang.hpp>
#include <spdlog/spdlog.h>
#include <toml.hpp>

#include "message_utility.h"
#include "config.h"
#include "server/object_storage.h"

#include "message.pb.h"

using namespace asio;
using namespace std;


int main(int argc, char* argv[]) {

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Server server_data;
    Log_Settings logger_settings;
    logger_settings.log_file = "logs/server.log";
    string config_file_json{};
    string config_file_toml{};

    CLI::App app("client");

    app.add_option("-p, --port", server_data.port, "port to connect to", true);

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

    cout << "Started" << endl;

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

    asio::io_context ctx;
    ip::tcp::endpoint ep{ip::tcp::v4(), server_data.port};
    ip::tcp::acceptor acceptor{ctx, ep};

    

    spdlog::info("Started Server!");

    try {
        while (1) {
            acceptor.listen();
                
            ip::tcp::iostream strm{acceptor.accept()};

            spdlog::info("Client connected to Server");

            Object_Storage obst{};

            while (strm) {           

                string data{};

                getline(strm, data);

                if (strm) {
                    spdlog::debug(fmt::format("server got message {}", data));

                    strm << message_utility::to_ascii(obst.new_action(data));
                }
            }

            strm.close();

            spdlog::info("Client disconnected");
        }
    } catch (asio::system_error& e) {
        spdlog::error(e.what()); 
    }

    google::protobuf::ShutdownProtobufLibrary();
}