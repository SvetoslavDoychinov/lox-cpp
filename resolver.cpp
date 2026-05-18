//
// Created by svetoslav on 28.4.2026 г..
//

#include "resolver.h"

#include "lox.h"
#include "type_checker.h"


void Resolver::visit_block_stmt(const Block *stmt) {
    this->begin_scope();
    this->resolve(stmt->statements);
    this->end_scope();
}

void Resolver::visit_var_stmt(const Var *stmt) {
    this->declare(stmt->name);
    if (stmt->initializer) {
        this->resolve(stmt->initializer);
    }
    this->define(stmt->name);
}

void Resolver::visit_function_stmt(const Function *stmt) {
    this->declare(stmt->name);
    this->define(stmt->name);
    this->resolve_function(stmt, FUNCTION);
}

void Resolver::visit_class_stmt(const Class *stmt) {
    this->declare(stmt->name);
    this->define(stmt->name);
}

void Resolver::visit_expression_stmt(const Expression *stmt) {
    this->resolve(stmt->expression);
}

void Resolver::visit_if_stmt(const If *stmt) {
    this->resolve(stmt->condition);
    this->resolve(stmt->then_branch);
    if (stmt->else_branch) this->resolve(stmt->else_branch);
}

void Resolver::visit_return_stmt(const Return *stmt) {
    if (this->current_function == NONE) {
        auto lox = Lox();
        lox.error(stmt->keyword, "Can't return from top-level code.");
    }
    if (stmt->value) this->resolve(stmt->value);
}

void Resolver::visit_while_stmt(const While *stmt) {
    this->loop_depth++;
    this->resolve(stmt->condition);
    this->resolve(stmt->body);
    this->loop_depth--;
}

void Resolver::visit_for_stmt(const For *stmt) {
    this->loop_depth++;
    this->resolve(stmt->condition);
    this->resolve(stmt->body);
    this->resolve(stmt->increment);
    this->loop_depth--;
}

void Resolver::visit_break_stmt(const Break *stmt) {
    if (this->loop_depth <= 0) {
        auto lox = Lox();
        lox.error(stmt->name, "break' statement may only be used within a loop statement");
    }
}

void Resolver::visit_continue_stmt(const Continue *stmt) {
    if (this->loop_depth <= 0) {
        auto lox = Lox();
        lox.error(stmt->name, "continue' statement may only be used within a loop statement");
    }
}

void Resolver::visit_variable_expr(const Variable *expr) {
    auto lox = Lox();
    if (!this->scopes.empty() && this->scopes.back().contains(expr->name.get_lexeme()) && this->scopes.back().at(expr->name.get_lexeme()) == false) {
      lox.error(expr->name, "Can't read local variable in its own initializer.");
    }

    this->resolve_local(expr, expr->name);
}

void Resolver::visit_assign_expr(const Assign *expr) {
    this->resolve(expr->value);
    this->resolve_local(expr, expr->name);
}

void Resolver::visit_binary_expr(const Binary *expr) {
    this->resolve(expr->left);
    this->resolve(expr->right);
}

void Resolver::visit_call_expr(const Call *expr) {
    this->resolve(expr->callee);

    for (const auto& argument : expr->arguments) {
        this->resolve(argument);
    }
}

void Resolver::visit_grouping_expr(const Grouping *expr) {
    this->resolve(expr->expression);
}

void Resolver::visit_literal_expr(const Literal *expr) {}

void Resolver::visit_logical_expr(const Logical *expr) {
    this->resolve(expr->left);
    this->resolve(expr->right);
}

void Resolver::visit_unary_expr(const Unary *expr) {
    this->resolve(expr->right);
}

void Resolver::resolve(const std::vector<std::shared_ptr<Stmt> > &statements) {
    for (const auto& stmt : statements) {
        this->resolve(stmt);
    }
}

void Resolver::resolve(const std::shared_ptr<Stmt> &statement) {
    statement->accept(this);
}

void Resolver::resolve(const std::shared_ptr<Expr> &expression) {
    expression->accept(this);
}

void Resolver::begin_scope() {
    this->scopes.emplace_back();
}

void Resolver::end_scope() {
    this->scopes.pop_back();
}

void Resolver::declare(const Token &name) {
    if (!this->scopes.empty()) {
        if (this->scopes.back().contains(name.get_lexeme())) {
            auto lox = Lox();
            lox.error(name, "Variable already declared in this scope.");
        }

        this->scopes.back().insert({name.get_lexeme(), false});
    }
}

void Resolver::define(const Token &name) {
    if (!this->scopes.empty()) {
        this->scopes.back()[name.get_lexeme()] = true;
    }
}

void Resolver::resolve_local(const Expr *expr, const Token &name) const {
    for (int i = this->scopes.size() - 1; i >= 0; i--) {
        if (this->scopes.at(i).contains(name.get_lexeme())) {
            this->interpreter->resolve(expr, this->scopes.size() - 1 - i);
            return;
        }
    }
}

void Resolver::resolve_function(const Function *function, const FunctionType type) {
    const auto enclosing_function = this->current_function;
    this->current_function = type;
    this->begin_scope();
    for (const Token& token: function->params) {
        this->declare(token);
        this->define(token);
    }
    this->resolve(function->body);
    this->end_scope();
    this->current_function = enclosing_function;
}
