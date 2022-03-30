#include "Lexer/Lexer.h"

int main(int countArg, char* args[]) {
    system("read -N 1 -p 'Press any key to continue...\n'");

    Log::config([](auto& self) {
        self.outConsole = true;
        self.folderToSave = "./logs/";
        self.numSpace = 2;
    });
    
    wd::LexicalAnalyzer analyzer;

    analyzer.config([](auto& self) {
        self.keyWords = { { "Yeah!", "Happy" } };
    });

    auto tokens = analyzer("./asset/text.txt");
    
    for (auto token : tokens)
        Log::info() << "Token ( type: " << token.first << "; value: " << token.second << "; )";

    system("read -N 1 -p 'Press any key to continue...'");
    return EXIT_SUCCESS;
}