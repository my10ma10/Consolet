#include <fstream>

#include "client_session/client_session.hpp"
#include "defines.hpp"

int main() {
    spdlog::set_level(spdlog::level::debug);

    try {
        ClientSession session("127.0.0.1", PORT);
        session.auth();
        session.start();
        return 0;
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
}