#pragma once

#include <grpcpp/grpcpp.h>

#include "grpc_message.grpc.pb.h"
#include "grpc_message.pb.h"


class RPC_Client {
    std::unique_ptr<shutdown_message::Shutdown::Stub> stub_;

    public:
    RPC_Client(std::shared_ptr<grpc::Channel> channel) : stub_(shutdown_message::Shutdown::NewStub(channel)) {}

    // sends a shutdown signal to the server. Returns true if the server is shuting down 
    // and false if the server rejects the singal.
    bool send_shutdown();
};