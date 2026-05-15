#include <complex>
#include <iostream>
#include <memory>
#include <stack>
#include <variant>

#include "ast_printer.h"
#include "expr.h"
#include "lox.h"
#include "lox_function.h"


int main(const int argc, char *argv[]) {
    auto lox = Lox();
    const std::vector<std::string> args(argv + 1, argv + argc);
    if (args.size() > 1) {
        std::cout << "Usage jlox [script]";
        return 64;
    }
    if (args.size() == 1) {
        lox.run_file(args.at(0));
    }
    else {
        lox.run_prompt();
    }

    return lox.get_exit_code();
}

