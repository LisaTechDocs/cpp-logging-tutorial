cmake_minimum_required(VERSION 3.16)

project(cpp_logging_tutorial LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(spdlog REQUIRED)

add_executable(logging_example src/main.cpp)

target_link_libraries(logging_example PRIVATE spdlog::spdlog)
