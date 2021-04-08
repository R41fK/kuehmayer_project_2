#include <iostream>
#include <thread>
#include <asio.hpp>
#include <unistd.h>

#include <CLI11.hpp>
#include <fmt/core.h>
#include <json.hpp>
#include <rang.hpp>
#include <spdlog/spdlog.h>
#include <toml.hpp>

#include "message_utility.h"
#include "config.h"
#include "server/object_storage.h"
#include "server/server_grpc.h"

#include "message.pb.h"

using namespace asio;
using namespace std;


int main(int argc, char* argv[]) {

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    bool allow_print{false};
    config::Server server_data;
    config::Log_Settings logger_settings;
    logger_settings.log_file = "logs/server.log";
    string config_file_json{};
    string config_file_toml{};

    CLI::App app("client");

    app.add_option("-p, --port", server_data.port, "port to connect to for asio. Grpc will connect one port above", true);

    app.add_flag("-e, --enable-shutdown", server_data.allow_shutdown, "allows a client to shutdown the server with a request");

    app.add_flag("-a, --allow-print", allow_print, "allows the server to print out his actions");

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


    if (config_file_json != "") {
        optional<nlohmann::json> o_json{config::validate_json(config_file_json)};

        if (o_json.has_value()) {
            if (!config_from_json(o_json.value(), ref(server_data), ref(logger_settings))) {
                exit(1);
            }
        } else {
            exit(1);
        }
    } else if (config_file_toml != "") {
        optional<toml::table> o_toml{config::validate_toml(config_file_toml)};

        if (o_toml.has_value()) {
            if (!config_from_toml(o_toml.value(), ref(server_data), ref(logger_settings))) {
                exit(1);
            }
        } else {
            exit(1);
        }
    }

    logger_settings.config_logger();

    Shutdown_Implementation shutdown{server_data, allow_print};

    try {

        asio::io_context ctx;
        ip::tcp::endpoint ep{ip::tcp::v4(), server_data.port};
        ip::tcp::acceptor acceptor{ctx, ep};

        std::thread grpc{ref(shutdown)};

        this_thread::sleep_for(std::chrono::milliseconds(50));

        if (!shutdown.shutdown_now) {

            cout << endl;

            cout << rang::fg::green
                << rang::style::bold  << "Started Server on port: "
                << rang::fg::yellow   << server_data.port 
                << rang::fg::green    << " and grpc on port: "
                << rang::fg::yellow   << server_data.port + 1
                << endl
            ;


            logger_settings.print_logger_config();

            spdlog::info("Started Server!");

            acceptor.listen();
            std::thread t{[&]() {

                while (!shutdown.shutdown_now) {
                    
                    ip::tcp::iostream* strm = new ip::tcp::iostream(acceptor.accept());

                    std::thread t{[&]() {
                        
                        spdlog::info("Client connected to Server");

                        Object_Storage obst{allow_print};

                        while (*strm) {

                            string data{};

                            getline(*strm, data);

                            if (*strm) {
                                spdlog::debug(fmt::format("server got message {}", data));

                                *strm << message_utility::to_ascii(obst.new_action(data));
                            }
                        }

                        strm->close();

                        delete strm;

                        spdlog::info("Client disconnected");
                    }};

                    t.detach();
                }
            }};
            
            t.detach();

        }

        grpc.join();
        
        acceptor.close();

    } catch (asio::system_error& e) {
        spdlog::error(e.what());
        fmt::print("Exception: {} occured!\n", e.what()); 
    }

    google::protobuf::ShutdownProtobufLibrary();
    fmt::print("Server closed\n");
    spdlog::info("Server closed");
}