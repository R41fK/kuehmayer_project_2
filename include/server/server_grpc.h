#pragma once

#include <grpcpp/grpcpp.h>
#include <fmt/color.h>

#include "grpc_message.pb.h"
#include "grpc_message.grpc.pb.h"

#include "config.h"

class Shutdown_Implementation final : public shutdown_message::Shutdown::Service {
    config::Server server_data;

    bool allow_print{false};

    std::promise<void> exit_requested;

    grpc::Status shutdown(grpc::ServerContext* context, const shutdown_message::ShutdownRequest* request, shutdown_message::ShutdownReply* reply) override;

    void print(std::string, fmt::color);

    public:

    bool imediate{false};
    bool shutdown_now{false};

    Shutdown_Implementation(config::Server server_data, bool allow_print): 
            server_data(server_data), 
            allow_print(allow_print) 
            {};

    void operator()();
};