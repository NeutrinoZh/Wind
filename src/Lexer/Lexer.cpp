#include "Lexer/Lexer.h"

namespace wd {
    
    std::list<LexicalAnalyzer::Token> LexicalAnalyzer::analyz(std::string text) {
        this->text = text;
        this->position = 0;
        this->size = text.size();

        std::list<LexicalAnalyzer::Token> tokens;
        while (position < size) {
            LexicalAnalyzer::Token token = keyWords();
            
            if (token.first.empty())
                break;
            
            tokens.push_back(token);
        }

        return tokens;
    }

    std::list<LexicalAnalyzer::Token> LexicalAnalyzer::operator()(std::filesystem::path path) {
        std::ifstream file(path);

        if (!file.is_open())
            return {};

        std::string text = "", line;
        while (std::getline(file, line))
            text += line;

        file.close();
        return this->analyz(text);
    }
  

    void LexicalAnalyzer::config(void (*config)(LexicalAnalyzer::Config& self)) {
        config(this->c);
    }

    //--------------------------------------------------------------------------//

    LexicalAnalyzer::Token LexicalAnalyzer::keyWords() {
        std::string word = "";
        while (isalnum(getChar(0)))
            word += consume();

        for (auto keyWord : this->c.keyWords)
            if (word == keyWord.second)
                return keyWord;
            
        return {"", ""};
    }


    //--------------------------------------------------------------------------//
    
    char LexicalAnalyzer::getChar(unsigned long long relativePosition) {
        unsigned long long p = this->position + relativePosition;
        if (p < size)
            return text[p];
        return '\0';
    }

    char LexicalAnalyzer::next() {
        return getChar(this->position++);
    }

    
    char LexicalAnalyzer::consume() {
        next();
        return getChar(-1);
    }
}