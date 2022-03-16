#include "Lexer/Lexer.h"

int main(int countArg, char* args[]) {
    wd::LexicalAnalyzer analyzer;

    analyzer.config([](auto& self) {
        self.keyWords = { { "Yeah!", "Happy" } };
    });

    auto tokens = analyzer.analyz("Happy Happy Happy");
    std::cout << "Count token:" << tokens.size() << "\n";
    for (auto token : tokens)
        std::cout << "Token ( type: " << token.first << "; value: " << token.second << "; )\n";

    return EXIT_SUCCESS;
}