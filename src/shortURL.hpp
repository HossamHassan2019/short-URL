#ifndef SHORTURL_H
#define SHORTURL_H

#include <string>


class ShortURL{
   private: 
     std::string shortURL_;
     std::string original_URL_;
     int counter_;

   public:
     ShortURL( std::string shortURL ,  std::string originaURL = " ", int counter = 0 ):shortURL_{shortURL} , original_URL_{originaURL} , counter_{counter}{};
     int getCounter() const{return counter_;}  
     void setCounter(const int& counter){counter_ = counter;}
     std::string getShortURL() const{return shortURL_;}
     void setShortURL(const std::string& shortURL){shortURL_ = shortURL;}
     std::string getOriginalURL() const {return original_URL_;}
     void setOriginalURL(const std::string& originalURL ){original_URL_ = originalURL;}
     
};
#endif