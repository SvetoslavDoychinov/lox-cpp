//
// Created by svetoslav on 8.4.2026 г..
//

#include "interpreter.h"

#include <utility>

#include "builtins.h"
#include "lox.h"
#include "lox_class.h"
#include "lox_function.h"

Interpreter::Interpreter() {
    this->globals->define("clock", std::make_shared<ClockFunction>());
    this->globals->define("print", std::make_shared<PrintFunction>());
}

void Interpreter::interpret(const std::vector<std::shared_ptr<Stmt>>& statements) {
    try {
        for (const auto& stmt : statements) {
            this->execute(stmt);
        }
    }
    catch (const RuntimeError &e) {
        auto lox = Lox();
        lox.runtime_error(e);
    }
}

void Interpreter::visit_variable_expr(const Variable *expr) {
    this->result = this->look_up_variable(expr->name, expr);
}

void Interpreter::visit_literal_expr(const Literal *expr) {
    this->result = this->convert_to_object(expr->value);
}

void Interpreter::visit_grouping_expr(const Grouping *expr) {
    this->evaluate(expr->expression);
}

void Interpreter::visit_call_expr(const Call *expr) {
    this->evaluate(expr->callee);
    const auto callee = this->result;

    std::vector<object> arguments;
    for (const auto& argument: expr->arguments) {
        this->evaluate(argument);
        arguments.push_back(this->result);
    }
    if (!std::holds_alternative<std::shared_ptr<LoxCallable>>(callee)) {
        throw RuntimeError(expr->paren, "Can only call functions and classes.");
    }
    const auto function = std::get<std::shared_ptr<LoxCallable>>(callee);
    if (arguments.size() != function->arity()) {
        throw RuntimeError(
            expr->paren,
            "Expected " + std::to_string(function->arity()) + " arguments but got " + std::to_string(arguments.size()) + "."
            );
    }
    this->result = function->call(this, arguments);
}

void Interpreter::visit_unary_expr(const Unary *expr) {
    this->evaluate(expr->right);

    switch (expr->oprtr.get_type()) {
        case MINUS:
            this->check_number_operand(expr->oprtr, this->result);
            this->result = -std::get<double>(this->result);
        case BANG:
            this->result = !this->is_truthy(this->result);
        default:;
    }
}

void Interpreter::visit_binary_expr(const Binary *expr) {
    this->evaluate(expr->left);
    const object left = this->result;
    this->evaluate(expr->right);

    switch (expr->oprtr.get_type()) {
        case MINUS:
            this->check_number_operands(left, expr->oprtr, this->result);
            this->result = std::get<double>(left) - std::get<double>(this->result);
            break;
        case PLUS:
            if (std::holds_alternative<double>(left) && std::holds_alternative<double>(this->result)) {
                this->result = std::get<double>(left) + std::get<double>(this->result);
            }
            else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(this->result)) {
                this->result = std::get<std::string>(left) + std::get<std::string>(this->result);
            }
            else {
                throw RuntimeError(expr->oprtr, "Operands must be two numbers or two strings.");
            }
            break;
        case SLASH:
            this->check_number_operands(left, expr->oprtr, this->result);
            this->check_division_by_zero(expr->oprtr, this->result);
            this->result = std::get<double>(left) / std::get<double>(this->result);
            break;
        case STAR:
            this->check_number_operands(left, expr->oprtr, this->result);
            this->result = std::get<double>(left) * std::get<double>(this->result);
            break;
        case GREATER:
            this->check_number_operands(left, expr->oprtr, this->result);
            this->result = std::get<double>(left) > std::get<double>(this->result);
            break;
        case GREATER_EQUAL:
            this->check_number_operands(left, expr->oprtr, this->result);
            this->result = std::get<double>(left) >= std::get<double>(this->result);
            break;
        case LESS:
            this->check_number_operands(left, expr->oprtr, this->result);
            this->result = std::get<double>(left) < std::get<double>(this->result);
            break;
        case LESS_EQUAL:
            this->check_number_operands(left, expr->oprtr, this->result);
            this->result = std::get<double>(left) <= std::get<double>(this->result);
            break;
        case BANG_EQUAL:
            this->result = left != this->result;
            break;
        case EQUAL_EQUAL:
            this->result = left == this->result;
        default: break;
    }
}

void Interpreter::visit_logical_expr(const Logical *expr) {
    this->evaluate(expr->left);
    if (expr->oprtr.get_type() == OR) {
        if (this->is_truthy(this->result)) {
            return;
        }
    }
    else {
        if (!this->is_truthy(this->result)) {
            return;
        }
    }
    this->evaluate(expr->right);
}

void Interpreter::visit_assign_expr(const Assign *expr) {
    this->evaluate(expr->value);
    if (this->locals.contains(expr)) {
        int distance = this->locals.at(expr);
        this->environment->assign_at(distance, expr->name, this->result);
    }
    else {
        this->globals->assign(expr->name, this->result);
    }
}

void Interpreter::evaluate(const std::shared_ptr<Expr> &expression) {
    expression->accept(this);
}

void Interpreter::visit_block_stmt(const Block *stmt) {
    this->execute_block(stmt, std::make_shared<Environment>(this->environment));
}

void Interpreter::visit_while_stmt(const While *stmt) {
    this->evaluate(stmt->condition);

    try {
        while (this->is_truthy(this->result)) {
            try {
                this->execute(stmt->body);
            }
            catch (const ContinueError &) {}
            this->evaluate(stmt->condition);
        }
    }
    catch(const BreakError &) {}
}

void Interpreter::visit_for_stmt(const For *stmt) {
    this->evaluate(stmt->condition);

    try {
        while (this->is_truthy(this->result)) {
            try {
                this->execute(stmt->body);
            }
            catch (const ContinueError &) {}
            this->evaluate(stmt->increment);
            this->evaluate(stmt->condition);
        }
    }
    catch(const BreakError &) {}
}

void Interpreter::visit_if_stmt(const If *stmt) {
    this->evaluate(stmt->condition);
    if (this->is_truthy(this->result)) {
        this->execute(stmt->then_branch);
    }
    else if (stmt->else_branch) {
        this->execute(stmt->else_branch);
    }
}

void Interpreter::visit_var_stmt(const Var *stmt) {
    if (this->environment->contains(stmt->name)) {
        // Needs to be here. If in the define method we don't have access to the token for a good error message.
        // Also covers redeclaration in global scope which I don't want to allow
        throw RuntimeError(stmt->name, "Variable already declared in this scope.");
    }
    if (stmt->initializer) {
        // When no assignment in the declaration I store a default shared_ptr<Stmt> which has a nullptr
        // The bool operator of the shared_ptr will check whether it has a nullptr
        this->evaluate(stmt->initializer);
        this->environment->define(stmt->name.get_lexeme(), this->result);
    }
    else {
        this->environment->define(stmt->name.get_lexeme(), nullptr);
        this->environment->add_to_unassigned(stmt->name.get_lexeme());
    }
}

void Interpreter::visit_function_stmt(const Function *stmt) {
    /* IMPORTANT Pointer is not deleted here because we add it to container in environment
    But when interpreter is deleted, environment is also deleted. In parsed_results we have a shared_ptr that has the same Function pointer as the one here
    Later when parsed_results is out of scope we attempt to delete the shared_ptr but its pointer is already deleted. Causing error. This why LoxFunction takes raw Function pointer

    something similar to this happens:

    auto og = std::make_shared<int>(1);
    {
        const auto new_ptr = og.get();
        auto new_shared = std::shared_ptr<int>(new_ptr);
    }

    */

    auto function = std::make_shared<LoxFunction>(stmt, this->environment);
    this->environment->define(stmt->name.get_lexeme(), function);
}

void Interpreter::visit_class_stmt(const Class *stmt) {
    this->environment->define(stmt->name.get_lexeme(), nullptr);
    auto klass = std::make_shared<LoxClass>(stmt->name.get_lexeme());
    this->environment->assign(stmt->name, klass);
}

void Interpreter::visit_expression_stmt(const Expression *stmt) {
    this->evaluate(stmt->expression);
}

void Interpreter::visit_break_stmt(const Break *stmt) {
    throw BreakError();
}

void Interpreter::visit_continue_stmt(const Continue *stmt) {
    throw ContinueError();
}

void Interpreter::visit_return_stmt(const Return *stmt) {
    object return_value = nullptr;
    if (stmt->value) {
        this->evaluate(stmt->value);
        return_value = this->result;
    }
    throw ReturnError(return_value);
}

void Interpreter::execute_block(const Block *stmt, const std::shared_ptr<Environment>& env) {
    const std::shared_ptr<Environment> previous = this->environment;
    try {
        this->environment = env;
        for (const auto& statement : stmt->statements) {
            this->execute(statement);
        }
    }
    catch (const RuntimeError &) {
        this->environment = previous;
        throw;
    }
    catch (const BreakError &) {
        // Break statements will otherwise not return us to the enclosing environment and we will be stuck with the wrong scope
        // All loop bodies are block statements so this is where we will initially catch the exception.
        this->environment = previous;
        throw;
    }
    catch (const ContinueError &) {
        this->environment = previous;
        throw;
    }
    catch (const ReturnError &) {
        this->environment = previous;
        throw;
    }
    this->environment = previous;
}

void Interpreter::execute(const std::shared_ptr<Stmt> &stmt) {
    stmt->accept(this);
}

void Interpreter::resolve(const Expr *expr, int depth) {
    this->locals.insert({expr, depth});
}

bool Interpreter::is_truthy(const object &object) {
    if (std::holds_alternative<std::nullptr_t>(object)) {
        return false;
    }
    if (std::holds_alternative<bool>(object)) {
        return std::get<bool>(object);
    }
    return true;
}

void Interpreter::check_number_operand(const Token &oprtr, const object &right) const {
    if (std::holds_alternative<double>(right)) return;
    throw RuntimeError(oprtr, "Operand must be a number.");
}

void Interpreter::check_number_operands(const object &left, const Token &oprtr, const object &right) const {
    if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) return;
    throw RuntimeError(oprtr, "Operands must be two numbers.");
}

void Interpreter::check_division_by_zero(const Token &oprtr, const object &right) const {
    if (std::holds_alternative<double>(right) && std::get<double>(right) == 0 && oprtr.get_type() == SLASH) {
        throw RuntimeError(oprtr, "Division by zero.");
    }
}

object Interpreter::convert_to_object(const primitive &value) {
    object converted_variant = nullptr;
    if (std::holds_alternative<double>(value)) converted_variant = std::get<double>(value);
    else if (std::holds_alternative<std::string>(value)) converted_variant = std::get<std::string>(value);
    else if (std::holds_alternative<bool>(value)) converted_variant = std::get<bool>(value);

    return converted_variant;
}

object Interpreter::look_up_variable(const Token &name, const Expr *expr) const {
    if (this->locals.contains(expr)) {
        int distance = this->locals.at(expr);
        if (this->environment->unassigned_at(distance, name.get_lexeme())) {
            throw RuntimeError(name, "Variable is unassigned.");
        }
        return this->environment->get_at(distance, name.get_lexeme());
    }
    if (this->globals->is_unassigned(name.get_lexeme())) {
        // TODO Is it possible to move unassigned logic to static resolver?
        throw RuntimeError(name, "Variable is unassigned.");
    }
    return globals->get_value(name);
}
