#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

#include "json.hpp"
using namespace nlohmann;

#include <iostream>
#include <thread>
#include <sys/wait.h>
#include <rang.hpp>
#include <asio.hpp>

#include "client/repl.h"
#include "client/client_grpc.h"

using namespace std;

int main(int argc, char* argv[]) {

    cout << endl;

    cout << rang::fg::green
         << rang::style::bold  << "Started " 
         << rang::fg::yellow   << " Integration Tests " 
         << rang::fg::green    << "for the "
         << rang::fg::yellow   << "client "
         << rang::fg::green    << "and the " 
         << rang::fg::yellow   << "server."
         << endl 
         << rang::fg::green    << "The server is started "
         << rang::fg::yellow   << "automatically." 
         << endl
        ;

    for (int i=0; i < 79; i++) {
        cout << "-";
    }

    cout << rang::style::reset 
         << rang::fg::reset 
         << endl;

    doctest::Context context;

    context.setOption("order-by", "name");            // sort the test cases by their name

    context.applyCommandLine(argc, argv);

    // overrides
    context.setOption("no-breaks", true);             // don't break in the debugger when assertions fail

    auto pid{fork()};
    config::Server server_data{};

    if (pid == 0) {
        execl("./server", "server", "-e" , nullptr);
    } else {
        this_thread::sleep_for(chrono::milliseconds(500));
        Repl repl{false};
        
        repl.strm = new asio::ip::tcp::iostream{server_data.ip, server_data.get_port_as_string()};
        if (!*repl.strm) {
            cerr << "No Connection to the Server possible!" << endl;
            repl.strm->close();
            exit(1);
        }
        repl.strm->close();
    }

    int res = context.run(); // run

    if (pid != 0) {
        int status_a{};
        RPC_Client client{grpc::CreateChannel(server_data.ip + ":" + server_data.get_grpc_port(), grpc::InsecureChannelCredentials())};
        client.send_shutdown();

        waitpid(pid, &status_a, 0);
    }

    if(context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
        return res;          // propagate the result of the tests
    
    // your program - if the testing framework is integrated in your production code
    
    cout << endl;

    return res; // the result from doctest is propagated here as well
}