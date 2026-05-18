cmake_minimum_required(VERSION 4.1)
project(lox-cpp)

set(CMAKE_CXX_STANDARD 20)

# Create executable
add_executable(lox-cpp
        lox.cpp
        main.cpp
        token_type.h
        token.cpp
        scanner.cpp
        expr.h
        ast_printer.cpp
        parser.cpp
        interpreter.cpp
        stmt.h
        environment.cpp
        type_checker.cpp
        lox_callable.h
        builtins.cpp
        lox_function.cpp
        resolver.cpp
        lox_class.cpp
)
