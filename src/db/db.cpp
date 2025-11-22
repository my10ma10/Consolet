#include "db.hpp"
#include "user.hpp"
#include "chat.hpp"
#include "message.hpp"

#include <array>
#include <iterator>
#include <functional>
#include <fstream>
#include <sstream>

DB::~DB() {
    int res = sqlite3_close(db_);
    if (res != SQLITE_OK) {
        std::cerr << "SQLite3 close error" << std::endl;
    }
    db_ = nullptr;
}

DB::DB(DB&& other) noexcept : db_(other.db_) {
    other.db_ = nullptr;
}

DB& DB::operator=(DB&& other) noexcept {
    if (this != &other) {
        if (db_) {
            int res = sqlite3_close(db_);
            if (res != SQLITE_OK) {
                std::cerr << "SQLite3 close error" << std::endl;
            }
        }
        
        db_ = other.db_;
        other.db_ = nullptr;
    }
    return *this;
}

void DB::init(const std::string& db_name, const std::string& sqlFile) {
    std::vector<std::string> sql = readSqlQuery(sqlFile);

    createDB(db_name, sql);
}

void DB::createDB(const std::string& db_name, const std::vector<std::string>& sql) {
    if (sqlite3_open(db_name.c_str(), &db_) != SQLITE_OK) {
        std::cerr << "Error: cannot open db: " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        db_ = nullptr;
        throw std::logic_error("Failed to open database");
    }
    
    execute("PRAGMA foreign_keys = ON;");

    if (!sql.empty()) {
        for (const auto& query : sql) {
            execute(query);
        }
    }
    else {
        db_ = nullptr;
        throw std::logic_error("Create query was not executed");
    }
}

std::vector<std::string> DB::readSqlQuery(const std::string& filename) {
    std::vector<std::string> res;
    res.reserve(4);
    
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: can not open query SQL file " << filename << std::endl;
        return {};
    }

    std::string line;
    while (std::getline(file, line, ';')) {
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
    
        if (!line.empty()) {
            res.emplace_back(line);
        }
    }

    return res;
}

ssize_t DB::getTableSize(const std::string& tableName) {
    std::optional<ssize_t> res;
    executeWithCallback(
        [this, &res] (sqlite3_stmt* stmt) {
            res = sqlite3_column_int64(stmt, 0);
            return false; // exit from execute immidiately
        },
        std::string("SELECT COUNT(*) FROM ") + tableName
    );

    if (!res.has_value()) {
        throw std::invalid_argument("Empty res");
    }
    return res.value();
}

bool DB::save(User& user) {
    bool res = execute(
        "INSERT INTO User (name, password) VALUES(?, ?)", 
        user.getName(), user.getPassword()
    );
    if (res) {
        user.setID(sqlite3_last_insert_rowid(db_));
    }

    return res;
}

bool DB::save(User&& user) {
    bool res = execute(
        "INSERT INTO User (name, password) VALUES(?, ?)", 
        user.getName(), user.getPassword()
    );

    return res;
}

void DB::addMemberToChat(ID_t userID, ID_t chatID) {
    auto user = findUser(userID);
    if (!user.has_value()) {
        std::cerr << "Error: user not found" << std::endl;
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

std::optional<Chat> DB::makePulledChat(
    std::vector<ID_t>& userIDs, const std::string& chatType, 
    const std::optional<std::string>& chatName, ID_t chatID
) {
    if (chatType.empty()) return std::nullopt;

    if (userIDs.empty()) {  
        throw std::invalid_argument("Can not find chat members - usersIDs is empty");
    }

    auto pulled_chat = std::make_optional<Chat>(
        shared_from_this(),
        userIDs,
        ChatType::fromString(chatType),
        chatName
    );
    pulled_chat->setID(chatID);
    
    return pulled_chat;
}

std::optional<User> DB::findUser(const std::string& name) {
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

std::optional<User> DB::findUser(ID_t id) {
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

bool DB::save(Message& message) {
    if (!chatExistsInDB(message.getChatID())) {
        std::cerr << "Save message error: chat does not exists\n";
        return false;
    }

    bool res = execute(
        "INSERT INTO MessagesHistory (sender_id, chat_id, text) VALUES (?, ?, ?)",
        message.getSenderID(), message.getChatID(), message.getText() 
    );

    if (res)
        message.setID(sqlite3_last_insert_rowid(db_));

    return res;
}

bool DB::save(Message&& message) {
    return save(message);
}

std::optional<Message> DB::findMessage(ID_t chatID, ID_t msgID) {
    std::string text;
    ID_t senderID = 0;

    bool exec_res = executeWithCallback([&] (sqlite3_stmt* stmt) -> bool {
        senderID = sqlite3_column_int64(stmt, 0);
        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        return true;
    }, 
        "SELECT sender_id, text FROM MessagesHistory WHERE chat_id = ? AND id = ?",
        chatID, msgID
    );

    if (!exec_res || !senderID || text.empty()) {
        std::cerr << "Message not found\n";
        return std::nullopt;
    }

    Message msg(chatID, senderID, text);
    msg.setID(msgID);

    return std::make_optional<Message>(std::move(msg));
}

std::optional<Message> DB::findMessage(ID_t chatID, const std::string& text) {
    ID_t senderID = 0;
    ID_t msgID = 0;

    bool exec_res = executeWithCallback([&] (sqlite3_stmt* stmt) -> bool {
        senderID = sqlite3_column_int64(stmt, 0);
        msgID = sqlite3_column_int64(stmt, 1);
        return true;
    },
        "SELECT sender_id, id FROM MessagesHistory WHERE chat_id = ? AND text = ?", 
        chatID, text
    );

    if (!exec_res || !(senderID || msgID)) {
        std::cerr << "Message not found\n";
        return std::nullopt;
    }

    Message msg(chatID, senderID, text);
    msg.setID(msgID);

    return std::make_optional<Message>(msg);
}

bool DB::deleteMessage(ID_t chatID, ID_t msgID) {
    if (findMessage(chatID, msgID)) {
        bool res = execute(
            "DELETE FROM MessagesHistory WHERE chat_id = ? AND id = ?",
            chatID, msgID
        );
        return res;
    }
    return false;
}

bool DB::chatExistsInDB(ID_t chatID) {
    bool exists = false;

    executeWithCallback([&exists] (sqlite3_stmt* stmt) {
            exists = sqlite3_column_int(stmt, 0);
            return true;
        },
        "SELECT 1 FROM Chat WHERE id = ?", chatID
    );
        
    return exists;
}

bool DB::save(Chat& chat) {
    if (chat.getID() && chatExistsInDB(*chat.getID())) {
        std::cerr << "Chat exists in DB - return without pulling\n";
        return true;
    }
    
    bool exec_res = execute(
        "INSERT INTO Chat (name, type) VALUES (?, ?)", 
        chat.getName(), chat.getStringType()
    );

    if (exec_res) chat.setID(sqlite3_last_insert_rowid(db_));

    for (const auto& userID : chat.userIDs_) {
        addMemberToChat(userID, *chat.getID());
    }
    return exec_res;
}

bool DB::save(Chat&& chat) {
    return save(chat); 
}

std::optional<Chat> DB::findChat(ID_t chatID) {
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

std::optional<Chat> DB::findChat(const std::string& chatName) {
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
        WHERE c.name = ?
        ORDER BY u.id;)", chatName
    );

    if (!exec_res || chatType.empty()) return std::nullopt;

    if (userIDs.empty()) {  
        throw std::invalid_argument("Can not find chat members - usersIDs is empty");
    }

    auto pulled_chat = std::make_optional<Chat>(
        shared_from_this(),
        userIDs,
        ChatType::fromString(chatType),
        chatName
    );
    pulled_chat->setID(chatID);
    
    return pulled_chat;
}

bool DB::deleteChat(ID_t chatID) {
    return execute("DELETE FROM Chat WHERE id = ?", chatID);
}

bool DB::prepareExecution(const std::string& query, sqlite3_stmt** stmt) {
    if (sqlite3_prepare_v2(db_, query.c_str(), -1, stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Preparing statement error: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    return true;
}
