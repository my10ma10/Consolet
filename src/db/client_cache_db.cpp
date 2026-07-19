#include "client_cache_db.hpp"

#include "chat/chat.hpp"
#include "user/user.hpp"
#include "message/message.hpp"

#include "spdlog/spdlog.h"

void ClientCacheDB::syncChat(ID_t chatID, std::shared_ptr<ServerDB> server_db) {
    // auto pulled_chat = findChat(chatID);
    
    // auto messages = getMessagesSince(*pulled_chat->getID(), CachedMsgsNumber);

    auto messages = getMessagesSince(chatID, CachedMsgsNumber);

}

void ClientCacheDB::clearCachedMessages(time_t days) {
    execute(
        "DELETE FROM MessagesHistory WHERE date_time < ?",
        std::time(nullptr) - days * 24 * 60 * 60
    );
    
}
/**
 * @note depends on sql script
 */
bool ClientCacheDB::save(User& user) {
    bool status = execute(
        "INSERT INTO User (id, name, password) VALUES (?, ?, ?)", 
        user.getID().value(), user.getName(), user.getPassword()
    );
    return status;
}

std::optional<User> ClientCacheDB::findUser(const std::string& name) {
    ID_t id;
    std::string password;
    executeWithCallback([&](sqlite3_stmt* stmt) {
        id = sqlite3_column_int64(stmt, 0);
        password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        return true;
    }, 
    "SELECT id, password FROM User WHERE name = ?", name);

    return std::make_optional<User>(name, password, id);
}

std::optional<User> ClientCacheDB::findUser(ID_t id) {
    std::string name, password;
    executeWithCallback([&](sqlite3_stmt* stmt) {
        name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

        return true;
    }, 
    "SELECT name, password FROM User WHERE id = ?", id);

    return std::make_optional<User>(name, password, id);
}

bool ClientCacheDB::save(Message& message) {
    bool status = false;
    if (message.getID().has_value()) {
        status = execute(
            "INSERT INTO MessagesHistory (id, sender_id, chat_id, date_time, text) "
            "VALUES (?, ?, ?, ?, ?)",
            message.getID().value(), message.getSenderID(), message.getChatID(), 
            std::time(nullptr), message.getText()
        );
    }
    else {
        status = execute(
            "INSERT INTO MessagesHistory (sender_id, chat_id, date_time, text) "
            "VALUES (?, ?, ?, ?)",
            message.getSenderID(), message.getChatID(), 
            std::time(nullptr), message.getText()
        );
    }
    return status;
}

std::optional<Message> ClientCacheDB::findMessage(ID_t chatID, ID_t msgID) {
    ID_t senderID;
    std::string text;
    executeWithCallback([&](sqlite3_stmt* stmt) {
        senderID = sqlite3_column_int64(stmt, 0);
        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

        return true;
    }, 
    "SELECT sender_id, text FROM MessagesHistory WHERE id = ? chat_id = ?", msgID, chatID);

    return std::make_optional<Message>(chatID, senderID, text, msgID);
}

std::optional<Message> ClientCacheDB::findMessage(ID_t chatID, const std::string& text) {
    ID_t msgID, senderID;
    executeWithCallback([&](sqlite3_stmt* stmt) {
        msgID = sqlite3_column_int64(stmt, 0);
        senderID = sqlite3_column_int64(stmt, 1);

        return true;
    }, 
    "SELECT id, sender_id FROM MessagesHistory WHERE text = ? chat_id = ?", text, chatID);

    return std::make_optional<Message>(chatID, senderID, text, msgID);
}

bool ClientCacheDB::save(Chat& chat) {
    bool status = execute("INSERT INTO Chat (id, name, type) VALUES (?, ?, ?)",
        chat.getID(), chat.getName(), chat.getStringType()
    );
    return status;
}

std::optional<Chat> ClientCacheDB::findChat(ID_t chatID) {
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

std::optional<Chat> ClientCacheDB::findChat(const std::string& chatName) {
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

std::optional<Chat> ClientCacheDB::findChatWith(const std::string& username) {
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
        R"(
        SELECT
            c.type,
            c.id,
            cm2.user_id
        FROM Chat c
        JOIN ChatMembers cm1 ON cm1.chat_id = c.id
        JOIN User u ON u.id = cm1.user_id
        JOIN ChatMembers cm2 ON cm2.chat_id = c.id
        WHERE u.name = ?
        AND c.type = 'personal'
        ORDER BY cm2.user_id;
        )", username
    );

    if (!exec_res) return std::nullopt;

    // auto senderId = findUser(username)->getID();
    // if (!senderId) {
    //     spdlog::warn("ClientCacheDB::findChatWith find sender id error");
    // }
    // userIDs.emplace_back(senderId.value());

    return makePulledChat(userIDs, chatType, std::string{}, chatID);
}
