//
// Created by svetoslav on 2.4.2026 г..
//

#ifndef JLOX_PARSER_H
#define JLOX_PARSER_H
#include <memory>
#include <vector>

#include "expr.h"
#include "stmt.h"
#include "token.h"


class ParseError : public std::exception {
private:
    std::string message;
public:
    explicit ParseError(std::string message): message(std::move(message)) {};

    [[nodiscard]] const char * what() const noexcept final {
        return this->message.c_str();
    }
};


class Parser {
    private:
    std::vector<Token> tokens;
    int current = 0;

    // Expressions, recursive dive top to bottom order
    std::shared_ptr<Expr> expression();
    std::shared_ptr<Expr> equality();
    std::shared_ptr<Expr> comparison();
    std::shared_ptr<Expr> term();
    std::shared_ptr<Expr> factor();
    std::shared_ptr<Expr> unary();
    std::shared_ptr<Expr> call();
    std::shared_ptr<Expr> primary();
    std::shared_ptr<Expr> assignment();
    std::shared_ptr<Expr> or_expression();
    std::shared_ptr<Expr> and_expression();
    std::shared_ptr<Expr> finish_call(const std::shared_ptr<Expr>& callee);

    bool match(const std::vector<TokenType> &token_types);
    bool check(TokenType type) const;
    Token advance();
    bool is_at_end() const;
    Token peek() const;
    Token previous() const;
    Token consume(TokenType type, const std::string &message);

    std::shared_ptr<Function> function(const std::string &kind);


    static ParseError error(const Token &type, const std::string &message);
    void synchronize();

    std::shared_ptr<Stmt> declaration();
    std::shared_ptr<Stmt> var_declaration();
    std::shared_ptr<Stmt> statement();
    std::vector<std::shared_ptr<Stmt>> block();
    std::shared_ptr<Stmt> expression_statement();
    std::shared_ptr<Stmt> if_statement();
    std::shared_ptr<Stmt> while_statement();
    std::shared_ptr<Stmt> for_statement();
    std::shared_ptr<Stmt> break_statement();
    std::shared_ptr<Stmt> continue_statement();
    std::shared_ptr<Stmt> return_statement();

    public:
    explicit Parser(const std::vector<Token> &tokens);

    std::vector<std::shared_ptr<Stmt>> parse();
};


#endif //JLOX_PARSER_H