// Final: Crow API + Async MySQL Connector/C++ (modern C++ style)

#include "crow.h"
#include "MySQLConnectionPool.hpp"
#include "ThreadPool.hpp"
#include "shortURL.hpp"
#include <iostream>
#include <string>
#include <random>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <regex>







MySQLConnectionPool ReadOperationconnectionPool(10);
MySQLConnectionPool WriteOperationconnectionPool(10);
ThreadPool threadPool(10);


/**
 * @brief Generates a random alphanumeric short key of given length.
 * 
 * @param length Length of the short key (default = 6)
 * @return A random short key string.
 */
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



/**
 * @brief Asynchronously inserts a new URL into the database and returns a short key.
 * 
 * @param originalUrl The original long URL to be shortened.
 * @return A future holding the generated short key or an error message.
 */
std::future<std::string> asyncShortenURL(const std::string& originalUrl) {
    return std::async(std::launch::async, [originalUrl]() {
   
            std::string shortKey = generateShortKey();
            ShortURL shortUrl(shortKey, originalUrl);
            auto user = WriteOperationconnectionPool.getConnection();
            user->insert(shortUrl);
            
            WriteOperationconnectionPool.releaseConnection(std::move(user));

            return  shortKey;

       
    });
}





/**
 * @brief Asynchronously retrieves the original URL from a short key.
 *        Also enqueues a task to increment the click count.
 * 
 * @param shortUrl The short key.
 * @return A future holding the original URL or "URL not found"/error message.
 */
std::future<std::string> asyncOriginalUrl(const std::string& shortUrl) {
    return std::async(std::launch::async, [shortUrl]() {
       
            auto readUser = ReadOperationconnectionPool.getConnection();
            auto shortURLObj =readUser->readOriginalURL(shortUrl);
            if(shortURLObj){
                ReadOperationconnectionPool.releaseConnection(std::move(readUser));
                threadPool.enqueue([shortURLObj](){
                    auto writeUser = WriteOperationconnectionPool.getConnection();
                    writeUser->incrementCounter(shortURLObj.value());
                    WriteOperationconnectionPool.releaseConnection(std::move(writeUser));
                });
                return shortURLObj->getOriginalURL();

            }else{
                ReadOperationconnectionPool.releaseConnection(std::move(readUser));
                return std::string("URL not found");
            }

            
    });
}

/**
 * @brief Asynchronously retrieves click statistics for a given short URL.
 * 
 * @param shortUrl The short key.
 * @return A future holding the number of clicks or -1 on error/not found.
 */
std::future<int> asyncStats(const std::string& shortUrl) {
    return std::async(std::launch::async, [shortUrl]() {
        
            auto readUser = ReadOperationconnectionPool.getConnection();
            auto shortUrlObj = readUser->readCounter(shortUrl);
            if(shortUrlObj){
                ReadOperationconnectionPool.releaseConnection(std::move(readUser));
                return shortUrlObj->getCounter();
            }
            else{
                ReadOperationconnectionPool.releaseConnection(std::move(readUser));
                return -1;

            }
            
    });
}




/**
 * @brief Validates a URL string using regex.
 * 
 * @param url The input URL to validate.
 * @return true if valid, false otherwise.
 */
bool isValidUrl(const std::string& url) {
    const std::regex pattern(R"(^(https?|ftp)://[^\s/$.?#].[^\s]*$)");
    return std::regex_match(url, pattern);
}



int main() {
    crow::SimpleApp app;
    // POST /shorten: Accepts a JSON body and returns a shortened URL key
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

    // GET /<shortKey>: Redirects to the original URL or returns 404 if not found
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

    // GET /stats/<shortKey>: Returns click stats for a short URL
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
