#include "user.hpp"

void User::notify() {
    ++_notificationCount;
    std::cout << "You have " << _notificationCount << " notifications";
}