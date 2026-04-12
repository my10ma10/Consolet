#pragma once
#include "db.hpp"

constexpr int CachedMsgsNumber = 50;

class ClientCacheDB : public DB {

public:
    ClientCacheDB() = default;
    ~ClientCacheDB() = default;
    
    bool save(User& user) override;
    std::optional<User> findUser(const std::string& name) override;
    std::optional<User> findUser(ID_t id) override;

    bool save(Message& message) override;
    std::optional<Message> findMessage(ID_t chatID, ID_t msgID) override;
    std::optional<Message> findMessage(ID_t chatID, const std::string& text) override;

    bool save(Chat& chat) override;
    std::optional<Chat> findChat(ID_t id) override;
    std::optional<Chat> findChat(const std::string& name) override;

    
    void syncChat(ID_t chat_id, std::shared_ptr<ServerDB> server_db);

    void clearCachedMessages(time_t days = 7);
};