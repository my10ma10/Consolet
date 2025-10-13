#include "db.hpp"

#include <openssl/evp.h>
#include <openssl/rand.h>

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

void DB::execute(const char* sql) {
    char* errMsg;

    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Query executing error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

void DB::execute(const std::string& sql) {
    execute(sql.data());
}

void DB::execute(const char* sql, int (*callback)(void*, int, char**, char**)) {
    char* errMsg;

    if (sqlite3_exec(db, sql, callback, nullptr, &errMsg)) {
        std::cerr << "Query executing error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

void DB::addUser(const char* name, const char* passwordHash) {
    std::string adding_query = std::format(
        "INSERT INTO users VALUES('{}', '{}')", 
        name, passwordHash
    );
    execute(adding_query);
}

bool DB::findUser(const std::string& name) {
    std::string finding_query = std::format(
        "SELECT * FROM User WHERE name = '{}';",
        name
    );
    execute(finding_query);

    return true;
    
}
