// Final: Crow API + Async MySQL Connector/C++ (modern C++ style)

#include "crow.h"
#include "MySQLConnectionPool.hpp"
#include "ThreadPool.hpp"
#include <iostream>
#include <string>
#include <random>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <regex>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>



MySQLConnectionPool ReadOperationconnectionPool(10);
MySQLConnectionPool WriteOperationconnectionPool(10);
ThreadPool threadPool(10);


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




std::future<std::string> asyncShortenURL(const std::string& originalUrl) {
    return std::async(std::launch::async, [originalUrl]() {
        try {
            auto conn = WriteOperationconnectionPool.getConnection();
            std::string shortKey = generateShortKey();

            std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement(
                "INSERT INTO urls (original_url, short_key ) VALUES (?, ?)"
            ));
            pstmt->setString(1, originalUrl);
            pstmt->setString(2, shortKey);
            pstmt->execute();

            WriteOperationconnectionPool.releaseConnection(std::move(conn));

            return  shortKey;

        } catch (sql::SQLException& e) {
            return std::string("Error: ") + e.what();
        }
    });
}






std::future<std::string> asyncOriginalUrl(const std::string& shortUrl) {
    return std::async(std::launch::async, [shortUrl]() {
        try {
            auto conn = ReadOperationconnectionPool.getConnection();

            // Step 1: Get original URL
            std::unique_ptr<sql::PreparedStatement> selectStmt(conn->prepareStatement(
                "SELECT original_url FROM urls WHERE short_key = ?"
            ));
            selectStmt->setString(1, shortUrl);
            auto result = std::unique_ptr<sql::ResultSet>(selectStmt->executeQuery());

            if (result->next()) {
                std::string originalUrl = result->getString("original_url");
                threadPool.enqueue([shortUrl]() {
                
                        auto conn = WriteOperationconnectionPool.getConnection();
                        
                        // Step 2: Increment clicks directly in SQL
                        std::unique_ptr<sql::PreparedStatement> updateStmt(conn->prepareStatement(
                            "UPDATE urls SET clicks = clicks + 1 WHERE short_key = ?"
                        ));
                        updateStmt->setString(1, shortUrl);
                        updateStmt->execute();

                        std::cout << "Clicks incremented for " << shortUrl << std::endl;
                        WriteOperationconnectionPool.releaseConnection(std::move(conn));
               
                });
                
                ReadOperationconnectionPool.releaseConnection(std::move(conn));
                return originalUrl;
            } else {
                ReadOperationconnectionPool.releaseConnection(std::move(conn));
                return std::string("URL not found");
            }

        } catch (sql::SQLException& e) {
            return std::string("Error: ") + e.what();
        }
    });
}


std::future<int> asyncStats(const std::string& shortUrl) {
    return std::async(std::launch::async, [shortUrl]() {
        try {
            auto conn = ReadOperationconnectionPool.getConnection();

            // Step 1: Get original URL
            std::unique_ptr<sql::PreparedStatement> selectStmt(conn->prepareStatement(
                "SELECT clicks FROM urls WHERE short_key = ?"
            ));
            selectStmt->setString(1, shortUrl);
            auto result = std::unique_ptr<sql::ResultSet>(selectStmt->executeQuery());

            if (result->next()) {
                int clicks = result->getInt("clicks");
                ReadOperationconnectionPool.releaseConnection(std::move(conn));
                return clicks;
            } else {
                ReadOperationconnectionPool.releaseConnection(std::move(conn));
                return -1;
            }

        } catch (sql::SQLException& e) {
            return -1;
        }
    });
}





bool isValidUrl(const std::string& url) {
    const std::regex pattern(R"(^(https?|ftp)://[^\s/$.?#].[^\s]*$)");
    return std::regex_match(url, pattern);
}



int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/shorten").methods("POST"_method)([](const crow::request& req, crow::response& res) {
        auto body = crow::json::load(req.body);
        
        std::string originalUrl = body["url"].s();
        if(!body || !isValidUrl(originalUrl)) {
            res.code = 400;
            res.write("Invalid URL");
            res.end();
            return;
        }
        auto futureResult = asyncShortenURL(originalUrl);
        std::string result = futureResult.get();

        res.set_header("Content-Type", "text/plain");
        res.write(result);
        res.end();
    });


    CROW_ROUTE(app, "/<string>")([](const crow::request& req, crow::response& res, std::string shortKey) {
        try {
            
            auto result = asyncOriginalUrl(shortKey).get();
            std::cout<< result <<std::endl;
            if (result != "URL not found") {
                res.code = 301;
                res.set_header("Location", result);
            } else {
                res.code = 404;
                res.write("URL not found");
            }

        } catch (sql::SQLException& e) {
            res.code = 500;
            res.write("Error: " + std::string(e.what()));
        }
        res.end();
    });


    CROW_ROUTE(app, "/stats/<string>")([](const crow::request& req, crow::response& res, std::string shortKey) {
        try {
            
            auto result = asyncStats(shortKey).get();
            if (result != -1) {
                res.code = 301;
                res.write(std::to_string(result));
            } else {
                res.code = 404;
                res.write("URL not found");
            }

        } catch (sql::SQLException& e) {
            res.code = 500;
            res.write("Error: " + std::string(e.what()));
        }
        res.end();
    });

    std::cout << "Starting server on port 8080...\n";
    app.port(8080).multithreaded().run();
    return 0;
}
