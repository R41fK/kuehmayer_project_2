#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

#include <iostream>
#include <rang.hpp>

using namespace std;

int main(int argc, char* argv[]) {

    cout << endl;

    cout << rang::fg::green
         << rang::style::bold  << "Started " 
         << rang::fg::yellow   << "Unit Tests " 
         << rang::fg::green    << "for the "
         << rang::fg::yellow   << "client "
         << rang::fg::green    << "and the " 
         << rang::fg::yellow   << "server"
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

    int res = context.run(); // run

    if(context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
        return res;          // propagate the result of the tests
    
    // your program - if the testing framework is integrated in your production code
    
    cout << endl;

    return res; // the result from doctest is propagated here as well
}