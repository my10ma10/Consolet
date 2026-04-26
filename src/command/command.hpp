#pragma once

#include <memory>
#include <vector>
#include <string>

#include "message/message.hpp"
#include "command_interface.hpp"


class Client;

class ICommand {
protected:
    std::vector<std::string> args_;
public:
    virtual ~ICommand() = default;

    virtual void execute() = 0;
    virtual std::string getName() const = 0;
    virtual std::vector<std::string> getArgs() const = 0;
    virtual std::string getDescription() const = 0;

    void setArgs(const std::vector<std::string>& args) {args_ = args;}
};


// class InfoCommand : public ICommand {
// public:
//     void execute() override;
//     std::string getName() const override { return "info"; }
//     std::string getDescription() const override { return "Print available commands"; }
    
// private:
//     std::vector<ICommand> makeCommands() const;
// };

class SendMsgCommand : public ICommand {
    Client& client_;
public:
    SendMsgCommand(Client& s) : client_(s) {}

    void execute() override = 0; 

    std::string getName() const override { return "msg"; }
    std::vector<std::string> getArgs() const override { return {"receiver_name", "message"}; }
    std::string getDescription() const override { return "Send a message to chat or user"; }
   
protected: 
    bool sendMessage(const std::string& receiverName, const std::string& text);

private:
    // void sendToPersonalChat(std::shared_ptr<DB> db, const std::string& receiverName, const std::string& text);
    // void sendToGroupChat(std::shared_ptr<DB> db, const std::string& receiverName, const std::string& text);
};

// class PrintChatsListCommand : public ICommand {
//     Client& client_;
// public:
//     PrintChatsListCommand(Client& client) : client_(client) {}

//     void execute() override;
//     std::string getName() const override { return "chats"; }
//     std::string getDescription() const override { return "Print user's active chats"; }
// };

// class OpenChatCommand : public ICommand {
//     Client& client_;
// public:
//     OpenChatCommand(Client& client) : client_(client) {}

//     void execute() override;
//     std::string getName() const override { return "open"; }
//     std::string getDescription() const override { return "Open chat"; }
// };

// class ExitAccountCommand : public ICommand {
//     Client& client_;
// public:
//     ExitAccountCommand(Client& client) : client_(client) {}

//     void execute() override;
//     std::string getName() const override { return "exit"; }
//     std::string getDescription() const override { return "Exit account"; }
// };

// class QuitAppCommand : public ICommand {
//     Client& client_;
// public:
//     QuitAppCommand(Client& client) : client_(client) {}

//     void execute() override;
//     std::string getName() const override { return "quit"; }
//     std::string getDescription() const override { return "Quit application"; }
// };



class CommandParser {
    std::unique_ptr<ICommand> command;
public:
    CommandParser() = default;

    template <typename... Args>
    void parse(Args... args) const;
};

template <typename... Args>
inline void CommandParser::parse(Args... args) const {
    command->execute(args...);
}
