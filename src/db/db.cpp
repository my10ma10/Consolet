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
    sqlite3_close(db_);
}

DB::DB(DB&& other) {
    db_ = other.db_;
    other.db_ = nullptr;
}

DB& DB::operator=(DB&& other) {
    if (this != &other) {
        if (db_) {
            sqlite3_close(db_);
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
    sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
    
    if (sqlite3_open(db_name.c_str(), &db_) != SQLITE_OK) {
        std::cerr << "Error: cannot open db: " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        throw std::runtime_error("Failed to open database");
    }
    
    execute("PRAGMA foreign_keys = ON;");

    if (!sql.empty()) {
        for (const auto& query : sql) {
            execute(query);
        }
    }
    else {
        std::cerr << "Error: sql query was not executed" << std::endl;
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

void DB::addMemberToChat(ID_t chatId, ID_t userID) {
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
        chatId, userID
    );
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
    if (!chatExistsInDB(message.getChatID())) {
        std::cerr << "Save message error: chat does not exists\n";
        return false;
    }

    bool res = execute(
        "INSERT INTO MessagesHistory (sender_id, chat_id, text) VALUES (?, ?, ?)",
        message.getSenderID(), message.getChatID(), message.getText() 
    );
    return res;
}

std::optional<Message> DB::findMessage(ID_t chatID, ID_t msgID) {
    std::string text;
    ID_t senderID;

    bool exec_res = executeWithCallback([&] (sqlite3_stmt* stmt) {
        senderID = sqlite3_column_int64(stmt, 0);
        text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        return true;
    }, 
        "SELECT sender_id, text FROM  MessagesHistory WHERE chat_id = ? AND id = ?",
        chatID, msgID
    );

    Message msg(chatID, senderID, text);
    if (!msg.isSavedToDB() && exec_res)
        msg.setID(msgID);

    return std::make_optional<Message>(std::move(msg));
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
    if (chatExistsInDB(chat.getID())) {
        return true;
    }

    if (chat.getType() == ChatType::Type::PERSONAL && chat.getName().has_value()) 
        throw std::invalid_argument("Chat is personal but has a name");
    
    if (chat.getType() == ChatType::Type::GROUP 
            && (!chat.getName().has_value() || chat.getName().value().empty())
    ) {
        throw std::invalid_argument("Chat is group but has no name");
    }
    
    bool exec_res = execute(
        "INSERT INTO Chat (name, type) VALUES (?, ?)", 
        chat.getName(), chat.getStringType()
    );

    if (exec_res) chat.setID(sqlite3_last_insert_rowid(db_));

    for (const auto& userID : chat.userIDs_) {
        addMemberToChat(chat.getID(), userID);
    }
    return exec_res;
}

bool DB::save(Chat&& chat) {
    return save(chat); 
}

std::optional<Chat> DB::findChat(ID_t chatId) {
    std::string chatTypeStr;
    std::optional<std::string> chatName;
    std::vector<ID_t> userIDs;

    executeWithCallback([&] (sqlite3_stmt* stmt) {
        
        const unsigned char* typeText = sqlite3_column_text(stmt, 0);
        if (typeText) {
            chatTypeStr = reinterpret_cast<const char*>(typeText);
                  
            const unsigned char* nameText = sqlite3_column_text(stmt, 1);
            if (nameText) {
                chatName = std::string(reinterpret_cast<const char*>(nameText));
            } 
            else { 
                chatName = std::nullopt;
            }
        }
        else {
            std::cerr << "Empty pointer to char" << std::endl;
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
        ORDER BY u.id;)", chatId
    );

    if (chatTypeStr.empty()) {
        std::cerr << "Empty chat type" << std::endl;
    }
    else if (chatTypeStr == "personal" && userIDs.size() != 2) {
        throw std::invalid_argument("Invalid member count for personal chat");
    }

    if (userIDs.empty()) {  
        throw std::invalid_argument("Can not find chat members - usersIDs is empty");
    }

    auto pulled_chat = std::make_optional<Chat>(
        shared_from_this(),
        userIDs,
        ChatType::fromString(chatTypeStr),
        chatName
    );
    pulled_chat->setID(chatId);
    
    return pulled_chat;
}

void DB::deleteChat(ID_t chatID) {
    execute("DELETE FROM Chat WHERE id = ?", chatID);
}

void DB::dropAllTables(const std::string& file_with_drop_query) {
    std::vector<std::string> drop_sql = readSqlQuery(file_with_drop_query);
    for (const auto& query : drop_sql)
        execute(query);
}

bool DB::prepareExecution(const std::string& query, sqlite3_stmt** stmt) {
    if (sqlite3_prepare_v2(db_, query.c_str(), -1, stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Preparing statement error: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    return true;
}
