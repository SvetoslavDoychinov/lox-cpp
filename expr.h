#ifndef JLOX_EXPR_H
#define JLOX_EXPR_H
#include <memory>
#include <utility>
#include <vector>

#include "token.h"


//Forward declarations
class Binary;
class Grouping;
class Literal;
class Unary;
class Variable;
class Assign;
class Logical;
class Call;


class ExprVisitor {
    public:
    virtual ~ExprVisitor() = default;
    virtual void visit_binary_expr(const Binary *expr) = 0;
    virtual void visit_grouping_expr(const Grouping *expr) = 0;
    virtual void visit_literal_expr(const Literal *expr) = 0;
    virtual void visit_unary_expr(const Unary *expr) = 0;
    virtual void visit_variable_expr(const Variable *expr) = 0;
    virtual void visit_assign_expr(const Assign *expr) = 0;
    virtual void visit_logical_expr(const Logical *expr) = 0;
    virtual void visit_call_expr(const Call *expr) = 0;
};


class Expr {
    public:
    Expr() = default;
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor *visitor) = 0;
};


class Binary: public Expr {
    public:
    std::shared_ptr<Expr> left; Token oprtr; std::shared_ptr<Expr> right;
    Binary(std::shared_ptr<Expr> left, Token oprtr, std::shared_ptr<Expr> right)
    : left(std::move(left)), oprtr(std::move(oprtr)), right(std::move(right)) {}

    void accept(ExprVisitor *visitor) override {
        return visitor->visit_binary_expr(this);
    }
};


class Grouping: public Expr {
    public:
    std::shared_ptr<Expr> expression;
    explicit Grouping(std::shared_ptr<Expr> expression) : expression(std::move(expression)){}

    void accept(ExprVisitor *visitor) override {
        return visitor->visit_grouping_expr(this);
    }
};


class Literal: public Expr {
    public:
    primitive value;
    explicit Literal(primitive value) : value(std::move(value)) {}

    void accept(ExprVisitor *visitor) override {
        return visitor->visit_literal_expr(this);
    }
};


class Unary: public Expr {
    public:
    Token oprtr; std::shared_ptr<Expr> right;
    Unary(Token oprtr, std::shared_ptr<Expr> right) : oprtr(std::move(oprtr)), right(std::move(right)) {}

    void accept(ExprVisitor *visitor) override {
        return visitor->visit_unary_expr(this);
    }
};


class Variable: public Expr {
public:
    Token name;
    explicit Variable(Token name) : name(std::move(name)) {}

    void accept(ExprVisitor *visitor) override {
        return visitor->visit_variable_expr(this);
    }
};

class Assign: public Expr {
public:
    Token name;
    std::shared_ptr<Expr> value;
    explicit Assign(Token name, std::shared_ptr<Expr> value) : name(std::move(name)), value(std::move(value)) {}

    void accept(ExprVisitor *visitor) override {
        return visitor->visit_assign_expr(this);
    }
};


class Logical: public Expr {
public:
    std::shared_ptr<Expr> left; Token oprtr; std::shared_ptr<Expr> right;
    Logical(std::shared_ptr<Expr> left, Token oprtr, std::shared_ptr<Expr> right)
    : left(std::move(left)), oprtr(std::move(oprtr)), right(std::move(right)) {}

    void accept(ExprVisitor *visitor) override {
        return visitor->visit_logical_expr(this);
    }
};


class Call: public Expr {
    public:
    std::shared_ptr<Expr> callee; Token paren; std::vector<std::shared_ptr<Expr>> arguments;
    Call(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> arguments)
    : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}

    void accept(ExprVisitor *visitor) override {
        visitor->visit_call_expr(this);
    }
};


#endif //JLOX_EXPR_H
