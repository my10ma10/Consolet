#pragma once
#include <sqlite3.h>
#include <iostream>
#include <format>
#include <type_traits>

class DB {
    using PreparedStatement = sqlite3_stmt;

    sqlite3* db;

    PreparedStatement* stmt;

    template <typename T>
    void bind(unsigned int index, T arg);
public: 
    ~DB();

    void init();
    void createDB();

    template <typename... Args>
    void execute(const std::string query, Args&&... args);

    void addUser(const std::string& name, const std::string& passwordHash);

    bool findUser(const std::string& name);
};

template <typename... Args>
void DB::execute(const std::string query, Args&&... args) {
    if (sqlite3_prepare_v2(db, query.data(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepating statement error: " << sqlite3_errmsg(db);
    }

    unsigned int index = 1;
    (bind(index++, std::forward<Args>(args)), ...);


    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Insert error: " << sqlite3_errmsg(db) << std::endl;
    }    
    sqlite3_finalize(stmt);
}

template <typename T>
void DB::bind(unsigned int index, T arg) {
    using DecayedT = std::remove_cvref_t<T>;
    
    if constexpr (std::is_same_v<DecayedT, double>) {
        sqlite3_bind_double(stmt, index, arg);
    }
    else if constexpr (std::is_same_v<DecayedT, int>) {
        sqlite3_bind_int(stmt, index, arg);
    }
    else if constexpr (std::is_same_v<DecayedT, std::string>) {
        sqlite3_bind_text(stmt, index, arg.c_str(), -1, SQLITE_STATIC);
    }
    else if constexpr (std::is_same_v<DecayedT, const char*> || std::is_same_v<DecayedT, char*>) {
        sqlite3_bind_text(stmt, index, arg, -1, SQLITE_STATIC);
    }
    else if constexpr (std::is_same_v<DecayedT, nullptr_t>) {
        sqlite3_bind_null(stmt, index);
    }
    else {
        throw std::invalid_argument("sqlite3_bind wasn't found\n");
    }
}
