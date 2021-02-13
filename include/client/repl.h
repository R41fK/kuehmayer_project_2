#pragma once

class Repl {
private:
    bool& running;

    // show the commands for the repl
    void show_help();

    // stop all threads
    void stop();

public:

    Repl(bool& running):
    running{running}
    {};

    // a method that starts the repl, should be started in its own thread
    void operator()();
};