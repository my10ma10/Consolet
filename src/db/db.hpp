#pragma once
#include <sqlite3.h>

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
class DBTest;

class DB : public std::enable_shared_from_this<DB> {
public:
    friend class DBTest;

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

    // -- User --
    bool save(User& user);
    bool save(User&& user);

    void addMemberToChat(ID_t chatId, ID_t userID);

    std::optional<User> findUser(const std::string& name);
    std::optional<User> findUser(ID_t id);
    

    // -- Message --
    bool save(Message& message);
    bool save(Message&& message);

    std::optional<Message> findMessage(ID_t chatID, ID_t msgID);

    bool deleteMessage(ID_t chatID, ID_t msgID);


    // -- Chat --
    bool save(Chat& chat);
    bool save(Chat&& chat);

    std::optional<Chat> findChat(ID_t id);

    void deleteChat(ID_t chatID);
    
private:
    void createDB(const std::string& db_name, const std::vector<std::string>& sql);

    void dropAllTables(const std::string& dropQuery);

    std::vector<std::string> readSqlQuery(const std::string& filename);

    bool chatExistsInDB(ID_t chatID);
    
    template <typename T>
    void bind(sqlite3_stmt* stmt, unsigned int index, T&& arg);

    template <typename... Args>
    void bindAll(sqlite3_stmt* stmt, unsigned int index, Args&&... args);

    bool prepareExecution(const std::string& query, sqlite3_stmt** stmt);

    ssize_t getTableSize(const std::string& tableName); 
};


template <typename... Args>
bool DB::execute(const std::string& query, Args&&... args) {
    if (!db_) {
        throw std::runtime_error("Database not initialized");
    }

    std::scoped_lock<std::mutex> lock(executionMutex_);

    sqlite3_stmt* stmt;

    if (!prepareExecution(query, &stmt)) return false;

    unsigned int index = 1;
    bindAll(stmt, index, std::forward<Args>(args)...);

    int rc = sqlite3_step(stmt);
    bool success = (rc == SQLITE_DONE || rc == SQLITE_ROW || rc == SQLITE_OK);
    if (!success) {
        std::cerr << "SQLite step failed (rc = " << rc << "): "
              << sqlite3_errstr(rc) << " | " << sqlite3_errmsg(db_) << std::endl;
        
    }

    sqlite3_finalize(stmt);
    return success;
}

template <typename Func, typename... Args>
bool DB::executeWithCallback(
    Func&& func,
    const std::string& query, 
    Args&&... args
) {
    if (!db_) {
        throw std::runtime_error("Database not initialized");
    }
    
    std::scoped_lock<std::mutex> lock(executionMutex_);
    sqlite3_stmt* stmt = nullptr;

    if (!prepareExecution(query, &stmt)) return false;

    unsigned int index = 1;
    bindAll(stmt, index, std::forward<Args>(args)...);

    int rc = SQLITE_OK;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (!std::forward<Func>(func)(stmt)) {
            break;
        }
    }
    
    bool success = (rc == SQLITE_DONE || rc == SQLITE_ROW);
    if (!success) {
        std::cerr << "Execution error: " << sqlite3_errmsg(db_) << std::endl;
    }
    if (rc == SQLITE_ERROR || rc == SQLITE_MISUSE || rc == SQLITE_CONSTRAINT) {
        std::cerr << "";
        sqlite3_finalize(stmt);
        return false;
    }

    if (stmt) sqlite3_finalize(stmt);
    else throw std::runtime_error("Empty stmt");

    return success;
}

template <typename... Args>
void DB::bindAll(sqlite3_stmt* stmt, unsigned int index, Args&&... args) {
    if constexpr (sizeof... (Args) > 0) {
        (bind(stmt, index++, std::forward<Args>(args)), ...);
    }
}

template <typename T>
void DB::bind(sqlite3_stmt* stmt, unsigned int index, T&& arg) {
    using DecayedT = std::remove_cvref_t<T>;
    int r;

    if constexpr (std::is_same_v<DecayedT, std::optional<std::string>>) {
        if (arg.has_value()) {
            bind(stmt, index, arg.value());
        } else {
            r = sqlite3_bind_null(stmt, index);
        }
    }
    
    else if constexpr (std::is_same_v<DecayedT, double>) {
        r = sqlite3_bind_double(stmt, index, arg);
    }
    else if constexpr (std::is_same_v<DecayedT, int>) {
        r = sqlite3_bind_int(stmt, index, arg);
    }
    else if constexpr (std::is_same_v<DecayedT, int64_t>) {
        r = sqlite3_bind_int64(stmt, index, arg);
    }
    else if constexpr (std::is_same_v<DecayedT, std::string>) {
        r = sqlite3_bind_text(stmt, index, arg.c_str(), -1, SQLITE_TRANSIENT);
        if (r != SQLITE_OK) {
            std::cerr << "bind_text failed rc=" << r << " (" << sqlite3_errstr(r)
                    << "): idx=" << index << " value='" << arg << "'\n";
        }
    }
    else if constexpr (std::is_same_v<DecayedT, const char*> || std::is_same_v<DecayedT, char*>) {
        r = sqlite3_bind_text(stmt, index, arg, -1, SQLITE_TRANSIENT);
        if (r != SQLITE_OK) {
            std::cerr << "bind_text failed rc=" << r << " (" << sqlite3_errstr(r)
                    << "): idx=" << index << " value='" << arg << "'\n";
        }
    }
    else if constexpr (
                std::is_same_v<DecayedT, nullptr_t> ||
                std::is_same_v<DecayedT, std::nullopt_t>
        ) {
        r = sqlite3_bind_null(stmt, index);
    }
    else {
        throw std::invalid_argument("sqlite3_bind was not found\n");
    }
}

