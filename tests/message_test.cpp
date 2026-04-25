#include <gtest/gtest.h>

#include "message/message.hpp"

TEST(MessageTest, equal_messages_is_equal) {
    Message msg1(1, 2, "3", 4, 5);
    Message msg2(1, 2, "3", 4, 5);

    EXPECT_EQ(msg1, msg2);
}