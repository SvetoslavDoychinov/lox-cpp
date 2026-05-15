//
// Created by svetoslav on 26.4.2026 г..
//

#include "lox_function.h"

#include "interpreter.h"

object LoxFunction::call(Interpreter *interpreter, const std::vector<object> arguments) {
    // Functions should be able to access enclosing scopes. Otherwise, you can't call another function inside ever as functions are stored in the environment
    // It's also expected from most languages for functions to have access to all enclosing scopes
    const auto environment = std::make_shared<Environment>(this->closure);
    for (int i = 0; i < this->declaration->params.size(); i++) {
        environment->define(this->declaration->params.at(i).get_lexeme(), arguments.at(i));
    }

    const auto block_stmt = std::make_shared<Block>(this->declaration->body); // Should delete at call() scope end
    try {
        /* Arguments are declared in the same scope as the body scope? Currently using the same one. As I am passing it here. It's same in C++
        This fails with variable already declared because we define all our arguments in the same environment check line 14

        fun scope(a) {
            var a = "local";
        }

         */
        interpreter->execute_block(block_stmt.get(), environment);
    }
    catch(const ReturnError& e) {
        return e.get_return_value();
    }

    return nullptr;
}

int LoxFunction::arity() {
    return this->declaration->params.size();
}

std::string LoxFunction::to_string() {
    return "<fn " + this->declaration->name.get_lexeme() + ">";
}
