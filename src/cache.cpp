#include "cache.hpp"



Cache& Cache::getInstance(){
    static Cache cacheInstance;
    return cacheInstance;
}




void Cache::increment(const std::string& shortURL){
    std::unique_lock<std::shared_mutex> lock(sharedMutex_);
    auto it = inMemoryCache_.find(shortURL);
    it->second.second++;
    //std::cout<<it->second.second<<std::endl;
    
}


void Cache::insert(const ShortURL& shortURL){
    std::unique_lock<std::shared_mutex> lock(sharedMutex_);
    inMemoryCache_[shortURL.getShortURL()] = std::make_pair(shortURL.getOriginalURL() , 0);
}


std::optional<ShortURL> Cache::readOriginalURL(const std::string& shortURL) {
    std::shared_lock lock(sharedMutex_);
    auto it = inMemoryCache_.find(shortURL);
    if (it != inMemoryCache_.end()) {
        return ShortURL(shortURL, it->second.first);
    }
    return std::nullopt;
}

