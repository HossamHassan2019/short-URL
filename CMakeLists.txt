cmake_minimum_required(VERSION 3.10)
project(MySQLCppConnectorExample LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the paths manually
add_subdirectory(external/crow)

set(MYSQL_CPP_INCLUDE_DIR "/usr/include/cppconn")
set(MYSQL_CPP_LIB "/usr/lib/x86_64-linux-gnu/libmysqlcppconn.so")

# Show the paths for debug
message(STATUS "Using MySQL Connector/C++ headers from: ${MYSQL_CPP_INCLUDE_DIR}")
message(STATUS "Using MySQL Connector/C++ library: ${MYSQL_CPP_LIB}")

# Add your source file here
add_executable(shortenUrl src/main.cpp
                          src/MySQLConnectionPool.cpp
                          src/MySQLConnectionPool.hpp
                          src/ThreadPool.cpp
                          src/ThreadPool.hpp) 

# Include cppconn headers
include_directories(external/crow/include)
target_include_directories(shortenUrl PRIVATE ${MYSQL_CPP_INCLUDE_DIR})

# Link the MySQL C++ connector library
target_link_libraries(shortenUrl PRIVATE ${MYSQL_CPP_LIB})
