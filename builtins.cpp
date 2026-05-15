//
// Created by svetoslav on 25.4.2026 г..
//

#include "builtins.h"

#include <chrono>
#include <iostream>


object ClockFunction::call(Interpreter *interpreter, std::vector<object> arguments) {
    const auto p1 = std::chrono::system_clock::now();
    const auto seconds = std::chrono::duration_cast<std::chrono::milliseconds>(
        p1.time_since_epoch()
        ).count();
    return static_cast<double>(seconds);
}

int ClockFunction::arity() {
    return 0;
}

std::string ClockFunction::to_string() {
    return "<native fn>";
}

object PrintFunction::call(Interpreter *interpreter, const std::vector<object> arguments) {
    std::cout << this->stringify(arguments.at(0)) << std::endl;
    return nullptr;
}

int PrintFunction::arity() {
    return 1;
}

std::string PrintFunction::to_string() {
    return "<native fn>";
}

std::string PrintFunction::stringify(const object &object) {
    if (std::holds_alternative<bool>(object)) {
        if (std::get<bool>(object)) return "true";
        return "false";
    }
    if (std::holds_alternative<std::string>(object)) {
        return std::get<std::string>(object);
    }
    if (std::holds_alternative<double>(object)) {
        return this->remove_trailing_zeros(std::to_string(std::get<double>(object)));
    }
    if (std::holds_alternative<std::shared_ptr<LoxCallable>>(object)) {
        return std::get<std::shared_ptr<LoxCallable>>(object)->to_string();
    }
    return "nil";
}

std::string PrintFunction::remove_trailing_zeros(std::string number) {
    if (number.find('.') != std::string::npos) {
        while (!number.empty() && number.at(number.length() - 1) == '0') {
            number.pop_back();
        }
        if (!number.empty() && number.at(number.length() - 1) == '.') number.pop_back();
    }
    return number;
}
