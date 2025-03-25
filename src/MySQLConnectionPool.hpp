#ifndef MYSQLCONNECTIONPOOL
#define MYSQLCONNECTIONPOOL

#include <iostream>
#include <string>
#include <random>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <mysql_driver.h>
#include <mysql_connection.h>



class MySQLConnectionPool {
    private:
        std::queue<std::unique_ptr<sql::Connection>> pool;
        std::mutex poolMutex;
        std::condition_variable condition;
        int maxConnections;
        sql::mysql::MySQL_Driver* driver;
    
    public:
        MySQLConnectionPool(int );
        std::unique_ptr<sql::Connection> getConnection();
        void releaseConnection(std::unique_ptr<sql::Connection>);
    };


#endif 