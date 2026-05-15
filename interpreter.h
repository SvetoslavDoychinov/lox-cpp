//
// Created by svetoslav on 8.4.2026 г..
//

#ifndef JLOX_INTERPRETER_H
#define JLOX_INTERPRETER_H

#include <vector>

#include "environment.h"
#include "expr.h"
#include "stmt.h"
#include "lox_callable.h"


class RuntimeError: public std::runtime_error {

private:
    std::string message;
    Token token;

public:
    RuntimeError(Token token, std::string message) :
    std::runtime_error(""),
    message(std::move(message)),
    token(std::move(token)) {}

    [[nodiscard]] const char * what() const noexcept final {
        return this->message.c_str();
    }

    [[nodiscard]] std::string get_message() const noexcept {return this->message;}
    [[nodiscard]] Token get_token() const noexcept {return this->token;}
};


class BreakError: public std::exception {

public:
    BreakError() = default;
};


class ContinueError: public std::exception {

public:
    ContinueError() = default;
};


class ReturnError: public std::exception {
    private:
    object return_value;

    public:
    explicit ReturnError(object return_value) : return_value(std::move(return_value)) {}
    object get_return_value() const noexcept {return this->return_value;}
};


class Interpreter: public ExprVisitor, public StmtVisitor {

    public:
    const std::shared_ptr<Environment> globals = std::make_shared<Environment>();
    std::shared_ptr<Environment> environment = globals;

    Interpreter();

    void visit_binary_expr(const Binary *expr) override;
    void visit_grouping_expr(const Grouping *expr) override;
    void visit_literal_expr(const Literal *expr) override;
    void visit_unary_expr(const Unary *expr) override;
    void visit_variable_expr(const Variable *expr) override;
    void visit_assign_expr(const Assign *expr) override;
    void visit_logical_expr(const Logical *expr) override;
    void visit_call_expr(const Call *expr) override;


    void visit_expression_stmt(const Expression *stmt) override;
    void visit_var_stmt(const Var *stmt) override;
    void visit_block_stmt(const Block *stmt) override;
    void visit_if_stmt(const If *stmt) override;
    void visit_while_stmt(const While *stmt) override;
    void visit_for_stmt(const For *stmt) override;
    void visit_break_stmt(const Break *stmt) override;
    void visit_continue_stmt(const Continue *stmt) override;
    void visit_function_stmt(const Function *stmt) override;
    void visit_return_stmt(const Return *stmt) override;

    void interpret(const std::vector<std::shared_ptr<Stmt>>& statements);
    void execute_block(const Block *stmt, const std::shared_ptr<Environment>& env);
    void resolve(const Expr *expr, int depth); // It's relative to current scope is distance a better name?

    private:
    object result;
    std::unordered_map<const Expr*, int> locals;

    void evaluate(const std::shared_ptr<Expr> &expression);
    void check_number_operand(const Token &oprtr, const object &right) const;
    void check_number_operands(const object &left, const Token &oprtr, const object &right) const;
    bool is_truthy(const object &object);
    void check_division_by_zero(const Token &oprtr, const object &right) const;
    void execute(const std::shared_ptr<Stmt> &stmt);
    object convert_to_object(const primitive &value);
    object look_up_variable(const Token &name, const Expr *expr) const;
};


#endif //JLOX_INTERPRETER_H