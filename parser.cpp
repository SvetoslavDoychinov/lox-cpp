//
// Created by svetoslav on 2.4.2026 г..
//

#include "parser.h"

#include "lox.h"
#include "type_checker.h"


Parser::Parser(const std::vector<Token> &tokens) {
    this->tokens = tokens;
}

std::vector<std::shared_ptr<Stmt>> Parser::parse() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!this->is_at_end()) {
        try {
            statements.push_back(this->declaration());
        }
        catch (ParseError&) {
            this->synchronize();
        }
    }

    return statements;
}

std::shared_ptr<Stmt> Parser::declaration() {
    if (this->match({VAR})) {return this->var_declaration();}
    if (this->match({FUN})) {return this->function("function");}
    return this->statement();
}

std::shared_ptr<Stmt> Parser::var_declaration() {
    Token name = this->consume(IDENTIFIER, "Expect variable name.");

    std::shared_ptr<Expr> initializer;
    if (this->match({EQUAL})) {
       initializer = this->expression();
    }

    this->consume(SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<Var>(name, initializer);
}

std::shared_ptr<Function> Parser::function(const std::string &kind) {
    Token name = this->consume(IDENTIFIER, "Expect " + kind + " name.");
    this->consume(LEFT_PAREN, "Expect '(' after" + kind + " name.");
    std::vector<Token> parameters;
    if (!this->check(RIGHT_PAREN)) {
        do {
            if (parameters.size() >= 255) {
                Parser::error(this->peek(), "Can't have more than 255 parameters.");
            }

            parameters.push_back(this->consume(IDENTIFIER, "Expect parameter name."));
        } while (this->match({COMMA}));
    }
    this->consume(RIGHT_PAREN, "Expect ')' after parameters.");

    // Function body, just a block stmt
    this->consume(LEFT_BRACE, "Expect '{' before " + kind + " body.");
    auto body = this->block();
    return std::make_shared<Function>(name, parameters, body);
}

std::shared_ptr<Stmt> Parser::statement() {
    if (this->match({LEFT_BRACE})) {return std::make_shared<Block>(this->block());}
    if (this->match({IF})) {return this->if_statement();}
    if (this->match({WHILE})) {return this->while_statement();}
    if (this->match({FOR})) {return this->for_statement();}
    if (this->match({BREAK})) {return this->break_statement();}
    if (this->match({CONTINUE})) {return this->continue_statement();}
    if (this->match({RETURN})) {return this->return_statement();}
    return this->expression_statement();
}

std::shared_ptr<Stmt> Parser::for_statement() {
    this->consume(LEFT_PAREN, "Expect '(' after 'for'.");
    std::shared_ptr<Stmt> initializer;
    if (this->match({VAR})) {
        initializer = this->var_declaration();
    }
    else if (!this->match({SEMICOLON})) {
        initializer = this->expression_statement();
    }

    std::shared_ptr<Expr> condition;
    if (!this->check(SEMICOLON)) {
        condition = this->expression();
    }
    this->consume(SEMICOLON, "Expect ';' after loop condition.");

    std::shared_ptr<Expr> increment;
    if (!this->check(RIGHT_PAREN)) {
        increment = this->expression();
    }
    this->consume(RIGHT_PAREN, "Expect ')' after condition.");


    this->consume(LEFT_BRACE, "Expect '{' after `for` statement.");
    std::shared_ptr<Stmt> body = std::make_shared<Block>(this->block());


    if (!condition) {
        condition = std::make_shared<Literal>(true);
    }
    body = std::make_shared<For>(condition, body, increment);
    if (initializer) {
        body = std::make_shared<Block>(std::vector{initializer, body});
    }

    return body;
}

// TODO The error reporting is a bit shit with all the changes I've made to the language. Maybe improve it?
std::shared_ptr<Stmt> Parser::while_statement() {
    this->consume(LEFT_PAREN, "Expect '(' after 'while' statement.");
    auto condition = this->expression();
    this->consume(RIGHT_PAREN, "Expect ')' after condition.");
    // I want it to require a block statement {} for loop and if bodies. I like it and it's more readable.
    this->consume(LEFT_BRACE, "Expect '{' after `while` statement.");

    auto body = std::make_shared<Block>(this->block());;

    return std::make_shared<While>(condition, body);
}

std::shared_ptr<Stmt> Parser::if_statement() {
    this->consume(LEFT_PAREN, "Expect '(' after 'if'.");
    auto condition = this->expression();
    this->consume(RIGHT_PAREN, "Expect ')' after condition.");

    this->consume(LEFT_BRACE, "Expect '{' after condition.");

    auto then_branch = std::make_shared<Block>(this->block());
    std::shared_ptr<Stmt> else_branch;
    if (this->match({ELSE})) {
        else_branch = this->statement();
    }

    return std::make_shared<If>(condition, then_branch, else_branch);
}

std::vector<std::shared_ptr<Stmt>> Parser::block() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!this->check(RIGHT_BRACE) && !this->is_at_end()) {
        statements.push_back(this->declaration());
    }
    this->consume(RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::shared_ptr<Stmt> Parser::expression_statement() {
    std::shared_ptr<Expr> expression = this->expression();
    this->consume(SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<Expression>(expression);
}

std::shared_ptr<Stmt> Parser::break_statement() {
    std::shared_ptr<Stmt> statement = std::make_shared<Break>(this->previous());
    this->consume(SEMICOLON, "Expect ';' after 'break'.");
    return statement;
}

std::shared_ptr<Stmt> Parser::continue_statement() {
    std::shared_ptr<Stmt> statement = std::make_shared<Continue>(this->previous());
    this->consume(SEMICOLON, "Expect ';' after 'continue'.");
    return statement;
}

std::shared_ptr<Stmt> Parser::return_statement() {
    auto keyword = this->previous();
    std::shared_ptr<Expr> expression;
    if (!this->check(SEMICOLON)) {
        expression = this->expression();
    }
    this->consume(SEMICOLON, "Expect ';' after return statement.");
    return std::make_shared<Return>(keyword, expression);
}

std::shared_ptr<Expr> Parser::expression() {
    return this->assignment();
}

std::shared_ptr<Expr> Parser::assignment() {
    auto left = this->or_expression();

    if (this->match({EQUAL})) {
        auto equals = this->previous();
        auto value = this->assignment();

        if (TypeChecker type_checker; type_checker.check(left, std::make_shared<Variable>(equals))) {
            Token name = std::static_pointer_cast<Variable>(left)->name;
            return std::make_shared<Assign>(name, value);
        }

        Parser::error(equals, "Invalid assignment target.");
    }
    return left;
}

std::shared_ptr<Expr> Parser::or_expression() {
    auto expr = this->and_expression();

    while (this->match({OR})) {
        Token oprtr = this->previous();
        auto right = this->and_expression();
        expr = std::make_shared<Logical>(expr, oprtr, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::and_expression() {
    auto expr = this->equality();

    while (this->match({AND})) {
        Token oprtr = this->previous();
        auto right = this->equality();
        expr = std::make_shared<Logical>(expr, oprtr, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::equality() {
    std::shared_ptr<Expr> expr = this->comparison();

    while (this->match({EQUAL_EQUAL, BANG_EQUAL})) {
        Token oprtr = this->previous();
        std::shared_ptr<Expr> right = this->comparison();
        expr = std::make_shared<Binary>(expr, oprtr, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::comparison() {
    std::shared_ptr<Expr> expr = this->term();

    while (this->match({GREATER_EQUAL, LESS, LESS_EQUAL, GREATER})) {
        Token oprtr = this->previous();
        std::shared_ptr<Expr> right = this->term();
        expr = std::make_shared<Binary>(expr, oprtr, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::term() {
    std::shared_ptr<Expr> expr = this->factor();

    while (this->match({MINUS, PLUS})) {
        Token oprtr = this->previous();
        std::shared_ptr<Expr> right = this->factor();
        expr = std::make_shared<Binary>(expr, oprtr, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::factor() {
    std::shared_ptr<Expr> expr = this->unary();

    while (this->match({STAR, SLASH})) {
        Token oprtr = this->previous();
        std::shared_ptr<Expr> right = this->unary();
        expr = std::make_shared<Binary>(expr, oprtr, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::unary() {
    if (this->match(std::vector{BANG, MINUS})) {
        Token oprtr = this->previous();
        std::shared_ptr<Expr> right = this->primary();
        auto expr = std::make_shared<Unary>(oprtr, right);
    }
    return this->call();
}

std::shared_ptr<Expr> Parser::call() {
    auto expr = this->primary();

    while (true) {
        if (this->match({LEFT_PAREN})) {
            expr = this->finish_call(expr);
        }
        else {
            break;
        }
    }
    return expr;
}

std::shared_ptr<Expr> Parser::finish_call(const std::shared_ptr<Expr>& callee) {
    std::vector <std::shared_ptr<Expr>> arguments;
    if (!this->check(RIGHT_PAREN)) {
        do {
            arguments.push_back(this->expression());
        } while (this->match({COMMA}));
    }
    if (arguments.size() > 255) {
        Parser::error(this->peek(), "Can't have more than 255 arguments");
    }
    Token paren = this->consume(RIGHT_PAREN, "Expect ')' after arguments.");
    return std::make_shared<Call>(callee, paren, arguments);
}

std::shared_ptr<Expr> Parser::primary() {
    if (this->match({FALSE})) return std::make_shared<Literal>(false);
    if (this->match({TRUE})) return std::make_shared<Literal>(true);
    if (this->match({NIL})) return std::make_shared<Literal>(nullptr);

    if (this->match({NUMBER, STRING})) {
        return std::make_shared<Literal>(previous().get_literal());
    }

    if (this->match({IDENTIFIER})) {
        return std::make_shared<Variable>(this->previous());
    }

    if (this->match({LEFT_PAREN})) {
        std::shared_ptr<Expr> expr = this->expression();
        this->consume(RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<Grouping>(expr);
    }
    // Will reach here at token types we haven't made parser logic for (identifier, for, while, etc.)
    throw Parser::error(this->peek(), "Expect expression.");
}

bool Parser::match(const std::vector<TokenType> &token_types) {
    for (const auto type: token_types) {
        if (this->check(type)) {
            this->advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(const TokenType type) const {
    if (this->is_at_end()) return false;
    return this->peek().get_type() == type;
}

Token Parser::advance() {
    if (!this->is_at_end()) current++;
    return this->previous();
}

bool Parser::is_at_end() const {
    return this->peek().get_type() == ENDOFFILE;
}

Token Parser::peek() const {
    return this->tokens.at(current);
}

Token Parser::previous() const {
    return this->tokens.at(current - 1);
}

Token Parser::consume(TokenType type, const std::string &message) {
    if (this->check(type)) return this->advance();
    throw Parser::error(this->peek(), message);
}

ParseError Parser::error(const Token &type, const std::string &message) {
    auto lox = Lox();
    lox.error(type, message);
    return ParseError(message);
}

void Parser::synchronize() {
    this->advance();

    while (!this->is_at_end()) {
        if (previous().get_type() == SEMICOLON) return;

        switch (this->peek().get_type()) {
            case CLASS:
            case FUN:
            case VAR:
            case FOR:
            case IF:
            case WHILE:
            case RETURN:
                return;
            default: break;
        }

        this->advance();
    }
}
