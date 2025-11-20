#include <gtest/gtest.h>

#include "db/db.hpp"
#include "chat/chat.hpp"
#include "usr/user.hpp"
#include "message/message.hpp"

#include <string>
#include <memory>

class DBTest : public ::testing::Test {
protected:
    std::shared_ptr<DB> db;

public:
    void SetUp() override {
        db = std::make_shared<DB>();
        db->init(
            ":memory:", 
            std::string(PROJECT_SOURCE_DIR) + "/assets/sql/createDB.sql"
        );
    }
    
    void TearDown() override {
        db.reset();
    }
 
protected:
    ssize_t getTableSize(const std::string& tableName) {
        return db->getTableSize(tableName);
    }
};

TEST_F(DBTest, init_test) {
    EXPECT_NE(db, nullptr);
}

TEST_F(DBTest, save_user_to_db) {
    User user("Alice", "password");

    bool save_res = db->save(user);
    
    EXPECT_TRUE(save_res);
    EXPECT_EQ(getTableSize("User"), 1);
}

TEST_F(DBTest, save_personal_chat_to_db) {
    // arrange
    std::vector<User> users;
    users.emplace_back("Alice", "password1");
    users.emplace_back("Bob", "password2");
    
    for (User& user : users) {
        ASSERT_TRUE(db->save(user));
    }

    Chat chat(db, users, ChatType::Type::PERSONAL);
    
    // act
    bool save_res = db->save(chat);

    // main assert
    EXPECT_EQ(getTableSize("User"), 2);
    EXPECT_EQ(getTableSize("ChatMembers"), 2);
    EXPECT_EQ(getTableSize("Chat"), 1);
    EXPECT_TRUE(save_res);
}

TEST_F(DBTest, save_group_chat_to_db) {
    // arrange
    std::vector<User> users;
    users.emplace_back("Alice", "password1");
    users.emplace_back("Bob", "password2");
    users.emplace_back("Charlie", "password3");
    
    for (User& user : users) {
        ASSERT_TRUE(db->save(user));
    }

    // act
    Chat chat(db, users, ChatType::Type::GROUP, "group chat name");
    db->save(chat);

    // main assert
    EXPECT_EQ(getTableSize("User"), 3);
    EXPECT_EQ(getTableSize("ChatMembers"), 3);
    EXPECT_EQ(getTableSize("Chat"), 1);
}


TEST_F(DBTest, save_group_chat_with_one_user) {
    // arrange
    std::vector<User> users;
    users.emplace_back("Alice", "password1");
    
    ASSERT_TRUE(db->save(users[0]));

    // act
    Chat chat(db, users, ChatType::Type::GROUP, "group chat name");
    db->save(chat);

    // main assert
    EXPECT_EQ(getTableSize("User"), 1);
    EXPECT_EQ(getTableSize("ChatMembers"), 1);
    EXPECT_EQ(getTableSize("Chat"), 1);
}

TEST_F(DBTest, not_saving_chat_without_users) {
    std::vector<User> empty_users;
    
    EXPECT_ANY_THROW(Chat(db, empty_users, ChatType::Type::PERSONAL));
    EXPECT_ANY_THROW(Chat(db, empty_users, ChatType::Type::GROUP));
}

TEST_F(DBTest, find_saved_personal_chat_in_db) {
    // arrange
    std::vector<User> users;
    users.emplace_back("Alice", "password1");
    users.emplace_back("Bob", "password2");

    for (User& user : users) {
        ASSERT_TRUE(db->save(user));
    }

    Chat chat(db, users, ChatType::Type::PERSONAL);
    db->save(chat);

    // act
    auto pulled_chat = db->findChat(chat.getID());

    // assert
    EXPECT_EQ(chat, *pulled_chat);
}

TEST_F(DBTest, find_saved_group_chat_in_db) {
    // arrange
    std::vector<User> users;
    users.emplace_back("Alice", "password1");
    users.emplace_back("Bob", "password2");

    for (User& user : users) {
        ASSERT_TRUE(db->save(user));
    }

    Chat chat(db, users, ChatType::Type::GROUP, "group");
    db->save(chat);

    // act
    auto pulled_chat = db->findChat(chat.getID());

    // assert
    EXPECT_EQ(chat, *pulled_chat);
}

TEST_F(DBTest, check_empty_table_sizes) {
    EXPECT_EQ(getTableSize("User"), 0);
    EXPECT_EQ(getTableSize("Chat"), 0);
    EXPECT_EQ(getTableSize("MessagesHistory"), 0);
    EXPECT_EQ(getTableSize("ChatMembers"), 0);
}

TEST_F(DBTest, check_not_empty_table_sizes) {
    User user("Alice", "password");

    bool res1 = db->save(user);

    EXPECT_TRUE(res1);
    EXPECT_EQ(getTableSize("User"), 1);
}

TEST_F(DBTest, save_user_with_the_same_name) {
    User user("Alice", "password1");
    
    bool res1 = db->save(user);
    bool res2 = db->save(user);

    EXPECT_TRUE(res1);
    EXPECT_FALSE(res2);
    EXPECT_EQ(getTableSize("User"), 1);
}

TEST_F(DBTest, find_saved_user_in_db) {
    User user("Alice", "password1");
    db->save(user);

    auto pulled_user = db->findUser(user.getID());

    EXPECT_EQ(user, *pulled_user);
}

TEST_F(DBTest, save_message_to_db) {
    // arrange
    std::vector<User> users;
    users.emplace_back("Alice", "password1");
    users.emplace_back("Bob", "password2");

    for (User& user : users) {
        ASSERT_TRUE(db->save(user));
    }

    Chat chat(db, users, ChatType::Type::PERSONAL);
    db->save(chat);

    Message msg(chat.getID(), users[0].getID(), "Hello from Alice!");

    // act
    bool save_res = db->save(msg);

    // assert
    EXPECT_TRUE(save_res);
    EXPECT_TRUE(msg.getID().has_value());
    EXPECT_EQ(getTableSize("MessagesHistory"), 1);
}

TEST_F(DBTest, check_message_deleted) {
    // arrange
    std::vector<User> users;
    users.emplace_back("Alice", "password1");
    users.emplace_back("Bob", "password2");

    for (User& user : users) {
        ASSERT_TRUE(db->save(user));
    }
    Chat chat(db, users, ChatType::Type::PERSONAL);
    db->save(chat);

    Message msg(chat.getID(), users[0].getID(), "Hello from Alice!");
    db->save(msg);

    // act 
    bool delete_res = db->deleteMessage(msg.getChatID(), msg.getID().value());

    // assert
    EXPECT_TRUE(delete_res);
    EXPECT_EQ(getTableSize("MessagesHistory"), 0);
}