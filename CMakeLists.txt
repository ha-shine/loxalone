cmake_minimum_required(VERSION 3.24)
project(loxalone)

set(CMAKE_CXX_STANDARD 20)

find_package(fmt CONFIG REQUIRED)

# TODO: It would be great to have the list of source files in a variable
add_executable(loxalone src/cli/loxalone.cpp src/scanner.cpp src/scanner.h src/tokens.h src/errors.h src/expr.cpp src/expr.h src/PrettyPrinter.cpp src/PrettyPrinter.h)
target_link_libraries(loxalone PRIVATE fmt::fmt)

add_executable(pretty_print src/cli/pretty_print.cpp src/scanner.cpp src/scanner.h src/tokens.h src/errors.h src/expr.cpp src/expr.h src/PrettyPrinter.cpp src/PrettyPrinter.h)
target_link_libraries(pretty_print PRIVATE fmt::fmt)

add_executable(generate_ast src/cli/generate_ast.cpp src/misc.cpp src/misc.h)
target_link_libraries(generate_ast PRIVATE fmt::fmt)
