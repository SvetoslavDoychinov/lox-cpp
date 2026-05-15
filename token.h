#ifndef JLOX_TOKEN_H
#define JLOX_TOKEN_H
#include <iostream>
#include <string>
#include <variant>

#include "token_type.h"

using primitive = std::variant<double, std::string, bool, std::nullptr_t>;


// A Lexeme is the smallest substring segment of the file that has a meaning, e.g.
// the line var i = "a value"", the lexemes are var, i, =, "a value",
// A Token contains the lexeme and additional information that makes it meaningful to the interpreter
class Token {
    TokenType type;
    std::string lexeme;
    int line;
    primitive literal;

    public:
    Token(const TokenType &type, const std::string &lexeme, int line, const primitive &literal);
    std::string to_string() const;
    std::string get_lexeme() const;
    TokenType get_type() const;
    primitive get_literal() const;
    int get_line() const;
};


#endif //JLOX_TOKEN_H