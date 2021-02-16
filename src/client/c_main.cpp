
#include <iostream>
#include <thread>
#include <optional>

#include <asio.hpp>
#include <CLI11.hpp>
#include <fmt/core.h>
#include <json.hpp>
#include "pystring.h"
#include <rang.hpp>
#include <spdlog/spdlog.h>
#include <toml.hpp>

#include "config.h"
#include "car_calculator.h"
#include "car.h"
#include "car_builder.h"
#include "client/repl.h"

using namespace asio;
using namespace std;

int main(int argc, char* argv[]) {

    Server server_data;
    Log_Settings logger_settings;
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


    optional<Car> o_car{Car_Builder().ps(80)->purchase_value(16000)->build()};

    if (o_car.has_value()) {

        Car_Calculator l;

        l.set_car(o_car.value());
        l.set_insurance_class(0);
        l.set_leasing_duration(5);
        l.set_rest_value(0);
        l.set_deposit(7000);
        l.is_over_24();

        cout << "Leasingrate: " << l.calculate_leasing_rate().value_or(0) << endl;
        cout << "Insurcane: " << l.calculate_insurance_rate().value_or(0) << endl;
    } else {
        cerr << "Car Builder failed" << endl;
    }

    bool running = true;
    std::thread tr{Repl(ref(running))};

    tr.join();
}
