#include "db.hpp"
#include <array>

DB::~DB() {
    sqlite3_close(db);
}

void DB::init()
{
    std::array<std::string, 4> creaing_query = {
        R"(CREATE TABLE IF NOT EXISTS User (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,
            password TEXT NOT NULL
        );)",
        R"(CREATE TABLE IF NOT EXISTS Chat (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT,
            type TEXT NOT NULL
        );)",
        R"(CREATE TABLE IF NOT EXISTS MessagesHistory (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            sender_id INTEGER NOT NULL REFERENCES User(id),
            chat_id INTEGER NOT NULL REFERENCES Chat(id),
            date_time TEXT NOT NULL DEFAULT (datetime('now')),
            text TEXT NOT NULL,
            is_read INTEGER NOT NULL DEFAULT 0
        );)",
        R"(CREATE TABLE IF NOT EXISTS ChatMembers (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            chat_id INTEGER NOT NULL REFERENCES Chat(id),
            user_id INTEGER NOT NULL REFERENCES User(id)
        );)"
    };

    createDB();
    for (const auto& q : creaing_query) {
        execute(q);
    }
    std::cout << "DB created\n";
}

void DB::createDB() {
    sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
    
    if (sqlite3_open("database.db", &db) != SQLITE_OK) {
        std::cerr << "Error: cannot open db: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        exit(1);
    }
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

bool DB::prepareExecution(const std::string& query, sqlite3_stmt** stmt) {
    constexpr auto prepareDB = sqlite3_prepare_v2;

    if (prepareDB(db, query.c_str(), -1, stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepating statement error: " << sqlite3_errmsg(db);
        if (stmt) sqlite3_finalize(*stmt);
        return false;
    }
    return true;
}
