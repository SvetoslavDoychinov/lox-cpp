//
// Created by svetoslav on 31.3.2026 г..
//

#include "ast_printer.h"

#include <memory>

void ASTPrinter::visit_binary_expr(const Binary *expr) {
    this->parenthesize(expr->oprtr.get_lexeme(), std::vector {expr->left, expr->right});
}

void ASTPrinter::visit_grouping_expr(const Grouping *expr) {
    parenthesize("group", std::vector {expr->expression});
}

void ASTPrinter::visit_literal_expr(const Literal *expr) {
    if (std::holds_alternative<double>(expr->value)) {
        std::cout << std::get<double>(expr->value) << std::endl;
    }
    else if (std::holds_alternative<bool>(expr->value)) {
        std::cout << std::get<bool>(expr->value) << std::endl;
    }
    else {
        std::cout << std::get<std::string>(expr->value) << std::endl;
    }
}

void ASTPrinter::visit_unary_expr(const Unary *expr) {
    this->parenthesize(expr->oprtr.get_lexeme(), std::vector {expr->right});
}

void ASTPrinter::parenthesize(const std::string &name, const std::vector<std::shared_ptr<Expr>> &expressions) {
    std::cout << "(" << name;
    for (auto &expr : expressions) {
        std::cout << " ";
        // The issue with shared_ptr here is that once we exit the scope of this func the whole ASTPrinter primitive is destroyed
        // Because the declared share ptr is out of scope now
        expr->accept(this);
    }
    std::cout << ")";
}
