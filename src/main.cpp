// Final: Crow API + Async MySQL Connector/C++ (modern C++ style)

#include "crow.h"
#include "MySQLConnectionPool.hpp"
#include <iostream>
#include <string>
#include <random>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>


// Random short key generator
std::string generateShortKey(int length = 6) {
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distrib(0, chars.size() - 1);

    std::string key;
    for (int i = 0; i < length; ++i) {
        key += chars[distrib(generator)];
    }
    return key;
}


MySQLConnectionPool connectionPool(10);

std::future<std::string> asyncShortenURL(const std::string& originalUrl) {
    return std::async(std::launch::async, [originalUrl]() {
        try {
            auto conn = connectionPool.getConnection();
            std::string shortKey = generateShortKey();

            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
                "INSERT INTO urls (original_url, short_key) VALUES (?, ?)"
            ));
            pstmt->setString(1, originalUrl);
            pstmt->setString(2, shortKey);
            pstmt->execute();

            connectionPool.releaseConnection(std::move(conn));

            return std::string("http://yourdomain.com/") + shortKey;

        } catch (sql::SQLException& e) {
            return std::string("Error: ") + e.what();
        }
    });
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/shorten").methods("POST"_method)([](const crow::request& req, crow::response& res) {
        auto originalUrl = req.body;
        auto futureResult = asyncShortenURL(originalUrl);
        std::string result = futureResult.get();

        res.set_header("Content-Type", "text/plain");
        res.write(result);
        res.end();
    });

    std::cout << "Starting server on port 8080...\n";
    app.port(8080).multithreaded().run();
    return 0;
}
