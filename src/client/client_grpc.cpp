#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "grpc_message.grpc.pb.h"
#include "grpc_message.pb.h"

#include "client/client_grpc.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;


using shutdown_message::Shutdown;
using shutdown_message::ShutdownRequest;
using shutdown_message::ShutdownReply;

using namespace std;

bool RPC_Client::send_shutdown() {
    ClientContext context;
    ShutdownReply reply;
    ShutdownRequest request;

    request.set_imediate(true);

    Status status = stub_->shutdown(&context, request, &reply);

    if (!status.ok()) {
        return false;
    }

    return reply.allowed_to_shutdown();
}