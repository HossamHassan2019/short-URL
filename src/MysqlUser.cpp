#include "MysqlUser.hpp"


MySqlUser::MySqlUser(){
    if(getEnv("DB_USER")) {
        userName_ = *getEnv("DB_USER");
    }else{
        std::cerr << "DB_USER not set in environment variables" << std::endl;
        exit(1);
    }

    if(getEnv("DB_PASS")) {
        password_ = *getEnv("DB_PASS");
    }else{
        std::cerr << "DB_PASS not set in environment variables" << std::endl;
        exit(1);
    }
    if(getEnv("DB_URL")) {
        url_ = *getEnv("DB_URL");
    }else{
        std::cerr << "DB_URL not set in environment variables" << std::endl;
        exit(1);
    }
    if(getEnv("DB_NAME")) {
        schema_ = *getEnv("DB_NAME");
    }else{  
        std::cerr << "DB_NAME not set in environment variables" << std::endl;
        exit(1);
    }
    // Initialize the MySQL driver and connection
    driver_ = sql::mysql::get_mysql_driver_instance();
    connection_ = std::unique_ptr<sql::Connection>(driver_->connect(url_, userName_, password_));
    bool reconnect = true;
    connection_->setClientOption("OPT_RECONNECT", &reconnect);
    connection_->setSchema(schema_);
}


void MySqlUser::insert(const ShortURL& shortURL) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(connection_->prepareStatement(
            "INSERT INTO urls (original_url, short_key) VALUES (?, ?)"
        ));
        pstmt->setString(1, shortURL.getOriginalURL());
        pstmt->setString(2, shortURL.getShortURL());
        pstmt->executeUpdate();
    } catch (sql::SQLException& e) {
        std::cerr << "Error inserting URL: " << e.what() << std::endl;
    }
}

std::optional<ShortURL> MySqlUser::readOriginalURL(const std::string& shortURL) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(connection_->prepareStatement(
            "SELECT original_url FROM urls WHERE short_key = ?"
        ));
        pstmt->setString(1, shortURL);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        if (res->next()) {
            return ShortURL(shortURL, res->getString("original_url"));
        } else {
            return std::nullopt;
        }
    } catch (sql::SQLException& e) {
        std::cerr << "Error reading URL: " << e.what() << std::endl;
        return std::nullopt;
    }
}

void MySqlUser::incrementCounter(const ShortURL& shortURL) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(connection_->prepareStatement(
            "UPDATE urls SET clicks = clicks + 1 WHERE short_key = ?"
        ));
        pstmt->setString(1, shortURL.getShortURL());
        pstmt->executeUpdate();
    } catch (sql::SQLException& e) {
        std::cerr << "Error incrementing counter: " << e.what() << std::endl;
    }
}



std::optional<ShortURL> MySqlUser::readCounter(const std::string& shortURL) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(connection_->prepareStatement(
            "SELECT clicks FROM urls WHERE short_key = ?"
        ));
        pstmt->setString(1, shortURL);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        
        if (res->next()) {
            ShortURL shortUrlObj(shortURL , " ",res->getInt("clicks"));
            return shortUrlObj;
        } else {
            return std::nullopt;
        }
    } catch (sql::SQLException& e) {
        std::cerr << "Error reading counter: " << e.what() << std::endl;
        return std::nullopt;
    }
}


void MySqlUser::checkConnection(){
    if (!connection_->isValid()) {
        connection_.reset(driver_->connect(url_, userName_, password_));
        connection_->setSchema(schema_);
    }
}