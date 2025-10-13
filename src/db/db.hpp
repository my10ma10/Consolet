#pragma once
#include <sqlite3.h>
#include <iostream>
#include <format>

class DB {
    sqlite3* db;

    sqlite3_stmt* stmt;

public: 
    ~DB();

    void init();
    void createDB();

    void execute(const char* sql);
    void execute(const std::string& sql);
    void execute(const char* sql, int (*callback)(void*, int, char**, char**));

    void addUser(const char* name, const char* passwordHash);

    bool findUser(const std::string& name);
};

