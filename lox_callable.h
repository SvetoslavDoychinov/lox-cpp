//
// Created by svetoslav on 20.4.2026 г..
//

#ifndef JLOX_LOX_CALLABLE_H
#define JLOX_LOX_CALLABLE_H
#include <memory>
#include <string>
#include <variant>
#include <vector>


class Interpreter;
class LoxCallable;
// No need to "#include interpreter.h" here because we include this and environment.h in the interpreter file only
// Including will create a circular dependency that is unsolvable
// We don't know the size of Interpreter here, but it's not necessary probably because we include this file only in the Interpreter file and the linking phase solves this?


using object = std::variant<double, std::string, bool, std::nullptr_t, std::shared_ptr<LoxCallable>>;

class LoxCallable {
    public:
    LoxCallable() = default;
    virtual ~LoxCallable() = default;

    virtual object call(Interpreter *interpreter, std::vector<object> arguments) = 0;
    virtual int arity() = 0;
    virtual std::string to_string() = 0;
};


#endif //JLOX_LOX_CALLABLE_H