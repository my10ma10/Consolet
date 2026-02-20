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
        [&res] (sqlite3_stmt* stmt) {
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


std::optional<Chat> DB::makePulledChat(
    const std::vector<ID_t>& userIDs, const std::string& chatType, 
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

ID_t DB::getLastMsgID(ID_t chatID) {
    // max ID == last ID
    ID_t maxID;
    
    executeWithCallback([&](sqlite3_stmt* stmt){
        maxID = sqlite3_column_int64(stmt, 0);
        return true;
    },
    "SELECT COALESCE(MAX(id), 0) FROM MessagesHistory WHERE chat_id = ?;", 
    chatID
);
    return maxID;
}

bool DB::prepareExecution(const std::string& query, sqlite3_stmt** stmt) {
    if (sqlite3_prepare_v2(db_, query.c_str(), -1, stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Preparing statement error: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }
    return true;
}

std::vector<Message> DB::getMessagesSince(ID_t chatID, ID_t afterMessageID) {
    std::vector<Message> result;
    
    executeWithCallback([&](sqlite3_stmt* stmt) {
        ID_t chatID = sqlite3_column_int64(stmt, 2);
        ID_t senderID = sqlite3_column_int64(stmt, 1);
        auto rawText = sqlite3_column_text(stmt, 4);

        std::string text;
        if (rawText) {
            text = std::string(reinterpret_cast<const char*>(rawText));
        } 
        else { 
            throw std::logic_error("Empty message in ServerDB");
        }        

        result.emplace_back(Message{chatID, senderID, text});
        return true;
    },
    "SELECT * FROM MessagesHistory WHERE chat_id = ? AND id > ?",
    chatID, afterMessageID    
);
    
    return result;
}