#pragma once

#include <grpcpp/grpcpp.h>

#include "grpc_message.grpc.pb.h"
#include "grpc_message.pb.h"


class RPC_Client {
    std::unique_ptr<shutdown_message::Shutdown::Stub> stub_;

    public:
    RPC_Client(std::shared_ptr<grpc::Channel> channel) : stub_(shutdown_message::Shutdown::NewStub(channel)) {}

    bool send_shutdown();
};