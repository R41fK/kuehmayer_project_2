#pragma once

#include <grpcpp/grpcpp.h>
#include "grpc_message.pb.h"
#include "grpc_message.grpc.pb.h"

#include "config.h"

enum class ShutdownType {
    IMEDIATE,
    NORMAL,
    NONE
};

class Shutdown_Implementation final : public shutdown_message::Shutdown::Service {
    config::Server server_data;

    grpc::Status shutdown(grpc::ServerContext* context, const shutdown_message::ShutdownRequest* request, shutdown_message::ShutdownReply* reply) override;

    public:

    Shutdown_Implementation(config::Server server_data): server_data(server_data) {};

    void operator()();
};