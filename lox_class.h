//
// Created by svetoslav on 18.5.2026 г..
//

#ifndef LOX_CPP_LOX_CLASS_H
#define LOX_CPP_LOX_CLASS_H
#include <string>

#include "lox_callable.h"


class LoxClass: public LoxCallable {
    public:
    explicit LoxClass(std::string name) : name(std::move(name)) {};

    std::string to_string() override;
    int arity() override;
    object call(Interpreter *interpreter, std::vector<object> arguments) override;

    private:
    std::string name;
};


#endif //LOX_CPP_LOX_CLASS_H