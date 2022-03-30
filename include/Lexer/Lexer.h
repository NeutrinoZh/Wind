#include "Log/Log.h"

namespace wd {
    
    class LexicalAnalyzer {
    public:
        
        typedef std::pair<std::string, std::string> Token;
        struct Config {
            std::string separators = " \t ";
            std::vector<std::pair<std::string, std::string>> keyWords = {};
        };

    private:
    
        Config c;

        unsigned long long line;
        unsigned long long row;

        unsigned long long position;
        unsigned long long size;

        unsigned long long memo;
        unsigned long long memoLine;
        unsigned long long memoRow;

        std::string text = "";
    public:
        std::list<Token> analyz(std::string text, bool internalCall=false);
        std::list<Token> operator()(std::filesystem::path file);

        void config(void (*config)(Config& self));
    private:
        Token separators();
        Token keyWords();
        Token words();
        Token numbers();
        Token unknown();
    private:
        char getChar(unsigned long long relativePosition);
        char next();
        char consume();

        void rememberPosition();
        void memorizedPosition();
    };

}