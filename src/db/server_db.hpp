#pragma once
#include "db.hpp"


class ServerDB : public DB  {
public:
    ServerDB() = default;

    ServerDB(ServerDB&& other) noexcept = default;
    ServerDB& operator=(ServerDB&& other) noexcept = default;

    // -- User --
    bool save(User& user) override;
    bool save(User&& user);

    std::optional<User> findUser(const std::string& name) override;
    std::optional<User> findUser(ID_t id) override;
    
    bool deleteUser(ID_t userID);

    // -- Message --
    bool save(Message& message) override;
    bool save(Message&& message);

    std::optional<Message> findMessage(ID_t chatID, ID_t msgID) override;
    std::optional<Message> findMessage(ID_t chatID, const std::string& text) override;

    bool deleteMessage(ID_t chatID, ID_t msgID);


    // -- Chat --
    bool save(Chat& chat) override;
    bool save(Chat&& chat);

    std::optional<Chat> findChat(ID_t id) override;
    std::optional<Chat> findChat(const std::string& name) override;

    // std::optional<Chat> findPersonalChat(ID_t clientID, ID_t otherID);
    // std::optional<Chat> findPersonalChat(ID_t clientID, const std::string otherName);

    bool deleteChat(ID_t chatID);


private:
    void addMemberToChat(ID_t userID, ID_t chatId);
};