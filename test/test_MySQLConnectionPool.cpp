#include <gtest/gtest.h>
#include "MySQLConnectionPool.hpp"

class MySQLConnectionPoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialization code here
    }

    void TearDown() override {
        // Cleanup code here
    }
};

TEST_F(MySQLConnectionPoolTest, PoolInitialization) {
    int poolSize = 3;
    MySQLConnectionPool pool(poolSize);

    // The pool should be initialized with the correct number of connections
    EXPECT_EQ(pool.maxConnections, poolSize);
}

TEST_F(MySQLConnectionPoolTest, AcquireAndReleaseConnection) {
    int poolSize = 3;
    MySQLConnectionPool pool(poolSize);

    // Acquire a connection from the pool
    auto connection = pool.getConnection();
    EXPECT_NE(connection, nullptr);

    // Release the connection back to the pool
    pool.releaseConnection(std::move(connection));

    // Ensure the connection is returned to the pool and can be re-acquired
    auto connection2 = pool.getConnection();
    EXPECT_NE(connection2, nullptr);
}

TEST_F(MySQLConnectionPoolTest, AcquireAllConnections) {
    int poolSize = 3;
    MySQLConnectionPool pool(poolSize);

    std::vector<std::unique_ptr<MySqlUser>> connections;

    // Acquire all connections from the pool
    for (int i = 0; i < poolSize; ++i) {
        auto connection = pool.getConnection();
        EXPECT_NE(connection, nullptr);
        connections.push_back(std::move(connection));
    }

    // Try to acquire another connection, which should block or throw an error
    EXPECT_THROW(pool.getConnection(), std::runtime_error);

    // Release all connections back to the pool
    for (auto& connection : connections) {
        pool.releaseConnection(std::move(connection));
    }

    // Ensure connections can be re-acquired
    for (int i = 0; i < poolSize; ++i) {
        auto connection = pool.getConnection();
        EXPECT_NE(connection, nullptr);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
