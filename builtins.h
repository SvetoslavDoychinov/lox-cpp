//
// Created by svetoslav on 25.4.2026 г..
//

#ifndef JLOX_BUILTINS_H
#define JLOX_BUILTINS_H
#include "lox_callable.h"


class ClockFunction: public LoxCallable {
    public:
    int arity() override;
    object call(Interpreter *interpreter, std::vector<object> arguments) override;
    std::string to_string() override;
};


class PrintFunction: public LoxCallable {
    public:
    int arity() override;
    object call(Interpreter *interpreter, std::vector<object> arguments) override;
    std::string to_string() override;

    private:
    std::string stringify(const object &object);
    std::string remove_trailing_zeros(std::string number);
};


#endif //JLOX_BUILTINS_H