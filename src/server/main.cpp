#include "server.hpp"
#include "db/db.hpp"
#include "defines.hpp"

#include "spdlog/spdlog.h"

#include <format>
#include <iostream>

int main() {
    
    spdlog::set_level(spdlog::level::debug);
    try {
        Server server("127.0.0.1", PORT);
        server.start();
        return 0;
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
}

