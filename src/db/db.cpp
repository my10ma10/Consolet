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
}

void DB::createDB() {
    sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
    
    if (sqlite3_open("database.db", &db) != SQLITE_OK) {
        std::cerr << "Error: cannot open db: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        exit(1);
    }
    std::cout << "DB created\n";
}

void DB::addUser(const std::string& name, const std::string& passwordHash) {
    std::string adding_query = std::format(
        "INSERT INTO User (name, password) VALUES(?, ?)", 
        name, passwordHash
    );
    execute(adding_query, name, passwordHash);
}

bool DB::findUser(const std::string& name) {
    std::string finding_query = std::format(
        "SELECT * FROM User WHERE name = '{}';",
        name
    );
    execute(finding_query);

    return true;
    
}
