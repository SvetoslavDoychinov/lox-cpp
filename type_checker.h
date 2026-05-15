//
// Created by svetoslav on 12.4.2026 г..
//

#ifndef JLOX_TYPE_CHECKER_H
#define JLOX_TYPE_CHECKER_H
#include "expr.h"


class TypeChecker: public ExprVisitor {

    public:
    TypeChecker() = default;

    void visit_binary_expr(const Binary *expr) override;
    void visit_grouping_expr(const Grouping *expr) override;
    void visit_literal_expr(const Literal *expr) override;
    void visit_unary_expr(const Unary *expr) override;
    void visit_variable_expr(const Variable *expr) override;
    void visit_assign_expr(const Assign *expr) override;
    void visit_logical_expr(const Logical *expr) override;
    void visit_call_expr(const Call *expr) override;

    bool check(const std::shared_ptr<Expr> &left, const std::shared_ptr<Expr> &right);

    private:
    std::string type;
};


#endif //JLOX_TYPE_CHECKER_H