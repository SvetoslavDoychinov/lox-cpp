//
// Created by svetoslav on 18.5.2026 г..
//

#include "lox_class.h"


std::string LoxClass::to_string() {
    return this->name;
}

int LoxClass::arity() {
    return 0;
}

object LoxClass::call(Interpreter *interpreter, std::vector<object> arguments) {
    return nullptr;
}
