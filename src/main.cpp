#include "Lexer/Lexer.h"

int main(int countArg, char* args[]) {
    system("read -N 1 -p 'Press any key to continue...'");

    wd::Log::init();

    wd::LexicalAnalyzer analyzer;

    analyzer.config([](auto& self) {
        self.keyWords = { { "Yeah!", "Happy" } };
    });

    auto tokens = analyzer.analyz("Happy 32 Happy 65 Happy 89 Happy91 Happy");
    std::cout << "Count token:" << tokens.size() << "\n";
    for (auto token : tokens)
        std::cout << "Token ( type: " << token.first << "; value: " << token.second << "; )\n";

    wd::Log::warning() << "Warning";

    system("read -N 1 -p 'Press any key to continue...'");

    wd::Log::error() << false;
    wd::Log::info() << true;

    system("read -N 1 -p 'Press any key to continue...'");

    return EXIT_SUCCESS;
}