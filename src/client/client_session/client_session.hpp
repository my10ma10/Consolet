#pragma once
#include <memory>
#include <vector>

#include "user.hpp"
#include "client/client_connection/client_connection.hpp"
#include "db/client_cache_db.hpp"

#include "ui/ui.hpp"

class ClientSession {
    std::optional<ID_t> clientID_ = std::nullopt;

    std::unique_ptr<User> user_;
    std::unique_ptr<ClientConnection> connection_;

    std::shared_ptr<DB> localDB_;

    std::unique_ptr<CommandInterface> ci_;
public:
    ClientSession(
        const std::string& ip_address, 
        const std::string& port
    );

    void auth();
    void start();

    
    void sendToServer(Message&& message) const;

    std::string getHelpMsg() const;
    std::shared_ptr<DB> getLocalDB() const { return localDB_; }
    std::optional<ID_t> getClientID() const { return clientID_; };

    void setUser(std::unique_ptr<User> u);
    void setConnection(std::unique_ptr<ClientConnection> c);

private:
    std::vector<std::unique_ptr<ICommand>> selectUI();
};
