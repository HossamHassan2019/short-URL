#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <optional>
#include <cstdlib>
#include <regex>


std::optional<std::string> getEnv(const std::string& envKey);
bool isValidUrl(const std::string& url);


#endif