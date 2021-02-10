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

#include "config.h"

using namespace asio;
using namespace std;

int main(int argc, char* argv[]) {

    Server server_data;
    Log_Settings logger_settings;

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

    CLI11_PARSE(app, argc, argv);

    logger_settings.config_logger();
}