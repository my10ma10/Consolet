#include <fstream>

#include "client_session/client_session.hpp"

#define PORT "3490"
// #define DEBUG

int main() {
    try {
        #ifdef DEBUG
            std::ofstream logFile("~/.mini_messenger/client.log");
            std::clog.rdbuf(logFile.rdbuf());
        #endif // DEBUG

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