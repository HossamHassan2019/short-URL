#ifndef MYSQL_H
#define MYSQL_H

#include <string>
#include "UserDAO.hpp"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "utils.hpp"


class MySqlUser:UserDAO{
    private: 
      std::string userName_;
      std::string password_;
      std::string url_;
      std::string schema_;
      sql::Driver* driver_;
      std::unique_ptr<sql::Connection> connection_;
    public:
      MySqlUser();
      void insert(const ShortURL& shortURL) override;
      std::optional<ShortURL> readOriginalURL(const std::string& shortURL) override;
      void incrementCounter(const ShortURL& shortURL) override;
      std::optional<ShortURL> readCounter(const std::string& shortURL) override;
      void checkConnection();
};




#endif