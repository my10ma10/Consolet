#include "client.hpp"

Client::Client() {
    localDB_ = std::make_shared<DB>();

    localDB_->init("local.db", std::string(PROJECT_SOURCE_DIR) + "/assets/sql/createDB.sql");
    // ui_ = std::make_unique<UI>(localDB_);
}

void Client::sendToServer(Message&& message) const {
    connection_->sendToServer(message);
}

void Client::run() {
    connection_->start();
}
