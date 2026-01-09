cmake_minimum_required(VERSION 3.10)
project(todo_cli_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Handle Homebrew MySQL client on macOS
if(APPLE)
  set(MYSQL_PREFIX "/opt/homebrew/opt/mysql-client" CACHE PATH "MySQL client prefix")
  find_path(MYSQL_INCLUDE_DIR mysql/mysql.h PATHS "${MYSQL_PREFIX}/include" NO_DEFAULT_PATH)
  find_library(MYSQLCLIENT_LIB mysqlclient PATHS "${MYSQL_PREFIX}/lib" NO_DEFAULT_PATH)
else()
  find_path(MYSQL_INCLUDE_DIR NAMES mysql/mysql.h)
  find_library(MYSQLCLIENT_LIB NAMES mysqlclient)
endif()

if(NOT MYSQL_INCLUDE_DIR)
  message(FATAL_ERROR "Could not find mysql headers. Install MySQL client development files.")
endif()
if(NOT MYSQLCLIENT_LIB)
  message(FATAL_ERROR "Could not find mysqlclient library. Install MySQL client development files.")
endif()

include_directories(${MYSQL_INCLUDE_DIR})

add_executable(todo src/main.cpp)
target_include_directories(todo PRIVATE ${MYSQL_INCLUDE_DIR})
target_link_libraries(todo PRIVATE ${MYSQLCLIENT_LIB})
