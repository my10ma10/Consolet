#include "server_db.hpp"

#include "message/message.cpp"
#include "user/user.cpp"
#include "chat/chat.cpp"

#include <optional>

#include "spdlog/spdlog.h"

bool ServerDB::save(User &user)
{
    bool res = execute(
        "INSERT INTO User (name, password) VALUES(?, ?)", 
        user.getName(), user.getPassword()
    );
    if (res) {
        user.setID(sqlite3_last_insert_rowid(db_));
    }

    return res;
}

bool ServerDB::save(User&& user) {
    bool res = execute(
        "INSERT INTO User (name, password) VALUES(?, ?)", 
        user.getName(), user.getPassword()
    );

    return res;
}

void ServerDB::addMemberToChat(ID_t userID, ID_t chatID) {
    auto user = findUser(userID);
    if (!user.has_value()) {
        spdlog::error("Error: user not found");
        execute(
            "INSERT INTO User (name, password) VALUES(?, ?)", 
            user->getName(), user->getPassword()
        );
    }

    execute(
        "INSERT INTO ChatMembers (chat_id, user_id) VALUES(?, ?)",
        chatID, userID
    );
}



std::optional<User> ServerDB::findUser(const std::string& name) {
    ID_t id;
    std::string password;
    
    executeWithCallback([&] (sqlite3_stmt* stmt) {
        id = sqlite3_column_int(stmt, 0);
        password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

        return true;
    }, 
    "SELECT id, password FROM User WHERE name = ?;", name);

    return std::make_optional<User>(name, password, id);    
}

std::optional<User> ServerDB::findUser(ID_t id) {
    std::string name;
    std::string password;

    executeWithCallback([&] (sqlite3_stmt* stmt) {
        name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

        return true;
    }, 
    "SELECT name, password FROM User WHERE id = ?", id);

    return std::make_optional<User>(name, password, id);
}

bool ServerDB::deleteUser(ID_t userID) {
    if (findUser(userID)) {
        bool res = execute(
            "DELETE FROM User WHERE id = ?",
            userID
        );
        return res;
    }
    return false;
}

bool ServerDB::save(Message& message) {
    if (!chatExistsInDB(message.getChatID())) {
        spdlog::error("Save message error: chat does not exists");
        return false;
    }

    bool res = execute(
        "INSERT INTO MessagesHistory (sender_id, chat_id, text, date_time) VALUES (?, ?, ?, ?)",
        message.getSenderID(), message.getChatID(), message.getText(), message.getTimeSinceEpoch()
    );

    if (res)
        message.setID(sqlite3_last_insert_rowid(db_));

    return res;
}

bool ServerDB::save(Message&& message) {
    return save(message);
}

std::optional<Message> ServerDB::findMessage(ID_t chatID, ID_t msgID) {
    std::string text;
    ID_t senderID = 0;
    int64_t timestamp = 0;

    bool exec_res = executeWithCallback([&] (sqlite3_stmt* stmt) -> bool {
        senderID = sqlite3_column_int64(stmt, 0);
        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        timestamp = sqlite3_column_int64(stmt, 2);
        return true;
    }, 
        "SELECT sender_id, text, date_time FROM MessagesHistory WHERE chat_id = ? AND id = ?",
        chatID, msgID
    );

    if (!exec_res || !senderID || text.empty() || timestamp == 0) {
        spdlog::critical("Message not found");
        return std::nullopt;
    }

    Message msg(chatID, senderID, text, timestamp);
    msg.setID(msgID);

    return std::make_optional<Message>(std::move(msg));
}

std::optional<Message> ServerDB::findMessage(ID_t chatID, const std::string& text) {
    ID_t senderID = 0;
    ID_t msgID = 0;
    int64_t timestamp = 0;

    bool exec_res = executeWithCallback([&] (sqlite3_stmt* stmt) -> bool {
        senderID = sqlite3_column_int64(stmt, 0);
        msgID = sqlite3_column_int64(stmt, 1);
        timestamp = sqlite3_column_int64(stmt, 2);
        return true;
    },
        "SELECT sender_id, id, date_time FROM MessagesHistory WHERE chat_id = ? AND text = ?", 
        chatID, text
    );

    if (!exec_res || !(senderID || msgID || timestamp == 0)) {
        spdlog::critical("Message not found");
        return std::nullopt;
    }

    Message msg(chatID, senderID, text, timestamp);
    msg.setID(msgID);

    return std::make_optional<Message>(msg);
}

bool ServerDB::deleteMessage(ID_t chatID, ID_t msgID) {
    if (findMessage(chatID, msgID)) {
        bool res = execute(
            "DELETE FROM MessagesHistory WHERE chat_id = ? AND id = ?",
            chatID, msgID
        );
        return res;
    }
    return false;
}


bool ServerDB::save(Chat& chat) {
    if (chat.getID() && chatExistsInDB(*chat.getID())) {
        spdlog::info("Chat exists in ServerDB - return without pulling");
        return true;
    }
    
    bool exec_res = execute(
        "INSERT INTO Chat (name, type) VALUES (?, ?)", 
        chat.getName(), chat.getStringType()
    );

    if (exec_res) chat.setID(sqlite3_last_insert_rowid(db_));

    for (const auto& userID : chat.getUsersIDs()) {
        addMemberToChat(userID, *chat.getID());
    }
    return exec_res;
}

bool ServerDB::save(Chat&& chat) {
    return save(chat); 
}

std::optional<Chat> ServerDB::findChat(ID_t chatID) {
    std::string chatType;
    std::optional<std::string> chatName;
    std::vector<ID_t> userIDs;

    bool exec_res = executeWithCallback([&] (sqlite3_stmt* stmt) {
        const unsigned char* type = sqlite3_column_text(stmt, 0);
        if (!type) return true;

        chatType = reinterpret_cast<const char*>(type);
                
        const unsigned char* nameText = sqlite3_column_text(stmt, 1);
        if (nameText) {
            chatName = std::string(reinterpret_cast<const char*>(nameText));
        } 
        else { 
            chatName = std::nullopt;
        }

        userIDs.emplace_back(sqlite3_column_int64(stmt, 2));
        return true;
    }, 
        R"(SELECT 
            c.type AS c_type,
            c.name AS c_name,
            u.id AS u_id
        FROM Chat c
        JOIN ChatMembers cm ON cm.chat_id = c.id
        JOIN User u ON u.id = cm.user_id
        WHERE c.id = ?
        ORDER BY u.id;)", chatID
    );

    if (!exec_res) return std::nullopt;

    return makePulledChat(userIDs, chatType, chatName, chatID);
}

std::optional<Chat> ServerDB::findChat(const std::string& chatName) {
    std::string chatType;
    ID_t chatID;
    std::vector<ID_t> userIDs;

    bool exec_res = executeWithCallback([&] (sqlite3_stmt* stmt) {
        const unsigned char* type = sqlite3_column_text(stmt, 0);
        if (!type) return true;

        chatType = reinterpret_cast<const char*>(type);
        chatID = sqlite3_column_int64(stmt, 1);
            
        userIDs.emplace_back(sqlite3_column_int64(stmt, 2));
        return true;
    }, 
        R"(SELECT 
            c.type AS c_type,
            c.id AS c_id,
            u.id AS u_id
        FROM Chat c
        JOIN ChatMembers cm ON cm.chat_id = c.id
        JOIN User u ON u.id = cm.user_id
        WHERE u.name = ?
        ORDER BY u.id;)", chatName
    );

    if (!exec_res) return std::nullopt;

    return makePulledChat(userIDs, chatType, chatName, chatID);
}


std::optional<Chat> ServerDB::findChatWith(const std::string& username) {
    std::string chatType;
    ID_t chatID;
    std::vector<ID_t> userIDs;

    bool exec_res = executeWithCallback([&] (sqlite3_stmt* stmt) {
        const unsigned char* type = sqlite3_column_text(stmt, 0);
        if (!type) return true;

        chatType = reinterpret_cast<const char*>(type);
        chatID = sqlite3_column_int64(stmt, 1);
        userIDs.emplace_back(sqlite3_column_int64(stmt, 2));
        return true;
    },
        R"(SELECT 
            c.id, 
            c.type, 
            c.name
        FROM Chat c
        JOIN ChatMembers cm ON c.id = cm.chat_id
        JOIN User u ON cm.user_id = u.id
        WHERE u.name = ? AND c.type = 'personal';)", username
    );

    if (!exec_res) return std::nullopt;

    return makePulledChat(userIDs, chatType, std::string{}, chatID);
}


bool ServerDB::deleteChat(ID_t chatID) {
    return execute("DELETE FROM Chat WHERE id = ?", chatID);
}
