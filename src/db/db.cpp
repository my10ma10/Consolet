#include "db.hpp"
#include <array>
#include <iterator>
#include <sstream>
#include <fstream>

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

void DB::init(const std::string& sqlFile) {
    std::string sql = readSqlQuery(sqlFile);

    createDB(sql);

    std::cout << "DB created\n";
}

void DB::createDB(const std::string& sql) {
    sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
    
    if (sqlite3_open("database.db", &db_) != SQLITE_OK) {
        std::cerr << "Error: cannot open db: " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        exit(1);
    }
    execute(sql);
}

void DB::addUser(const std::string& name, const std::string& passwordHash) {
    std::string adding_query = "INSERT INTO User (name, password) VALUES(?, ?)";
    execute(adding_query, name, passwordHash);
}

std::optional<DB::UserRow> DB::findUser(const std::string& name) {
    std::string finding_query = "SELECT * FROM User WHERE name = ?;";
    std::optional<DB::UserRow> result;
    
    executeWithCallback([&result] (sqlite3_stmt* stmt) {
        UserRow userRow;
        userRow.id = sqlite3_column_int(stmt, 0);
        userRow.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        userRow.password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        result = std::move(userRow);
        return true;
    }, finding_query, name);

    return result;    
}

void DB::addMessage(
    ID_t chatID, 
    ID_t senderID, 
    ID_t recieverID, 
    const std::string& msg
) {
    if (!chatExists(senderID, recieverID)) {
        createChat(senderID, recieverID, "personal");
    }
    
    execute(
        "INSERT INTO MessagesHistory (sender_id, chat_id, text) \
            VALUES (?, ?, ?)",
            senderID, chatID, msg
    );
}

void DB::deleteChat(ID_t chatID) {
    execute("DELETE FROM Chat WHERE id = ?", chatID);
}

std::string DB::readSqlQuery(const std::string& filename) {
    std::stringstream buffer;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: can not open query SQL file " << filename << std::endl;
    }

    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

bool DB::chatExists(ID_t user1, ID_t user2)
{
    bool exists = false;

    executeWithCallback([&exists] (sqlite3_stmt* stmt) {
            exists = sqlite3_column_int(stmt, 0);
            return false;
        },
        R"(
            SELECT COUNT(DISTINCT user_id) = 2 
            FROM ChatMembers 
            WHERE user_id IN (?, ?)
        )", user1, user2
    );
        
    return exists;
}

void DB::createChat(
    ID_t user1, 
    ID_t user2, 
    const std::string& type, 
    const std::string& chatName
) {    
    std::string creating_chat_query = \
    "INSERT INTO Chat (name, type) VALUES (NULL, ?);";

    execute(creating_chat_query, std::string{}, "personal");

    ID_t newChatID = static_cast<ID_t>(sqlite3_last_insert_rowid(db_));

    std::string adding_members_query = \
    "INSERT INTO ChatMembers (chat_id, user_id) VALUES (?, ?);";
    execute(adding_members_query, newChatID, user1);
    execute(adding_members_query, newChatID, user2);

}

bool DB::prepareExecution(const std::string& query, sqlite3_stmt** stmt) {
    if (sqlite3_prepare_v2(db_, query.c_str(), -1, stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepating statement error: " << sqlite3_errmsg(db_) << std::endl;

        if (*stmt) sqlite3_finalize(*stmt);
        return false;
    }
    return true;
}
