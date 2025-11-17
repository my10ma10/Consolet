#pragma once
#include <sqlite3.h>
#include <cassert>
#include <iostream>
#include <type_traits>
#include <optional>
#include <memory>
#include <mutex>
#include <vector>

#include "chat/chat_type.hpp"

using ID_t = int64_t;

class User;
class Chat;
class Message;

class DB : public std::enable_shared_from_this<DB> {
protected:
    sqlite3* db_;
    std::mutex executionMutex_;

public:
    DB() = default;
    ~DB();

    DB(const DB& other) = delete;
    DB& operator=(const DB& other) = delete;

    DB(DB&& other);
    DB& operator=(DB&& other);

    void init(const std::string& db_name, const std::string& sqlFile);

    template <typename... Args>
    bool execute(const std::string& query, Args&&... args);
    
    template <typename Func, typename... Args>
    bool executeWithCallback(Func&& func,
        const std::string& query, Args&&... args);

    ssize_t getTableSize(const std::string& tableName); 

    // -- User --
    bool save(User& user);
    void addMemberToChat(ID_t chatId, ID_t userID);

    std::optional<User> findUser(const std::string& name);
    std::optional<User> findUser(ID_t id);
    
    // -- Message --
    bool save(Message& message);

    void deleteMessage(ID_t msgID);


    // -- Chat --
    bool save(Chat& chat);
    void createChat(std::vector<User> users, ChatType type, 
            const std::string& chatName = {});

    std::unique_ptr<Chat> findChat(ID_t id);

    void deleteChat(ID_t chatID);
    
private:
    void createDB(const std::string& db_name, const std::vector<std::string>& sql);

    void dropAllTables(const std::string& dropQuery);

    std::vector<std::string> readSqlQuery(const std::string& filename);

    bool chatExists(ID_t chatID);
    
    template <typename T>
    void bind(sqlite3_stmt* stmt, unsigned int index, T arg);

    template <typename... Args>
    void bindAll(sqlite3_stmt* stmt, unsigned int index, Args&&... args);

    bool prepareExecution(const std::string& query, sqlite3_stmt** stmt);
};


template <typename... Args>
bool DB::execute(const std::string& query, Args&&... args) {
    if (!db_) {
        throw std::runtime_error("Database not initialized");
    }

    std::scoped_lock<std::mutex> lock(executionMutex_);

    sqlite3_stmt* stmt;
    
    bool prepared = prepareExecution(query, &stmt);
    if (!prepared) return false;

    unsigned int index = 1;
    bindAll(stmt, index, std::forward<Args>(args)...);

    int rc = sqlite3_step(stmt);
    bool execStatus = (rc != SQLITE_DONE || rc != SQLITE_ROW);
    if (!execStatus) {
        std::cerr << "Execution error: " << sqlite3_errmsg(db_) << std::endl;
    }

    sqlite3_finalize(stmt);
    return execStatus;    
}

template <typename Func, typename... Args>
bool DB::executeWithCallback(
    Func&& func,
    const std::string& query, 
    Args&&... args
) {
    std::scoped_lock<std::mutex> lock(executionMutex_);
    sqlite3_stmt* stmt = nullptr;

    bool prepared = prepareExecution(query, &stmt);
    if (!prepared) return false;

    unsigned int index = 1;
    bindAll(stmt, index, std::forward<Args>(args)...);

    int rc = SQLITE_OK;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (!std::forward<Func>(func)(stmt)) {
            break;
        }
    }
    
    bool execStatus = (rc == SQLITE_DONE || rc == SQLITE_ROW);
    if (!execStatus) {
        std::cerr << "Execution error: " << sqlite3_errmsg(db_) << std::endl;
    }

    if (stmt) sqlite3_finalize(stmt);
    return execStatus;
}

template <typename... Args>
void DB::bindAll(sqlite3_stmt* stmt, unsigned int index, Args&&... args) {
    if constexpr (sizeof... (Args) > 0) {
        (bind(stmt, index++, std::forward<Args>(args)), ...);
    }
}

template <typename T>
void DB::bind(sqlite3_stmt* stmt, unsigned int index, T arg) {
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
    else if constexpr (
                std::is_same_v<DecayedT, nullptr_t> ||
                std::is_same_v<DecayedT, std::nullopt_t>
        ) {
        sqlite3_bind_null(stmt, index);
    }
    else {
        throw std::invalid_argument("sqlite3_bind wasn't found\n");
    }
}

