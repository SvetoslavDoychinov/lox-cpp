//
// Created by svetoslav on 11.4.2026 г..
//

#include "environment.h"

#include "interpreter.h"


void Environment::define(const std::string &name, const object &value) {
    this->values.insert({name, value});
}

object Environment::get_value(const Token &name) {
    if (this->values.contains(name.get_lexeme())) {
        return this->values.at(name.get_lexeme());
    }
    if (this->enclosing != nullptr) {
        return this->enclosing->get_value(name);
    }
    // TODO Should never be hitting this after adding logic to resolver. Should I remove? Along with similar errors here.
    throw RuntimeError(name, "Undeclared variable " + name.get_lexeme() + ".");
}

void Environment::assign(const Token &name, const object &value) {
    if (this->values.contains(name.get_lexeme())) {
        this->values[name.get_lexeme()] = value;
        // So a variable is assigned for all enclosing scopes when it's assigned
        this->remove_from_unassigned(name.get_lexeme());
        return;
    }
    if (this->enclosing != nullptr) {
        this->enclosing->assign(name, value);
        return;
    }
    throw RuntimeError(name, "Undeclared variable '" + name.get_lexeme() + "'.");
}

bool Environment::contains(const Token &name) const {
    if (this->values.contains(name.get_lexeme())) {
        return true;
    }
    return false;
}

void Environment::add_to_unassigned(const std::string &name) {
    this->unassigned.insert(name);
}

void Environment::remove_from_unassigned(const std::string &name) {
    this->unassigned.erase(name);
}

bool Environment::is_unassigned(const std::string &name) const {
    return this->unassigned.contains(name);
}

object Environment::get_at(const int distance, const std::string &name) {
    auto target_ancestor = this->ancestor(distance);
    auto dummy_token = Token(IDENTIFIER, name, 0, nullptr);
    if (target_ancestor->contains(dummy_token)) {
        return target_ancestor->get_value(dummy_token);
    }
    // Should never be able to reach here, but better not expose implementation just in case.
    throw RuntimeError(dummy_token,"Variable `" + dummy_token.get_lexeme() + "` is undeclared in target scope.");
}

bool Environment::unassigned_at(const int distance, const std::string &name) {
    return this->ancestor(distance)->is_unassigned(name);
}

void Environment::assign_at(const int distance, const Token &name, const object &value) {
    auto target_ancestor = this->ancestor(distance);
    if (target_ancestor->contains(name)) {
        target_ancestor->assign(name, value);
    }
    else {
        throw RuntimeError(name,"Variable `" + name.get_lexeme() + "` is undeclared in target scope.");
    }
}

Environment *Environment::ancestor(const int distance) {
    auto environment = this;
    for (int i = 0; i < distance; i++) {
        environment = environment->enclosing.get();
    }
    return environment;
}
