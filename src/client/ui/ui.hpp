#pragma once
#include <iostream>
#include <memory>

// #include <ncurses.h>
// #include <termios.h>

// #include "db.hpp"
// #include "command_interface.hpp"
// #include "command_factory.hpp"

// class UI final {
//     std::unique_ptr<CommandInterface> ci_;

//     std::shared_ptr<DB> db_;
    

// public:
//     UI(std::unique_ptr<CommandInterface>&& ci = nullptr);

//     ~UI();

//     void display() const;

//     void setDB(std::shared_ptr<DB> db) {db_ = db; }


// private:
// };

void disableEcho();
void enableEcho();
