#include "utils.hpp"



std::optional<std::string> getEnv(const std::string& envKey){
    const char * val = std::getenv(envKey.c_str());
    return val ? std::optional<std::string>(val) : std::nullopt;
}



