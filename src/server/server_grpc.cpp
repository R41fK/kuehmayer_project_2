#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <string>
#include <iostream>
#include <thread>
#include <future>


#include <fmt/core.h>
#include <fmt/color.h>
#include <rang.hpp>
#include <spdlog/spdlog.h>

#include "grpc_message.pb.h"
#include "grpc_message.grpc.pb.h"

#include "server/server_grpc.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;


using shutdown_message::Shutdown;
using shutdown_message::ShutdownRequest;
using shutdown_message::ShutdownReply;

using namespace std;

void Shutdown_Implementation::print(string output, fmt::color color) {
    if (this->allow_print) {
        fmt::print(fg(color), output + "\n");
    }
}


Status Shutdown_Implementation::shutdown(ServerContext*, const ShutdownRequest*, ShutdownReply* reply) {

    if (!this->server_data.allow_shutdown) {
        print("Client attempted to shutdown the server, he is not allowed", fmt::color::orange);
        reply->set_allowed_to_shutdown(false);
        return Status::OK;
    }

    this->imediate = true;
    reply->set_allowed_to_shutdown(true);
    print("Client shutdown the server", fmt::color::orange);
    exit_requested.set_value();

    return Status::OK;    
}


void Shutdown_Implementation::operator()(){
    //grpc::EnableDefaultHealthCheckService(true);
    
    string server_address{"[::]:" + this->server_data.get_grpc_port()};

    grpc::ServerBuilder builder;
    // Listen on the given address without any authentication mechanism
    
    builder.AddListeningPort(ref(server_address), grpc::InsecureServerCredentials());
    
    // Register "service" as the instance through which
    // communication with client takes place
    builder.RegisterService(this);
    // Assembling the server
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    if (server != nullptr) {
    
        auto wait = async(launch::async, [&](){
            server->Wait();
            return;
        });

        exit_requested.get_future().wait();
        
        server->Shutdown();

        wait.wait();
    } else {
        fmt::print("Exception: Build of grpc Server failed!\n");
        spdlog::info("Exception: Build of grpc Server failed!");
    }
    // } else {
    //     fmt::print("Exception: Ports are allready in Use!\n");
    //     spdlog::info("Exception: Ports are allready in Use!");
    // }

    this->shutdown_now = true;
}