#include <fstream>

#include "client/client.hpp"
#include "defines.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

int main() {
    spdlog::set_level(spdlog::level::debug);
    try {
        auto file_logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.log");
        
        spdlog::set_default_logger(file_logger);
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Logger initialization error: " << ex.what() << std::endl;
    }
    try {
        Client client("127.0.0.1", PORT);
        client.auth();
        client.start();
        return 0;
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
}