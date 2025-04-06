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
    
    for (int i = 0; i < maxConnections; ++i) {
        std::unique_ptr<MySqlUser> user = std::make_unique<MySqlUser>();
        pool.push(std::move(user));
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
std::unique_ptr<MySqlUser> MySQLConnectionPool::getConnection() {
    std::unique_lock<std::mutex> lock(poolMutex);
    condition.wait(lock, [this] { return !pool.empty(); });
    auto user = std::move(pool.front());
    user->checkConnection();
    pool.pop();
    if (!user) throw std::runtime_error("No DB connection");
    return user;
}

/**
 * @brief        Returns a previously acquired connection back to the pool.
 *
 * @details      Places the connection back into the pool and notifies one waiting
 *               thread that a connection is now available.
 *
 * @param[in]    conn   A unique pointer to the SQL connection being returned.
 */
void MySQLConnectionPool::releaseConnection(std::unique_ptr<MySqlUser> user) {
    std::lock_guard<std::mutex> lock(poolMutex);
    pool.push(std::move(user));
    condition.notify_one();
}
    