//
// Created by svetoslav on 9.4.2026 г..
//

#ifndef JLOX_STMT_H
#define JLOX_STMT_H
#include <memory>

#include "expr.h"

//Forward declarations
class Expression;
class Var;
class Block;
class If;
class While;
class For;
class Break;
class Continue;
class Function;
class Return;
class Class;


class StmtVisitor {
    public:
    virtual ~StmtVisitor() = default;

    virtual void visit_expression_stmt(const Expression *stmt) = 0;
    virtual void visit_var_stmt(const Var *stmt) = 0;
    virtual void visit_block_stmt(const Block *stmt) = 0;
    virtual void visit_if_stmt(const If *stmt) = 0;
    virtual void visit_while_stmt(const While *stmt) = 0;
    virtual void visit_break_stmt(const Break *stmt) = 0;
    virtual void visit_continue_stmt(const Continue *stmt) = 0;
    virtual void visit_for_stmt(const For *stmt) = 0;
    virtual void visit_function_stmt(const Function *stmt) = 0;
    virtual void visit_return_stmt(const Return *stmt) = 0;
    virtual void visit_class_stmt(const Class *stmt) = 0;
};


class Stmt {
    public:
    Stmt() = default;
    virtual ~Stmt() = default;

    virtual void accept(StmtVisitor *visitor) = 0;
};


class Expression: public Stmt {
    public:
    std::shared_ptr<Expr> expression;
    explicit Expression(std::shared_ptr<Expr> expression) : expression(std::move(expression)) {}

    void accept(StmtVisitor *visitor) override {
        visitor->visit_expression_stmt(this);
    }
};


class Var: public Stmt {
    public:
    std::shared_ptr<Expr> initializer;
    Token name;
    explicit Var(Token name, std::shared_ptr<Expr> initializer) : initializer(std::move(initializer)), name(std::move(name)){}

    void accept(StmtVisitor *visitor) override {
        visitor->visit_var_stmt(this);
    }
};


class Block: public Stmt {
    public:
    std::vector<std::shared_ptr<Stmt>> statements;
    explicit Block(std::vector<std::shared_ptr<Stmt>> statements) : statements(std::move(statements)) {}

    void accept(StmtVisitor *visitor) override {
        visitor->visit_block_stmt(this);
    }
};


class If: public Stmt {
    public:
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> then_branch;
    std::shared_ptr<Stmt> else_branch;
    If(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> then_branch, std::shared_ptr<Stmt> else_branch)
    : condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}

    void accept(StmtVisitor *visitor) override {
        visitor->visit_if_stmt(this);
    }
};


class While: public Stmt {
public:
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;
    While(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
    : condition(std::move(condition)), body(std::move(body)) {}

    void accept(StmtVisitor *visitor) override {
        visitor->visit_while_stmt(this);
    }
};


class For: public Stmt {
    // Necessary for the continue to work otherwise no way to differentiate between body and increment and continue skips increment
    // If we reused while statement instead the increment has to be part of the While body. And continue skips the whole body
public:
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;
    std::shared_ptr<Expr> increment;
    For(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body, std::shared_ptr<Expr> increment)
    : condition(std::move(condition)), body(std::move(body)), increment(std::move(increment)) {}

    void accept(StmtVisitor *visitor) override {
        visitor->visit_for_stmt(this);
    }
};


class Break: public Stmt {
    public:
    Token name;
    explicit Break(Token name) : name(std::move(name)) {}

    void accept(StmtVisitor *visitor) override {
        visitor->visit_break_stmt(this);
    }
};


class Continue: public Stmt {
    public:
    Token name;
    explicit Continue(Token name) : name(std::move(name)) {}

    void accept(StmtVisitor *visitor) override {
        visitor->visit_continue_stmt(this);
    }
};


class Function: public Stmt {
    public:
    Token name; std::vector<Token> params; std::vector<std::shared_ptr<Stmt>> body;
    explicit Function(Token name, std::vector<Token> params, std::vector<std::shared_ptr<Stmt>> body)
    : name(std::move(name)), params(std::move(params)), body(std::move(body)) {}

    void accept(StmtVisitor *visitor) override {
        visitor->visit_function_stmt(this);
    }
};


class Return: public Stmt {
    public:
    Token keyword; std::shared_ptr<Expr> value;
    Return(Token keyword, std::shared_ptr<Expr> value) : keyword(std::move(keyword)), value(std::move(value)) {}

    void accept(StmtVisitor *visitor) override {
        visitor->visit_return_stmt(this);
    }
};


class Class: public Stmt {
    public:
    Token name; std::vector<std::shared_ptr<Function>> methods;
    Class(Token name, std::vector<std::shared_ptr<Function>> methods) : name(std::move(name)), methods(std::move(methods)) {}

    void accept(StmtVisitor *visitor) override {
        visitor->visit_class_stmt(this);
    }
};


#endif //JLOX_STMT_H