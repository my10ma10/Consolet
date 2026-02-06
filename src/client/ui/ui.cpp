#include "ui.hpp"
#include "command.hpp"

// UI::UI(std::unique_ptr<CommandInterface>&& ci) 
//     : ci_(std::move(ci))
// {
//     initscr();
// }

// UI::~UI() {
//     endwin();
// }

// void UI::display() const
// {
//     // int row, col;
//     // getmaxyx(stdscr, row, col);
//     auto vec = CommandFactory::createForNumberedCI()
//     ci_->display();
// }


void disableEcho() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag &= ~ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void enableEcho() {
    struct termios term; 
    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag |= ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}