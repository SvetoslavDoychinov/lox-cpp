//
// Created by svetoslav on 25.3.2026 г..
//

#ifndef JLOX_SCANNER_H
#define JLOX_SCANNER_H
#include <string>
#include <vector>

#include "token.h"

const std::unordered_map<std::string, TokenType> identifiers_map = {
    {"and", TokenType::AND},
    {"class", TokenType::CLASS},
    {"else", TokenType::ELSE},
    {"false", TokenType::FALSE},
    {"fun", TokenType::FUN},
    {"for", TokenType::FOR},
    {"if", TokenType::IF},
    {"nil", TokenType::NIL},
    {"or", TokenType::OR},
    {"return", TokenType::RETURN},
    {"super", TokenType::SUPER},
    {"this", TokenType::THIS},
    {"true", TokenType::TRUE},
    {"var", TokenType::VAR},
    {"while", TokenType::WHILE},
    {"break", TokenType::BREAK},
    {"continue", TokenType::CONTINUE},
};


class Scanner {
    private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    bool is_at_end() const;
    void scan_token();
    char advance();
    void add_token(TokenType type);
    void add_token(TokenType type, const primitive &literal);
    bool match(const char &expected);
    char peek() const;
    void string();

    bool is_digit(const char &c);
    void number();
    char peek_next() const;
    void identifier();
    bool is_alpha(const char &c) const;
    bool is_alpha_numeric(const char &c);
    void skip_multiline_comment();

    public:
    explicit Scanner(const std::string &source) {this->source = source;};
    ~Scanner() = default;

    std::vector<Token> scan_tokens();

};


#endif //JLOX_SCANNER_H