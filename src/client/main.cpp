#include <fstream>

#include "client/client.hpp"
#include "defines.hpp"

int main() {
    spdlog::set_level(spdlog::level::debug);

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