// Final: Crow API + Async MySQL Connector/C++ (modern C++ style)

#include "crow.h"
#include "MySQLConnectionPool.hpp"
#include "ThreadPool.hpp"
#include "shortURL.hpp"
#include "cache.hpp"
#include "utils.hpp"
#include <iostream>
#include <string>
#include <random>
#include <queue>
#include <mutex>
#include <condition_variable>



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
 * @brief Generates a short key, saves the URL mapping to DB and cache.
 * 
 * @param originalUrl The original long URL.
 * @return The generated short key string.
 */
std::string writeUrl(const std::string& originalUrl){
    std::string shortKey = generateShortKey();
    Cache& cache = Cache::getInstance();
    
    ShortURL shortUrl(shortKey, originalUrl);
    auto user = WriteOperationconnectionPool.getConnection();
    user->insert(shortUrl);
    
    WriteOperationconnectionPool.releaseConnection(std::move(user));
    cache.insert(ShortURL(shortKey,originalUrl));

    return  shortKey;

}





/**
 * @brief Fetches the original URL using the short key.
 *        First checks the cache, then DB if needed.
 *        Also asynchronously increments the click counter.
 * 
 * @param shortUrl The short key string.
 * @return Optional original URL. std::nullopt if not found.
 */
std::optional<std::string> readUrl(const std::string& shortUrl) {
   
            Cache& cache = Cache::getInstance();
            auto val = cache.readOriginalURL(shortUrl);
            if(val) {
                threadPool.enqueue([val](){
                    auto writeUser = WriteOperationconnectionPool.getConnection();
                    auto shortURlObj = ShortURL(val.value().getShortURL(), val.value().getOriginalURL());
                    writeUser->incrementCounter(shortURlObj);
                    WriteOperationconnectionPool.releaseConnection(std::move(writeUser));
                });
                cache.increment(shortUrl);
                return val.value().getOriginalURL();
            }
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
                    return std::nullopt;
                }
}




/**
 * @brief Gets the click count for a short URL.
 * 
 * @param shortUrl The short key.
 * @return Click count if found, otherwise -1.
 */
int getStats(const std::string& shortUrl){
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
        std::string result  = writeUrl(originalUrl);
        res.set_header("Content-Type", "text/plain");
        res.write(result);
        res.end();
    });

    // GET /<shortKey>: Redirects to the original URL or returns 404 if not found
    CROW_ROUTE(app, "/<string>")([](const crow::request& req, crow::response& res, std::string shortKey) {
        
            auto result = readUrl(shortKey);
            if (result ) {
                std::cout<<result.value()<<std::endl;
                res.code = 301;
                res.set_header("Location", result.value());
            } else {
                res.code = 404;
                res.write("URL not found"); 
            }
            res.end();

    });

    // GET /stats/<shortKey>: Returns click stats for a short URL
    CROW_ROUTE(app, "/stats/<string>")([](const crow::request& req, crow::response& res, std::string shortKey) {
        try {
            
            auto result = getStats(shortKey);
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
