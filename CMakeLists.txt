cmake_minimum_required(VERSION 3.24)
project(loxalone)

set(CMAKE_CXX_STANDARD 20)

find_package(fmt CONFIG REQUIRED)

set(sources
        src/interpreter/Scanner.cpp
        src/interpreter/Scanner.h
        src/interpreter/Token.h
        src/interpreter/Error.h
        src/interpreter/Stmt.h
        src/interpreter/Expr.h
        src/interpreter/Parser.cpp
        src/interpreter/Parser.h
        src/interpreter/Interpreter.h
        src/interpreter/Interpreter.cpp
        src/interpreter/LoxCallable.cpp
        src/interpreter/LoxCallable.h
        src/interpreter/Environment.cpp
        src/interpreter/Environment.h
        src/interpreter/LiteralFormatter.h
        src/interpreter/Resolver.cpp
        src/interpreter/Resolver.h
        src/interpreter/LoxClass.cpp
        src/interpreter/LoxClass.h src/interpreter/LoxInstance.cpp src/interpreter/LoxInstance.h)

add_executable(loxalone src/cli/loxalone.cpp ${sources})
target_link_libraries(loxalone PRIVATE fmt::fmt)

add_executable(pretty_print src/cli/pretty_print.cpp src/interpreter/PrettyPrinter.cpp src/interpreter/PrettyPrinter.h ${sources})
target_link_libraries(pretty_print PRIVATE fmt::fmt)

add_executable(generate_ast src/cli/generate_ast.cpp src/interpreter/Misc.cpp src/interpreter/Misc.h)
target_link_libraries(generate_ast PRIVATE fmt::fmt)
