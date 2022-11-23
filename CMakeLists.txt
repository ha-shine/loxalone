cmake_minimum_required(VERSION 3.24)
project(loxalone)

set(CMAKE_CXX_STANDARD 20)

find_package(fmt CONFIG REQUIRED)

add_executable(loxalone src/loxalone.cpp src/scanner.cpp src/scanner.h src/tokens.h src/errors.h)
target_link_libraries(loxalone PRIVATE fmt::fmt)
