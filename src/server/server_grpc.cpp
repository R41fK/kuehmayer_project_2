#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <string>
#include <iostream>
#include <thread>


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

Status Shutdown_Implementation::shutdown(ServerContext* context, const ShutdownRequest* request, ShutdownReply* reply) {

    ShutdownType shutdown{};

    if (request->has_imediate()) {
        shutdown = ShutdownType::IMEDIATE;
    } else if (request->has_normal()) {
        shutdown = ShutdownType::NORMAL;
    } else {
        shutdown = ShutdownType::NONE;
    }

    // Do something

    if (shutdown == ShutdownType::IMEDIATE) {
        cout << "Imediate " << context << endl;
    } else {
        cout << "Normal " << context << endl;
    }

    reply->set_finished_shutdown(true);

    return Status::OK;    
}


void Shutdown_Implementation::operator()(){
    return;
}