#ifndef USERDAO_H
#define USERDAO_H

#include "shortURL.hpp"
#include <optional>
#include <string>

class UserDAO{
   
    virtual void insert(const ShortURL& shortURL) = 0;
    virtual std::optional<ShortURL> readOriginalURL(const std::string& shortURL) = 0;
    virtual void incrementCounter(const ShortURL& shortURL) = 0;
    virtual std::optional<ShortURL> readCounter(const std::string& shortURL)=0;
};

#endif