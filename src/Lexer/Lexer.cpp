#include "Lexer/Lexer.h"

namespace wd {
    
    inline bool charIs(char c, std::string&& str) {
        return std::count(str.begin(), str.end(), c) != 0;
    }

    std::list<LexicalAnalyzer::Token> LexicalAnalyzer::analyz(std::string text, bool internalCall) {
        if (!internalCall)
            Log::begin() << "Launching the Lexical Analyzer"; 
        Log::debug() << "Length text:" << text.size();

        this->line = 1;
        this->row = 1;

        this->position = 0;
        this->text = text;
        this->size = text.size();

        std::list<LexicalAnalyzer::Token> tokens;
        while (position < size) {
            LexicalAnalyzer::Token token = separators();
            
            if (token.first.empty())
                break;
            
            tokens.push_back(token);
        }

        Log::debug() << "Length token list:" << tokens.size();
        Log::end() << "Lexical analysis completed";

        return tokens;
    }

    std::list<LexicalAnalyzer::Token> LexicalAnalyzer::operator()(std::filesystem::path path) {
        Log::begin() << "Launching the Lexical Analyzer"; 

        std::ifstream file(path);

        if (!file.is_open()) {
            Log::error() << "Failed to open file: '" << path << "'";
            return {};
        }

        std::string text = "", line;
        while (std::getline(file, line))
            text += line;

        file.close();
        return this->analyz(text, true);
    }
  

    void LexicalAnalyzer::config(void (*config)(LexicalAnalyzer::Config& self)) {
        config(this->c);
    }

    //--------------------------------------------------------------------------//

    LexicalAnalyzer::Token LexicalAnalyzer::separators() {
        while (charIs(getChar(0), std::move(c.separators)))
            next();
        
        if (getChar(0) == '\n') {
            this->line += 1;
            this->row = 0;
            next();
        }
        
        return keyWords();
    }

    LexicalAnalyzer::Token LexicalAnalyzer::keyWords() {
        unsigned long long position = this->position;

        std::string word = "";
        while (isalnum(getChar(0)))
            word += consume();

        for (auto keyWord : this->c.keyWords)
            if (word == keyWord.second)
                return keyWord;
            
        setPosition(position);
        return numbers();
    }

    LexicalAnalyzer::Token LexicalAnalyzer::numbers() { 
        std::string number = ""; 
        while (isdigit(getChar(0)) || getChar(0) == '.')
            number += consume();

        if (!number.empty())
            return LexicalAnalyzer::Token("number", number);

        Log::error() << "Unknown character: \"" << getChar(0) << "\" Line:" << this->line << "; Row:" << this->row;
        return { "",  "" };
    }

    //--------------------------------------------------------------------------//
    
    char LexicalAnalyzer::getChar(unsigned long long relativePosition) {
        unsigned long long p = this->position + relativePosition;
        if (p < size)
            return text[p];
        return '\0';
    }

    char LexicalAnalyzer::next() {
        this->row += 1;
        this->position += 1;
        return getChar(0);
    }

    
    char LexicalAnalyzer::consume() {
        next();
        return getChar(-1);
    }

    void LexicalAnalyzer::setPosition(unsigned long long position) {
        if (position > 0 && position < size)
            this->position = position;
        else this->position = size;
    } 
}