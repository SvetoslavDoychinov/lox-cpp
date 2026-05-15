//
// Created by svetoslav on 11.4.2026 г..
//

#ifndef JLOX_ENVIRONMENT_H
#define JLOX_ENVIRONMENT_H
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <variant>

#include "lox_callable.h"
#include "token.h"


class Environment {
    private:
    std::unordered_map<std::string, object> values;
    std::shared_ptr<Environment> enclosing;
    std::set<std::string> unassigned;

    void remove_from_unassigned(const std::string &name);

    public:
    Environment() = default;
    explicit Environment(std::shared_ptr<Environment> enclosing) : enclosing(std::move(enclosing)) {}
    ~Environment() = default;

    void define(const std::string &name, const object &value);
    object get_value(const Token &name);
    void assign(const Token &name, const object &value);
    bool contains(const Token &name) const;
    void add_to_unassigned(const std::string &name);
    bool is_unassigned(const std::string &name) const;
    object get_at(int distance, const std::string &name);
    bool unassigned_at(int distance, const std::string &name);
    void assign_at(int distance, const Token &name, const object &value);
    Environment* ancestor(int distance);

};


#endif //JLOX_ENVIRONMENT_H