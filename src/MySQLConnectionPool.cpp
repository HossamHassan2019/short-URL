#include "MySQLConnectionPool.hpp"




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


std::unique_ptr<sql::Connection> MySQLConnectionPool::getConnection() {
    std::unique_lock<std::mutex> lock(poolMutex);
    condition.wait(lock, [this] { return !pool.empty(); });
    auto conn = std::move(pool.front());
    pool.pop();
    if (!conn) throw std::runtime_error("No DB connection");
    return conn;
}


void MySQLConnectionPool::releaseConnection(std::unique_ptr<sql::Connection> conn) {
    std::lock_guard<std::mutex> lock(poolMutex);
    pool.push(std::move(conn));
    condition.notify_one();
}
    