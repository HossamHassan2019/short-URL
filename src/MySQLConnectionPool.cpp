#include "MySQLConnectionPool.hpp"



/**
 * @brief        Constructs a MySQL connection pool with a specified size.
 *
 * @details      Initializes the connection pool by creating `poolSize` number
 *               of connections to the MySQL database. Each connection is configured
 *               to auto-reconnect and uses the "url_shortener" schema.
 *
 * @param[in]    poolSize   The number of database connections to maintain in the pool.
 */
MySQLConnectionPool::MySQLConnectionPool(int poolSize) : maxConnections(poolSize) {
    driver = sql::mysql::get_mysql_driver_instance();
    for (int i = 0; i < maxConnections; ++i) {
        auto conn = std::unique_ptr<sql::Connection>(
            driver->connect("tcp://127.0.0.1:3306", "cpp_user", "Hosam$$2018")
        );
        bool reconnect = true;
        conn->setClientOption("OPT_RECONNECT", &reconnect);
        conn->setSchema("url_shortener");
        pool.push(std::move(conn));
    }
}

/**
 * @brief        Acquires a database connection from the pool.
 *
 * @details      Waits if necessary until a connection becomes available.
 *               Once retrieved, the connection is removed from the pool and returned.
 *
 * @return       A unique pointer to an active SQL connection.
 *
 * @throws       std::runtime_error if the retrieved connection is null.
 */
std::unique_ptr<sql::Connection> MySQLConnectionPool::getConnection() {
    std::unique_lock<std::mutex> lock(poolMutex);
    condition.wait(lock, [this] { return !pool.empty(); });
    auto conn = std::move(pool.front());
    pool.pop();
    if (!conn) throw std::runtime_error("No DB connection");
    return conn;
}

/**
 * @brief        Returns a previously acquired connection back to the pool.
 *
 * @details      Places the connection back into the pool and notifies one waiting
 *               thread that a connection is now available.
 *
 * @param[in]    conn   A unique pointer to the SQL connection being returned.
 */
void MySQLConnectionPool::releaseConnection(std::unique_ptr<sql::Connection> conn) {
    std::lock_guard<std::mutex> lock(poolMutex);
    pool.push(std::move(conn));
    condition.notify_one();
}
    