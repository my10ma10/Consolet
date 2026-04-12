#include <gtest/gtest.h>

#include "db/client_cache_db.hpp"


class CacheDBTest : public ::testing::Test {
protected:
    std::shared_ptr<ClientCacheDB> db;

public: 
    void SetUp() override {
        db = std::make_shared<ClientCacheDB>();
        db->init(
            ":memory:", std::string(PROJECT_SOURCE_DIR) + 
                "/assets/sql/create_client_cache_db.sql"
        );
    }

    void TearDown() override {
        db.reset();
    }
};

TEST_F(CacheDBTest, init_test) {
    TearDown();
    SetUp();
    EXPECT_NE(db, nullptr);
}

