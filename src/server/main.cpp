#include "server.hpp"
#include "db/db.hpp"

#include <format>
#include <iostream>

#define PORT "3490"

int main() {
    Server server("127.0.0.1", PORT);
    server.start();
    return 0;
}

