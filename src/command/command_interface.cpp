#include "command_interface.hpp"

#include "command.hpp"

#include <sstream>
#include <ncurses.h>

CommandInterface::CommandInterface() { 
    initscr();
    raw();
}

CommandInterface::~CommandInterface() {
    endWindow();
}

void CommandInterface::endWindow() {
    endwin();
}



// === Numbered CI ===

void NumberedCI::display(const std::vector<std::unique_ptr<ICommand>>& commands) const {
    printw("=== Commands ===\n");
    for (size_t i = 0; i < commands.size(); ++i) {
        printw("%ld. %s\n", i+1, commands[i]->getDescription().c_str());
    }
    printw("0. Exit\n");
    printw("\nEnter command number: ");
    refresh();
}

void NumberedCI::processChoice(
    const std::vector<std::unique_ptr<ICommand>>& commands, 
    std::variant<std::size_t, std::string> ch
) {
    auto choice = std::get<std::size_t>(ch);
    auto index = choice - 1;

    if (choice == 0) {
        endWindow();
    }
    else if (choice > 0 && choice <= commands.size()) {
        commands[index]->execute();
    }
    else {
        throw std::invalid_argument(
            std::string("NumberedCI::processChoice: Unexpected choice: ") + std::to_string(choice)
        );
    }
}

std::string NumberedCI::help() {
    return "This is the numbered CI";
}

ICommand* NumberedCI::selectCommand(const std::vector<std::unique_ptr<ICommand>>& commands) const {
    std::size_t choice = getch() - '0';

    if (choice <= 0 || choice >= commands.size()) {
        printw("There is no command #%ld", choice);
        return nullptr;
    }

    return commands[choice - 1].get();
}



// === Slashed CI ===

void SlashedCI::display(const std::vector<std::unique_ptr<ICommand>>& commands) const {
    printw("=== Commands ===\n");
    
    for (const auto& cmd : commands) {
        printw("/%s ", cmd->getName().c_str());
        
        attron(A_ITALIC);
        for (const auto& arg : cmd->getArgs()) {
            printw( "%s ", arg.c_str());
        }
        attroff(A_ITALIC);

        printw("- %s\n", cmd->getDescription().c_str());
    }
    
    printw("\nEnter command:\n\t");
    refresh();
}

void SlashedCI::processChoice(
    const std::vector<std::unique_ptr<ICommand>>& commands, 
    std::variant<std::size_t, std::string> str
) {
    auto command_str = std::get<std::string>(str);

    auto [command_name, vec_args] = splitCmdString(command_str);

    for (const auto& cmd : commands) {
        if (cmd->getName() == command_name) {
            cmd->setArgs(vec_args);
            cmd->execute();
            break;
        }
    }

}

std::string SlashedCI::help() {
    return "This is the slashed CI";
}

std::pair<std::string, std::vector<std::string>> 
    CommandInterface::splitCmdString(const std::string& command_str)
{
    if (!command_str.starts_with('/')) {
        throw std::invalid_argument(std::string("Expected '/', received ") + command_str.front());
    }

    std::istringstream iss(command_str);

    std::string arg, name;
    std::vector<std::string> args;

    iss >> name;
    name.erase(name.begin());
    while (iss >> arg) {
        args.emplace_back(arg);
    }

    return {name, args};
}
