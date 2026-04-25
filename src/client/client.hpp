#pragma once

// #include "client_connection/client_connection.hpp"
// #include "command.hpp"
// #include "db/client_cache_db.hpp"

// class Client {    
//     std::unique_ptr<ClientConnection> connection_;
//     std::shared_ptr<DB> localDB_;

//     // std::unique_ptr<UI> ui_;

// public:
//     Client();

//     void sendToServer(Message&& message) const;

//     void run();

//     std::shared_ptr<DB> getLocalDB() const { return localDB_; }
// };