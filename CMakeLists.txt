cmake_minimum_required(VERSION 3.24)
project(loxalone)

set(CMAKE_CXX_STANDARD 20)

find_package(fmt CONFIG REQUIRED)

set(sources
        src/Scanner.cpp
        src/Scanner.h
        src/Token.h
        src/Error.h
        src/Expr.cpp
        src/Stmt.h
        src/Expr.h
        src/Parser.cpp
        src/Parser.h
        src/Interpreter.h
        src/Interpreter.cpp src/LoxCallable.cpp src/LoxCallable.h)

add_executable(loxalone src/cli/loxalone.cpp src/PrettyPrinter.cpp src/PrettyPrinter.h ${sources} src/Environment.cpp src/Environment.h src/LiteralFormatter.h src/Resolver.cpp src/Resolver.h)
target_link_libraries(loxalone PRIVATE fmt::fmt)

add_executable(pretty_print src/cli/pretty_print.cpp src/PrettyPrinter.cpp src/PrettyPrinter.h ${sources})
target_link_libraries(pretty_print PRIVATE fmt::fmt)

add_executable(generate_ast src/cli/generate_ast.cpp src/Misc.cpp src/Misc.h)
target_link_libraries(generate_ast PRIVATE fmt::fmt)
