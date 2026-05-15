//
// Created by svetoslav on 24.3.2026 г..
//

#include "token.h"

Token::Token(const TokenType &type, const std::string &lexeme, const int line, const primitive &literal) {
    this->type = type;
    this->lexeme = lexeme;
    this->line = line;
    this->literal = literal;
}


std::string Token::to_string() const {
    std::string literal_string;
    if (std::holds_alternative<double>(this->literal)) {
        literal_string = std::to_string(std::get<double>(literal));
    }
    else if (std::holds_alternative<bool>(this->literal)) {
        literal_string = std::get<bool>(literal) ? "true" : "false";
    }
    else {
        literal_string = std::get<std::string>(literal);
    }
    return TOKEN_TYPE_MAP.at(this->type) + " " + this->lexeme + " " + literal_string ;
}

std::string Token::get_lexeme() const {
    return this->lexeme;
}

TokenType Token::get_type() const {
    return this->type;
}

primitive Token::get_literal() const {
    return this->literal;
}

int Token::get_line() const {
    return this->line;
}
