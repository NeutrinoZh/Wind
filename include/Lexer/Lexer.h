#include "Log/Log.h"

namespace wd {
    
    class LexicalAnalyzer {
    public:
        
        typedef std::pair<std::string, std::string> Token;

        struct Type {
        private:
            std::string name;

            std::string prefix;
            std::string base;
            std::string postfix;

            bool single = false;
        
            friend class LexicalAnalyzer;
        public:
        
            Type(std::string name, std::string base) {
                this->single = true;
                this->name = name;
                this->base = base;
            }

            Type(std::string name, std::string prefix,
                 std::string base, std::string postfix) {
                this->name = name;
                this->prefix = prefix;
                this->base = base;
                this->postfix = postfix;
            }
        };

        
        struct Config {
            std::string separators = " \t ";
            
            std::vector<Type> types = {};
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

        std::vector<Type> possibles;
    public:
        std::list<Token> analyz(std::string text, bool internalCall=false);
        std::list<Token> operator()(std::filesystem::path file);

        void config(void (*config)(Config& self));
    private:
        Token separators();
        Token comments();
        Token keyWords();

        Token prefix();
        Token base();
        Token postfix(std::string value);

        Token unknown();
    private:
        char getChar(unsigned long long relativePosition);
        char next();
        char consume();

        void rememberPosition();
        void memorizedPosition();
    };

}