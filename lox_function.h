//
// Created by svetoslav on 26.4.2026 г..
//

#ifndef JLOX_LOX_FUNCTION_H
#define JLOX_LOX_FUNCTION_H
#include "environment.h"
#include "lox_callable.h"
#include "stmt.h"




class LoxFunction: public LoxCallable {
    private:
    // IMPORTANT This needs to be a raw pointer, because I am letting only the shared_ptrs in parsed_results in lox.cpp manage the lifetime of this pointer
    // Check comment in visit_function_stmt in interpreter.cpp for more info
    const Function *declaration;
    std::shared_ptr<Environment> closure;

    public:
    explicit LoxFunction(const Function* declaration, std::shared_ptr<Environment> closure) : declaration(declaration), closure(std::move(closure)) {};

    object call(Interpreter *interpreter, std::vector<object> arguments) override;
    int arity() override;
    std::string to_string() override;
};


#endif //JLOX_LOX_FUNCTION_H