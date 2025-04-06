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
#include "MysqlUser.hpp"


class MySQLConnectionPool {
    private:
        std::queue<std::unique_ptr<MySqlUser>> pool;
        std::mutex poolMutex;
        std::condition_variable condition;
        int maxConnections;
        
    
    public:
        MySQLConnectionPool(int );
        std::unique_ptr<MySqlUser> getConnection();
        void releaseConnection(std::unique_ptr<MySqlUser>);
    };


#endif 