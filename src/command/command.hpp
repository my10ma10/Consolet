#pragma once
#include <memory>

#include "../message/message.hpp"

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute() = 0;

};

class CommandParser {
    std::unique_ptr<ICommand> command;
public:
    CommandParser(std::unique_ptr<ICommand> cmd);

    template <typename... Args>
    void parse(Args... args) const;
};


class MsgCommand : public ICommand {
    Message _msg;
public:
    MsgCommand(const std::string& name, const std::string& text);


    void execute() override; // send message to reciever
};

class ListCommand : public ICommand {
public:

    void execute() override;
};

class ListCommand : public ICommand {
public:

    void execute() override;
};


template <typename... Args>
inline void CommandParser::parse(Args... args) const {
    command->execute(args...);
}
