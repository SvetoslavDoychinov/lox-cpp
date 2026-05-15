#ifndef JLOX_LIBRARY_H
#define JLOX_LIBRARY_H
#include <string>
#include <vector>

#include "interpreter.h"
#include "token.h"


class FileException : public std::exception {
private:
    std::string message;
public:
    explicit FileException(std::string message): message(std::move(message)) {};

    [[nodiscard]] const char * what() const noexcept final {
        return this->message.c_str();
    }
};


class Lox {
    public:
    static bool had_error;
    static bool had_runtime_error;
    static int exit_code;

    Lox() = default;
    ~Lox() = default;

    void run_file(const std::string& Path);
    void run_prompt();
    void error(int line, const std::string& message);
    void error(const Token &token, const std::string& message);
    void runtime_error(const RuntimeError& error);

    int get_exit_code() const;

    private:
    std::shared_ptr<Interpreter> interpreter = std::make_shared<Interpreter>();

    void run(const std::string& source);
    void report(int line, const std::string& where, const std::string& message);
};



#endif // JLOX_LIBRARY_H