#include "client_session/client_session.hpp"

#define PORT "3490"

int main() {
    ClientSession session("127.0.0.1", PORT);
    session.auth();
    session.start();
    return 0;
}