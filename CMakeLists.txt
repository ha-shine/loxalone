cmake_minimum_required(VERSION 3.24)
project(loxalone)

set(CMAKE_CXX_STANDARD 20)

find_package(fmt CONFIG REQUIRED)

add_executable(loxalone src/cli/loxalone.cpp src/scanner.cpp src/scanner.h src/tokens.h src/errors.h src/expr.cpp src/expr.h)
target_link_libraries(loxalone PRIVATE fmt::fmt)

add_executable(generate_ast src/cli/generate_ast.cpp src/misc.cpp src/misc.h)
target_link_libraries(generate_ast PRIVATE fmt::fmt)
