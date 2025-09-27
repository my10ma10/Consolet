#include "client.hpp"

#define PORT "3490"

int main() {
    Connection client("127.0.0.1", PORT);
    client.start();
    
    return 0;
}