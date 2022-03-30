#include "Log/Log.h"

namespace wd {
    
    class LexicalAnalyzer {
    public:
        
        typedef std::pair<std::string, std::string> Token;
        struct Config {
            std::string separators = " \t";
            std::vector<std::pair<std::string, std::string>> keyWords = {};
        };

    private:
    
        Config c;

        unsigned long long line;
        unsigned long long row;

        unsigned long long position;
        unsigned long long size;
        
        unsigned char current = '\0';
        std::string text = "";

    public:
        std::list<Token> analyz(std::string text, bool internalCall=false);
        std::list<Token> operator()(std::filesystem::path file);

        void config(void (*config)(Config& self));
    private:
        Token separators();
        Token keyWords();
        Token numbers();
    private:
        char getChar(unsigned long long relativePosition);
        char next();
        char consume();

        void setPosition(unsigned long long position);
    };

}