//
// Created by svetoslav on 28.4.2026 г..
//

#ifndef JLOX_RESOLVER_H
#define JLOX_RESOLVER_H
#include "expr.h"
#include "interpreter.h"
#include "stmt.h"


enum FunctionType {
    FUNCTION,
    NONE,
};

class Resolver: public ExprVisitor, public StmtVisitor {
    private:
    std::shared_ptr<Interpreter> interpreter;
    std::vector<std::unordered_map<std::string, bool>> scopes;
    FunctionType current_function = NONE;
    int loop_depth = 0;

    void resolve(const std::shared_ptr<Stmt> &statement);
    void resolve(const std::shared_ptr<Expr> &expression);
    void begin_scope();
    void end_scope();
    void declare(const Token &name);
    void define(const Token &name);
    void resolve_local(const Expr *expr, const Token &name) const;
    void resolve_function(const Function *function, FunctionType type);

    public:
    explicit Resolver(std::shared_ptr<Interpreter> interpreter) : interpreter(std::move(interpreter)) {};

    void resolve(const std::vector<std::shared_ptr<Stmt>> &statements);

    void visit_block_stmt(const Block *stmt) override;
    void visit_var_stmt(const Var *stmt) override;
    void visit_function_stmt(const Function *stmt) override;
    void visit_expression_stmt(const Expression *stmt) override;
    void visit_if_stmt(const If *stmt) override;
    void visit_return_stmt(const Return *stmt) override;
    void visit_while_stmt(const While *stmt) override;
    void visit_for_stmt(const For *stmt) override;
    void visit_break_stmt(const Break *stmt) override; // Should be just empty probably
    void visit_continue_stmt(const Continue *stmt) override; // Should be just empty probably

    void visit_variable_expr(const Variable *expr) override;
    void visit_assign_expr(const Assign *expr) override;
    void visit_binary_expr(const Binary *expr) override;
    void visit_call_expr(const Call *expr) override;
    void visit_grouping_expr(const Grouping *expr) override;
    void visit_literal_expr(const Literal *expr) override;
    void visit_logical_expr(const Logical *expr) override;
    void visit_unary_expr(const Unary *expr) override;
};


#endif //JLOX_RESOLVER_H