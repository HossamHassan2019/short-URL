#ifndef CACHE_HPP
#define CACHE_HPP

#include <string>
#include <shared_mutex>
#include <mutex>
#include <unordered_map>
#include <optional>
#include <iostream>
#include "shortURL.hpp"

class Cache{
   private: 
     mutable std::shared_mutex sharedMutex_;
     std::unordered_map<std::string,std::pair<std::string,int>> inMemoryCache_;
     Cache() = default;
   public:
     Cache(Cache& c) = delete ;
     void operator=(const Cache&) = delete;
     static Cache& getInstance();
     void insert(const ShortURL& shortURL);
     void increment(const std::string& shortURL);
     std::optional<ShortURL> readOriginalURL(const std::string& shortURL) ;
     void sync();

};



#endif