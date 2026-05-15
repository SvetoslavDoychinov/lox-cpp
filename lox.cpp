#include "lox.h"

#include <iostream>
#include <fstream>

#include "ast_printer.h"
#include "parser.h"
#include "resolver.h"
#include "scanner.h"


bool Lox::had_error = false;
bool Lox::had_runtime_error = false;
int Lox::exit_code = 0;

void Lox::run(const std::string& source) {
    Scanner scanner(source);
    const std::vector<Token> tokens = scanner.scan_tokens();
    if (Lox::had_error) return;

    Parser parser(tokens);
    const auto parsed_result = parser.parse();
    if (Lox::had_error) return;

    Resolver resolver(this->interpreter);
    resolver.resolve(parsed_result);
    if (Lox::had_error) return;

    this->interpreter->interpret(parsed_result);
}

void Lox::run_prompt() {
    while (true) {
        std::string line;
        std::cout << ">>";
        std::getline(std::cin, line);
        if (line.empty()) break;
        this->run(line);
        Lox::had_error = false;
    }
}

void Lox::run_file(const std::string& Path) {
    std::ifstream my_file(Path, std::ios::binary);
    if (!my_file.is_open()) {
        throw FileException("Failed to open file.");
    }
    my_file.seekg(0, std::ios::end);
    const std::streamsize end_pos = my_file.tellg();
    my_file.seekg(0, std::ios::beg);

    std::vector<std::byte> my_buffer(end_pos);
    my_file.read(reinterpret_cast<char *>(my_buffer.data()), end_pos);
    my_file.close();
    // TODO Escape sequences get treated like separate normal characters that way? Maybe find a fix?
    this->run(std::string(reinterpret_cast<char *>(my_buffer.data()), end_pos));
    // Indicate error in exit code
    if (Lox::had_error) {
        Lox::exit_code = 60;
        return;
    }
    if (Lox::had_runtime_error) Lox::exit_code = 70;
}

void Lox::error(const int line, const std::string& message) {
    this->report(line, "", message);
}

void Lox::error(const Token &token, const std::string &message) {
    if (token.get_type() == ENDOFFILE) {
        this->report(token.get_line(), " at end", message);
    }
    else {
        this->report(token.get_line(), " at '" + token.get_lexeme() + "'", message);
    }
}

void Lox::report(int line, const std::string& where, const std::string& message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
    Lox::had_error = true;
}

void Lox::runtime_error(const RuntimeError &error) {
    std::cerr << error.get_message() << std::endl << "[line " << error.get_token().get_line() << "]" << std::endl;
    Lox::had_runtime_error = true;
}

int Lox::get_exit_code() const {
    return Lox::exit_code;
}
