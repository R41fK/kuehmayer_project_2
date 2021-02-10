
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

    unsigned int port{};
    string ip{"127.0.0.1"};
    string log_file{"logs/control.log"};
    bool log_to_file{false};
    bool log_level_debug{false};

    CLI::App app("client");

    app.add_option("-p, --port", port, "port to connect to", true);

    app.add_option("-s, --server-ip", ip, "server-ip to connect to", true);

    auto flag_l{app.add_flag("-l, --log-to-file"
                            , log_to_file
                            , "Set if the program should log to a file")};

    app.add_flag("-d, --log-level-debug"
                , log_level_debug
                , "Sets the loginglevel for log-to-file to debug (default=info)")
                ->needs(flag_l);

    app.add_option("--log-file"
                  , log_file
                  , "Define a file in that the logs are written"
                  , true)->needs(flag_l);

    CLI11_PARSE(app, argc, argv);

    config_logger(log_file, log_to_file, log_level_debug);

    ip::tcp::iostream strm{"localhost", to_string(port)};

    strm.expires_after(10s);

    try {
        if (strm) { 
            string data;
            getline(strm, data);
            
            if (strm) {
                cout << data << endl;
            } else {
            spdlog::error(strm.error().message());
            }

            strm.close();
        } else { 
            spdlog::error(strm.error().message());
        } 
    } catch (asio::system_error& e) {
        spdlog::error(e.what()); 
    }


    for (int i=0; i < 1000000; i++) {
        spdlog::info(to_string(i));
    }
}
