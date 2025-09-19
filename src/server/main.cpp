#include "server.hpp"

#define PORT "3490"

int main() {
    Server server("127.0.0.1", PORT);
    server.start();
    return 0;
}

