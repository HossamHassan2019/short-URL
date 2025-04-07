#include "utils.hpp"



std::optional<std::string> getEnv(const std::string& envKey){
    const char * val = std::getenv(envKey.c_str());
    return val ? std::optional<std::string>(val) : std::nullopt;
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


