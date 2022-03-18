#include "Lexer/Lexer.h"

int main(int countArg, char* args[]) {
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
    wd::Log::error() << "Error";
    wd::Log::info() << "Info";

    std::string j = "";
    std::cin >> j;

    std::cout << "Null:\n";

    int* i = nullptr;
    std::cout << *i << "\n";

    system("read -N 1 -p 'Press any key to continue...'");

    return EXIT_SUCCESS;
}