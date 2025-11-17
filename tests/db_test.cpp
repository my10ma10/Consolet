#include <gtest/gtest.h>

#include "db/db.hpp"
#include "chat/chat.hpp"
#include "usr/user.hpp"

#include <string>
#include <memory>

class DBTest : public ::testing::Test {
protected:
    std::shared_ptr<DB> db;

public:
    void SetUp() override {
        db = std::make_unique<DB>();
        db->init(
            ":memory:", 
            std::string(PROJECT_SOURCE_DIR) + "/assets/sql/createDB.sql"
        );
    }
    
    void TearDown() override {
        db.reset();
    }
};

TEST_F(DBTest, init_test) {
    EXPECT_NE(db, nullptr);
}

TEST_F(DBTest, save_user_to_db) {
    User user("Alice", "password");

    bool res = db->save(user);

    EXPECT_TRUE(res);
    EXPECT_EQ(db->getTableSize("User"), 1);
}

TEST_F(DBTest, save_personal_chat_to_db) {
    User user1("Alice", "password1");
    User user2("Bob", "password2");

    bool res1 = db->save(user1);
    bool res2 = db->save(user2);
    Chat chat(db, {user1.getID(), user2.getID()});

    EXPECT_TRUE(res1);
    EXPECT_TRUE(res2);
    EXPECT_EQ(db->getTableSize("User"), 1);
}

TEST_F(DBTest, save_group_chat_to_db) {
    User user1("Alice", "password1");
    User user2("Bob", "password2");

    bool res1 = db->save(user1);
    bool res2 = db->save(user2);
    Chat chat(db, {user1.getID(), user2.getID()});
    

    EXPECT_TRUE(res1);
    EXPECT_TRUE(res2);
    EXPECT_EQ(db->getTableSize("User"), 1);
}

TEST_F(DBTest, not_saving_chat_without_users) {
    // Chat chat;
}

TEST_F(DBTest, check_empty_table_sizes) {
    EXPECT_EQ(db->getTableSize("User"), 0);
    EXPECT_NO_THROW(db->getTableSize("Chat"));
    EXPECT_EQ(db->getTableSize("Chat"), 0);
    EXPECT_EQ(db->getTableSize("MessagesHistory"), 0);
    EXPECT_EQ(db->getTableSize("ChatMembers"), 0);
}

TEST_F(DBTest, check_not_empty_table_sizes) {
    User user("Alice", "password");

    bool res1 = db->save(user);

    EXPECT_TRUE(res1);
    EXPECT_EQ(db->getTableSize("User"), 1);
}