#include <gtest/gtest.h>

#include "db/db.hpp"
#include "chat/chat.hpp"
#include "usr/user.hpp"
#include "message/message.hpp"

#include <string>
#include <memory>
#include <thread>
#include <atomic>

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
    
    ssize_t getTableSize(const std::string& tableName, DB& source_db) {
        return source_db.getTableSize(tableName);
    }

    sqlite3* getRawDB(const DB& source_db) const {
        return source_db.db_;
    }
};

TEST_F(DBTest, init_test) {
    TearDown();
    SetUp();
    EXPECT_NE(db, nullptr);
}

TEST_F(DBTest, not_empty_raw_sqlite3_field) {
    User user("Alice", "password1");
    db->save(user);
    
    auto raw_source_db = getRawDB(*db);

    ASSERT_NE(raw_source_db, nullptr);
}

TEST_F(DBTest, db_move_constructor_test) {
    // arrange
    User user("Alice", "password1");
    db->save(user);
    
    auto raw_source_db = getRawDB(*db);
    ssize_t source_db_users_count = getTableSize("User");

    // act
    DB moved_db(std::move(*db));

    // assert
    ASSERT_EQ(raw_source_db, getRawDB(moved_db));
    ASSERT_EQ(getRawDB(*db), nullptr);

    EXPECT_EQ(getTableSize("User", moved_db), 1);
    EXPECT_EQ(source_db_users_count, 1);
}

TEST_F(DBTest, db_move_assignment_test) {
    // arrange
    User user("Alice", "password1");
    db->save(user);
    
    auto raw_source_db = getRawDB(*db);
    ssize_t source_db_users_count = getTableSize("User");
    
    DB moved_db;
    moved_db.init(":memory:", std::string(PROJECT_SOURCE_DIR) + "/assets/sql/createDB.sql");

    auto moved_db_old_users_count = getTableSize("User", moved_db);

    // act
    moved_db = std::move(*db);

    // assert
    ASSERT_EQ(raw_source_db, getRawDB(moved_db));
    ASSERT_EQ(getRawDB(*db), nullptr);

    EXPECT_EQ(moved_db_old_users_count, 0);

    auto moved_db_new_users_count = getTableSize("User", moved_db);
    EXPECT_EQ(moved_db_new_users_count, 1);
    EXPECT_EQ(source_db_users_count, 1);
}

TEST_F(DBTest, right_user_count_after_db_move) {
    // arrange
    User user1("Alice", "password1");
    User user2("Bob", "password2");
    db->save(user1);
    
    auto raw_source_db = getRawDB(*db);

    DB moved_db(std::move(*db));    

    // act
    bool save_res = moved_db.save(user2);

    // assert
    ASSERT_NE(raw_source_db, nullptr);

    EXPECT_EQ(getTableSize("User", moved_db), 2);
    EXPECT_TRUE(save_res);
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

TEST_F(DBTest, sqlite_open__error_processing_test) {
    db.reset();
    db = std::make_shared<DB>();

    EXPECT_THROW(db->init(
        "/invalid/path/to/mydb.db", 
        std::string(PROJECT_SOURCE_DIR) + "/assets/sql/createDB.sql"
    ), std::logic_error);

    EXPECT_EQ(getRawDB(*db), nullptr);
}

TEST_F(DBTest, can_not_open_query_file_test) {
    db.reset();
    db = std::make_shared<DB>();
    
    EXPECT_THROW(db->init(":memory:", "/invalid/path/to/query.sql"), std::logic_error);
    
    EXPECT_EQ(getRawDB(*db), nullptr);
}

// -- User --

TEST_F(DBTest, save_user_to_db) {
    User user("Alice", "password");

    bool save_res = db->save(user);
    
    ASSERT_TRUE(save_res);

    EXPECT_TRUE(user.getID());
    EXPECT_EQ(getTableSize("User"), 1);
}

TEST_F(DBTest, save_rvalue_user) {

    bool save_res = db->save(User{"Alice", "password"});
    
    ASSERT_TRUE(save_res);
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


TEST_F(DBTest, parallel_users_saving) {
    // arrange
    constexpr int Threads_Count = 2;
    constexpr int Users_Count = 3;

    std::vector<std::thread> threads;
    std::atomic<unsigned int> saves_count{0};

    
    for (int i = 0; i < Threads_Count; ++i) {
        threads.emplace_back([this, i, &saves_count] () {
            for (int j = 0; j < Users_Count; ++j) {
                User user("User_" + std::to_string(i) + std::to_string(j), "password" + std::to_string(i) + std::to_string(j));

                // act
                if (db->save(user))
                    ++saves_count;
            }
        });
    }

    for (auto& th : threads) {
        if (th.joinable()) th.join();
    }

    // assert
    EXPECT_EQ(saves_count.load(), Threads_Count * Users_Count);
    EXPECT_EQ(getTableSize("User"), Threads_Count * Users_Count);
}

TEST_F(DBTest, find_saved_user_by_id) {
    User user("Alice", "password1");
    db->save(user);

    auto pulled_user = db->findUser(*user.getID());

    ASSERT_TRUE(pulled_user);
    EXPECT_EQ(user, *pulled_user);
}

TEST_F(DBTest, find_saved_user_by_name) {
    User user("Alice", "password1");
    db->save(user);

    auto pulled_user = db->findUser(user.getName());

    ASSERT_TRUE(pulled_user);
    EXPECT_EQ(user, *pulled_user);
}

// -- Message --

TEST_F(DBTest, save_message_to_db) {
    // arrange
    std::vector<User> users;
    users.emplace_back("Alice", "password1");
    users.emplace_back("Bob", "password2");

    for (User& user : users) {
        ASSERT_TRUE(db->save(user));
    }

    Chat chat(db, users, ChatType::Type::PERSONAL);
    bool save_chat_res = db->save(chat);

    Message msg(*chat.getID(), *users[0].getID(), "Hello from Alice!");

    // act
    bool save_res = db->save(msg);

    // assert
    ASSERT_TRUE(save_chat_res);

    EXPECT_TRUE(save_res);
    EXPECT_TRUE(msg.getID().has_value());
    EXPECT_EQ(getTableSize("MessagesHistory"), 1);
}

TEST_F(DBTest, save_rvalue_message) {
    // arrange
    std::vector<User> users;
    users.emplace_back("Alice", "password1");
    users.emplace_back("Bob", "password2");

    for (User& user : users) {
        ASSERT_TRUE(db->save(user));
    }

    Chat chat(db, users, ChatType::Type::PERSONAL);
    bool save_chat_res = db->save(chat);

    // act
    bool save_res = db->save(Message{*chat.getID(), *users[0].getID(), "Hello from Alice!"});

    // assert
    ASSERT_TRUE(save_chat_res);

    EXPECT_TRUE(save_res);
    EXPECT_EQ(getTableSize("MessagesHistory"), 1);
}

TEST_F(DBTest, find_message_by_id) {
    // arrange
    std::vector<User> users;
    users.emplace_back("Alice", "password1");
    users.emplace_back("Bob", "password2");

    for (User& user : users) {
        ASSERT_TRUE(db->save(user));
    }

    Chat chat(db, users, ChatType::Type::PERSONAL);
    db->save(chat);

    Message msg(*chat.getID(), *users[0].getID(), "Hello from Alice!");
    db->save(msg);

    // act
    auto pulled_msg = db->findMessage(msg.getChatID(), *msg.getID());

    // arrange
    ASSERT_TRUE(pulled_msg.has_value());
    EXPECT_EQ(msg, *pulled_msg);
}

TEST_F(DBTest, find_message_by_text) {
    // arrange
    std::vector<User> users;
    users.emplace_back("Alice", "password1");
    users.emplace_back("Bob", "password2");

    for (User& user : users) {
        ASSERT_TRUE(db->save(user));
    }

    Chat chat(db, users, ChatType::Type::PERSONAL);
    db->save(chat);

    Message msg(*chat.getID(), *users[0].getID(), "Hello from Alice!");
    db->save(msg);

    // act
    auto pulled_msg = db->findMessage(msg.getChatID(), msg.getText());

    // arrange
    ASSERT_TRUE(pulled_msg.has_value());
    EXPECT_EQ(msg, *pulled_msg);
}

TEST_F(DBTest, delete_existing_message) {
    // arrange
    std::vector<User> users;
    users.emplace_back("Alice", "password1");
    users.emplace_back("Bob", "password2");

    for (User& user : users) {
        ASSERT_TRUE(db->save(user));
    }
    Chat chat(db, users, ChatType::Type::PERSONAL);
    db->save(chat);

    Message msg(*chat.getID(), *users[0].getID(), "Hello from Alice!");
    db->save(msg);

    // act 
    bool delete_res = db->deleteMessage(msg.getChatID(), msg.getID().value());

    // assert
    EXPECT_TRUE(delete_res);
    EXPECT_EQ(getTableSize("MessagesHistory"), 0);
}

TEST_F(DBTest, not_delete_not_existing_message) {
    // arrange
    std::vector<User> users;
    users.emplace_back("Alice", "password1");
    users.emplace_back("Bob", "password2");

    for (User& user : users) {
        ASSERT_TRUE(db->save(user));
    }
    Chat chat(db, users, ChatType::Type::PERSONAL);
    db->save(chat);

    Message msg(*chat.getID(), *users[0].getID(), "Hello from Alice!");
    db->save(msg);

    // act 
    bool delete_res = db->deleteMessage(msg.getChatID(), 0);

    // assert
    EXPECT_FALSE(delete_res);
    EXPECT_EQ(getTableSize("MessagesHistory"), 1);
}

// -- Chat --

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

    // assert
    ASSERT_TRUE(save_res);
    ASSERT_TRUE(chat.getID());
    
    EXPECT_EQ(getTableSize("User"), 2);
    EXPECT_EQ(getTableSize("ChatMembers"), 2);
    EXPECT_EQ(getTableSize("Chat"), 1);
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
    bool save_res = db->save(chat);

    // assert
    ASSERT_TRUE(save_res);
    ASSERT_TRUE(chat.getID());

    EXPECT_EQ(getTableSize("User"), 3);
    EXPECT_EQ(getTableSize("ChatMembers"), 3);
    EXPECT_EQ(getTableSize("Chat"), 1);
}

TEST_F(DBTest, save_group_chat_with_one_user) {
    // arrange
    std::vector<User> users;
    users.emplace_back("Alice", "password1");
    
    db->save(users[0]);

    // act
    Chat chat(db, users, ChatType::Type::GROUP, "group chat name");
    bool save_chat_res = db->save(chat);

    // assert
    ASSERT_TRUE(save_chat_res);
    ASSERT_TRUE(chat.getID());

    EXPECT_EQ(getTableSize("User"), 1);
    EXPECT_EQ(getTableSize("ChatMembers"), 1);
    EXPECT_EQ(getTableSize("Chat"), 1);
}

TEST_F(DBTest, save_rvalue_chat) {    
    // arrange
    std::vector<User> users;
    users.emplace_back("Alice", "password1");
    users.emplace_back("Bob", "password2");
    
    for (User& user : users) {
        ASSERT_TRUE(db->save(user));
    }

    
    // act
    bool save_res = db->save(Chat{db, users, ChatType::Type::PERSONAL});

    // assert
    ASSERT_TRUE(save_res);
    
    EXPECT_EQ(getTableSize("User"), 2);
    EXPECT_EQ(getTableSize("ChatMembers"), 2);
    EXPECT_EQ(getTableSize("Chat"), 1);
}

TEST_F(DBTest, not_saving_chat_without_users) {
    std::vector<User> empty_users;
    
    EXPECT_THROW(Chat(db, empty_users, ChatType::Type::PERSONAL), std::invalid_argument);
    EXPECT_THROW(Chat(db, empty_users, ChatType::Type::GROUP), std::invalid_argument);
}

TEST_F(DBTest, not_creating_not_two_users_in_personal_chat) {
    // arrange
    std::vector<User> threeUsers;
    threeUsers.emplace_back("Alice", "password1");
    threeUsers.emplace_back("Bob", "password2");
    threeUsers.emplace_back("Charlie", "password3");
    
    for (User& user : threeUsers) {
        ASSERT_TRUE(db->save(user));
    }   

    // act
    EXPECT_THROW(
        Chat chat(db, threeUsers, ChatType::Type::PERSONAL), 
        std::invalid_argument
    );

    // assert
    EXPECT_EQ(getTableSize("ChatMembers"), 0);
    EXPECT_EQ(getTableSize("Chat"), 0);
}

TEST_F(DBTest, not_creating_personal_chat_with_name) {
    // arrange
    std::vector<User> users;
    users.emplace_back("Alice", "password1");
    users.emplace_back("Bob", "password2");
    
    for (User& user : users) {
        ASSERT_TRUE(db->save(user));
    }
    
    // act
    EXPECT_THROW(
        Chat chat(db, users, ChatType::Type::PERSONAL, "personal chat name"), 
        std::invalid_argument
    );

    // assert
    EXPECT_EQ(getTableSize("ChatMembers"), 0);
    EXPECT_EQ(getTableSize("Chat"), 0);
}

TEST_F(DBTest, not_saving_group_chat_without_name) {
    // arrange
    std::vector<User> users;
    users.emplace_back("Alice", "password1");
    users.emplace_back("Bob", "password2");
    users.emplace_back("Charlie", "password3");
    
    for (User& user : users) {
        ASSERT_TRUE(db->save(user));
    }   

    // act
    EXPECT_THROW(
        Chat chat(db, users, ChatType::Type::GROUP),
        std::invalid_argument
    );

    // assert
    EXPECT_EQ(getTableSize("ChatMembers"), 0);
    EXPECT_EQ(getTableSize("Chat"), 0);
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
    auto pulled_chat = db->findChat(*chat.getID());

    // assert
    ASSERT_TRUE(pulled_chat);
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
    auto pulled_chat = db->findChat(*chat.getID());

    // assert
    ASSERT_TRUE(pulled_chat);
    EXPECT_EQ(chat, *pulled_chat);
}

TEST_F(DBTest, delete_chat_from_db) {
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
    bool del_res = db->deleteChat(*chat.getID());

    // arrange
    ASSERT_TRUE(del_res);
    EXPECT_EQ(getTableSize("Chat"), 0);
    EXPECT_EQ(getTableSize("User"), 2);
}