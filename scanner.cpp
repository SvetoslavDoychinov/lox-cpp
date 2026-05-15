#include "scanner.h"

#include "lox.h"


bool Scanner::is_at_end() const {
    return this->current >= this->source.length();
}

std::vector<Token> Scanner::scan_tokens() {
    while (!is_at_end()) {
        this->start = this->current;

        this->scan_token();
    }
    // push_back but constructs element in place automatically
    this->tokens.emplace_back(ENDOFFILE, "", this->line, "nil");
    return this->tokens;
}

void Scanner::scan_token() {
    char c = this->advance();
    switch (c) {
        case '(': this->add_token(LEFT_PAREN); break;
        case ')': this->add_token(RIGHT_PAREN); break;
        case '{': this->add_token(LEFT_BRACE); break;
        case '}': this->add_token(RIGHT_BRACE); break;
        case ',': this->add_token(COMMA); break;
        case '.': this->add_token(DOT); break;
        case '-': this->add_token(MINUS); break;
        case '+': this->add_token(PLUS); break;
        case ';': this->add_token(SEMICOLON); break;
        case '*': this->add_token(STAR); break;
        case '!':
            this->add_token(this->match('=') ? BANG_EQUAL : BANG);
            break;
        case '=':
            this->add_token(this->match('=') ? EQUAL_EQUAL : EQUAL);
            break;
        case '<':
            this->add_token(this->match('=') ? LESS_EQUAL : LESS);
            break;
        case '>':
            this->add_token(this->match('=') ? GREATER_EQUAL : GREATER);
            break;
        case '/':
            if (this->match('/')) {
                // A comment goes until the end of the line.
                while (this->peek() != '\n' && !this->is_at_end()) this->advance();
            }
            else if (this->match('*')) {
                // C++ multiline style comments
                this->skip_multiline_comment();
            }
            else {
                this->add_token(SLASH);
            }
            break;
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;
        case '\n':
            this->line++;
            break;
        case '"': this->string(); break;
        default:
            if (this->is_digit(c)) {
                this->number();
            }
            else if (this->is_alpha(c)) {
                this->identifier();
            }
            else Lox().error(this->line, "Unexpected character.");
    }

}

char Scanner::advance() {
    return this->source.at(this->current++);
}

void Scanner::add_token(const TokenType type) {
    this->add_token(type, nullptr);
}

void Scanner::add_token(const TokenType type, const primitive &literal) {
    std::string lexeme = this->source.substr(this->start, this->current - this->start);
    this->tokens.emplace_back(type, lexeme, this->line, literal);
}

bool Scanner::match(const char &expected) {
    if (!this->is_at_end() && this->source.at(this->current) == expected) {
        this->current++;
        return true;
    }
    return false;
}

char Scanner::peek() const {
    if (this->is_at_end()) return '\0';
    return this->source.at(this->current);
}

void Scanner::string() {
    while (this->peek() != '"') {
        if (this->peek() == '\n') this->line++;
        this->advance();
    }
    if (this->is_at_end()) {
        Lox().error(this->line, "Unterminated string.");
    }
    std::string val = this->source.substr(this->start + 1, this->current - this->start - 1);
    // the closing "
    this->advance();
    this->add_token(STRING, val);

}

bool Scanner::is_digit(const char &c) {
    return c >= '0' && c <= '9';
}

void Scanner::number() {
    while (this->is_digit(this->peek())) this->advance();
    if (this->peek() == '.' && this->is_digit(this->peek_next())) {
        this->advance();
    }
    while (this->is_digit(this->peek())) this->advance();
    std::string val = this->source.substr(this->start, this->current - this->start);
    this->add_token(NUMBER, std::stod(val));
}

char Scanner::peek_next() const {
    if (this->current + 1 >= this->source.size()) return '\0';
    return this->source.at(this->current + 1);
}

bool Scanner::is_alpha(const char &c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::is_alpha_numeric(const char &c) {
    return this->is_digit(c) || this->is_alpha(c);
}

void Scanner::identifier() {
    while (this->is_alpha_numeric(this->peek())) this->advance();
    std::string lexeme = this->source.substr(this->start, this->current - this->start);
    if (identifiers_map.contains(lexeme)) {
        this->add_token(identifiers_map.at(lexeme));
    }
    else {
        this->add_token(IDENTIFIER);
    }
}

void Scanner::skip_multiline_comment() {
    while (!(this->peek() == '*' && this->peek_next() == '/')) {
        if (this->peek() == '\n') this->line++;
        if (this->peek_next() == '\0') {
            Lox().error(this->line, "Comment never closed.");
            break;
        }
        this->advance();
    }
    this->current += 2;
}
