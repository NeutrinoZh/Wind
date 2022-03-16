#include "utils.h"

namespace wd {
    
    class LexicalAnalyzer {
    public:
        typedef std::pair<std::string, std::string> Token;
        struct Config {
            std::vector<std::pair<std::string, std::string>> keyWords = {};
        } c;
    private:
        unsigned long long position = 0;
        unsigned long long size = 0;
        unsigned char current = '\0';
        std::string text = "";
    public:
        std::list<Token> analyz(std::string text);
        std::list<Token> operator()(std::filesystem::path file);

        void config(void (*config)(Config& self));
    private:
        Token keyWords();
    private:
        char getChar(unsigned long long relativePosition);
        char next();
        char consume();
    };

}