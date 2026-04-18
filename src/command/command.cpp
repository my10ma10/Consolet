#include "command.hpp"
#include "client/client_session/client_session.hpp"
#include "clock.cpp"

#include <ncurses.h>

// std::vector<ICommand> InfoCommand::makeCommands() const {
//     std::vector<ICommand> res;
//     res.reserve(NUMBER_OF_COMMANDS);

//     res.emplace_back(InfoCommand{});
//     res.emplace_back(SendMsgCommand{});
//     res.emplace_back(PrintChatsListCommand{});
//     res.emplace_back(OpenChatCommand{});
//     res.emplace_back(ExitAccountCommand{});
//     res.emplace_back(QuitAppCommand{});

//     return res;
// }

// void InfoCommand::execute() {
//     std::vector<ICommand> commands = makeCommands();

//     std::cout << "List of commands:\n";
//     for (int i = 0; i < commands.size(); ++i) {
//         std::format("\t{}. {}\n", i, commands[i].getName());
//     }
// }

bool SendMsgCommand::sendMessage(const std::string& receiverName, const std::string& text) {
    printw("\nTrying to write msg to the db\n");

    auto chat = session_.getLocalDB()->findChat(receiverName);
    if (!chat.has_value()) {
        return false;
    }

    ID_t chat_id = chat->getID().value_or(0);


    Message message(
        chat_id,
        session_.getClientID().value_or(0), 
        text,
        cl::time_since_epoch()
    );


    if (message.getText().empty()) {
        return false;
    }

    session_.sendToServer(std::move(message));

    session_.getLocalDB()->save(message);
    
    return true;
}

// void SendMsgCommand::sendToPersonalChat(
//     std::shared_ptr<DB> db, 
//     const std::string& receiverName, 
//     const std::string& text
// ) 
// {

//     Message message(
//         session_.getLocalDB()->findChat(receiverName)->getID().value_or(0), 
//         session_.getClientID().value_or(0), 
//         text
//     );

// }

// void SendMsgCommand::sendToGroupChat(
//     std::shared_ptr<DB> db, 
//     const std::string& receiverName, 
//     const std::string& text
// ) 
// {
//     // sendMessage(Message{ , caller_id, });
// }
