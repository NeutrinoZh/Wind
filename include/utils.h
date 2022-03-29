#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <map>
#include <Windows.h>
#include <time.h>


/*
     wd::LexicalAnalyzer analyzer;

    analyzer.config([](auto& self) {
        self.keyWords = { { "Yeah!", "Happy" } };
    });

    auto tokens = analyzer.analyz("Happy 32 Happy 65 Happy 89 Happy91 Happy");
    std::cout << "Count token:" << tokens.size() << "\n";
    for (auto token : tokens)
        std::cout << "Token ( type: " << token.first << "; value: " << token.second << "; )\n";
*/