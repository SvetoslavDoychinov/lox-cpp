//
// Created by svetoslav on 12.4.2026 г..
//

#include "type_checker.h"


void TypeChecker::visit_assign_expr(const Assign *expr) {
    this->type = "Assign";
}

void TypeChecker::visit_binary_expr(const Binary *expr) {
    this->type = "Binary";
}

void TypeChecker::visit_unary_expr(const Unary *expr) {
    this->type = "Unary";
}

void TypeChecker::visit_variable_expr(const Variable *expr) {
    this->type = "Variable";
}

void TypeChecker::visit_grouping_expr(const Grouping *expr) {
    this->type = "Grouping";
}

void TypeChecker::visit_literal_expr(const Literal *expr) {
    this->type = "Literal";
}

void TypeChecker::visit_logical_expr(const Logical *expr) {
    this->type = "Logical";
}

void TypeChecker::visit_call_expr(const Call *expr) {
    this->type = "Call";
}

bool TypeChecker::check(const std::shared_ptr<Expr> &left, const std::shared_ptr<Expr> &right) {
    left->accept(this);
    const auto left_type = this->type;
    right->accept(this);
    return left_type == this->type;
}
