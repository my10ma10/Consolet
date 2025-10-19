#pragma once
#include <sqlite3.h>
#include <iostream>
#include <functional>
#include <type_traits>
#include <optional>
#include <mutex>

using ID_t = std::size_t;

enum class ChatType { 
    Personal,
    Group
};

class DB {
    sqlite3* db_;
    std::mutex executionMutex_;

public:
    struct UserRow {
        int id;
        std::string name;
        std::string password;
    };

public:
    DB() = default;
    ~DB();

    DB(const DB& other) = delete;
    DB& operator=(const DB& other) = delete;

    DB(DB&& other);
    DB& operator=(DB&& other);

    void init();
    void createDB();

    template <typename... Args>
    bool execute(const std::string& query, Args&&... args);
    
    template <typename Func, typename... Args>
    bool executeWithCallback(Func&& func,
        const std::string& query, Args&&... args);

    void addUser(const std::string& name, const std::string& passwordHash);
    std::optional<UserRow> findUser(const std::string& name);

    void createChat(ID_t user1, ID_t user2, const std::string& type, 
            const std::string& chatName = {});
    void addMessage(ID_t chatID, ID_t senderID, 
            ID_t recieverID, const std::string& msg);

    void deleteChat(ID_t chatID);
    void deleteMessage(ID_t chatID, ID_t msgID);

private:
    bool chatExists(ID_t user1, ID_t user2);
    
    template <typename T>
    void bind(sqlite3_stmt* stmt, unsigned int index, T arg);

    template <typename... Args>
    void bindAll(sqlite3_stmt* stmt, unsigned int index, Args&&... args);

    bool prepareExecution(const std::string& query, sqlite3_stmt** stmt);
};


template <typename... Args>
bool DB::execute(const std::string& query, Args&&... args) {
    std::scoped_lock<std::mutex> lock(executionMutex_);
    sqlite3_stmt* stmt;
    
    bool prepared = prepareExecution(query, &stmt);
    if (!prepared) return false;

    unsigned int index = 1;
    bindAll(stmt, index, std::forward<Args>(args)...);

    int rc = sqlite3_step(stmt);
    bool execStatus = (rc == SQLITE_DONE || rc == SQLITE_ROW);
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
    else if constexpr (std::is_same_v<DecayedT, nullptr_t>) {
        sqlite3_bind_null(stmt, index);
    }
    else {
        throw std::invalid_argument("sqlite3_bind wasn't found\n");
    }
}

