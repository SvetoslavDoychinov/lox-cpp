//
// Created by svetoslav on 31.3.2026 г..
//

#ifndef JLOX_AST_PRINTER_H
#define JLOX_AST_PRINTER_H
#include <vector>

#include "expr.h"


class ASTPrinter: ExprVisitor {
    public:
    ASTPrinter() = default;
    ~ASTPrinter() override {std::cout << "~ASTPrinter()" << std::endl;};

    void visit_binary_expr(const Binary *expr) override;
    void visit_grouping_expr(const Grouping *expr) override;
    void visit_literal_expr(const Literal *expr) override;
    void visit_unary_expr(const Unary *expr) override;

    void print(const std::shared_ptr<Expr> &expr) {
        ExprVisitor *v = this;
        // The issue with shared_ptr here is that once we exit the scope of this func the whole ASTPrinter primitive is destroyed
        // Because the declared share ptr is out of scope now
        expr->accept(v);
    }

    private:
    void parenthesize(const std::string &name, const std::vector<std::shared_ptr<Expr>> &expressions);
};


#endif //JLOX_AST_PRINTER_H