#include "user.hpp"

void User::notify() {
    ++notificationCount_;
    std::cout << "You have " << notificationCount_ << " notifications";
}
