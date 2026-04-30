#pragma once
#include <memory>
#include <vector>

#include "defines.hpp"

#include "user/user.hpp"
#include "client_connection/client_connection.hpp"
#include "db/server_db.hpp"

#include "ui/ui.hpp"
#include "command/command_interface.hpp"

class ClientSession {
    std::unique_ptr<User> user_;
    std::atomic<bool> is_active_{true};

    Socket socket_;
    std::shared_ptr<ServerDB> db_ = nullptr;

public:
    ClientSession(Socket&& socket);
    ~ClientSession();

    ClientSession(const ClientSession&) = delete;
    ClientSession& operator=(const ClientSession&) = delete;

    ClientSession(ClientSession&& other) = delete;
    ClientSession& operator=(ClientSession&& other)  = delete;

    void start();
    void stop();
};
