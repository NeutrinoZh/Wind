#include "Lexer/Lexer.h"

namespace wd {
    
    inline bool charIs(char c, std::string&& str) {
        return std::count(str.begin(), str.end(), c) != 0;
    }

    void LexicalAnalyzer::config(void (*config)(LexicalAnalyzer::Config& self)) {
        config(this->c);
    }

    std::list<LexicalAnalyzer::Token> LexicalAnalyzer::operator()(std::filesystem::path path) {
        Log::begin() << "Launching the Lexical Analyzer"; 

        std::ifstream file(path);

        if (!file.is_open()) {
            Log::error() << "Failed to open file: '" << path << "'";
            return {};
        }

        std::string text = "", line;
        char last;
        while (std::getline(file, line)) {
            text += line.substr(0, line.size() - 1) + "\n";
            last = *(line.end() - 1);
        }
        text.erase(text.end() - 1);
        text += last;

        file.close();
        return this->analyz(text, true);
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
        this->memo = 0;

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

    //--------------------------------------------------------------------------//

    LexicalAnalyzer::Token LexicalAnalyzer::separators() {
        while (charIs(getChar(0), std::move(c.separators)))
            next();
        
        while (getChar(0) == '\n') {
            this->line += 1;
            this->row = 0;
            next();
        }
        
        if (getChar(0) == '\0')
            return { "", "" };

        return keyWords();
    }

    LexicalAnalyzer::Token LexicalAnalyzer::comments() {
        if (getChar(0) == '#') {
            while (getChar(0) != '\n')
                next();
            return separators();
        }

        return keyWords();
    }

    LexicalAnalyzer::Token LexicalAnalyzer::keyWords() {
        rememberPosition();

        std::string word = "";
        while (!charIs(getChar(0), std::move(c.separators)))
            word += consume();

        for (auto keyWord : this->c.keyWords)
            if (word == keyWord.second)
                return keyWord;
    
        memorizedPosition();
        return prefix();
    }

    LexicalAnalyzer::Token LexicalAnalyzer::prefix() {
        this->possibles = {};

        Log::debug() << "START PREFIX";

        for (Type type : c.types)
            if (!type.prefix.empty()) {
                
                char current = getChar(0);

                if (
                    (type.prefix == "number" && (isdigit(current) || current == '-')) ||
                    (type.prefix == "letter" && isalpha(current)) ||
                    (type.prefix == "letter_and_number" && isalnum(current)) ||
                    (type.prefix == "x-number" && isxdigit(current)) ||
                    (type.prefix == "name" && (isalpha(current) || current == '_')) ||
                    (type.prefix == "name_and_number" && (isalpha(current) || current == '_'))
                ) {
                    if (type.single) {
                        std::string value = "";
                        value += consume();
                        return LexicalAnalyzer::Token(type.name, value);
                    }

                    this->possibles.push_back(type); 
                }

            }

        Log::debug() << "END PREFIX";

        if (this->possibles.size() > 0) 
            return base();

        Log::error() << "Not one possible type found by prefix";
        return { "", "" };
    }

    LexicalAnalyzer::Token LexicalAnalyzer::base() {
        
        auto isBasic = [](char current, std::vector<Type>* possible) {
            for (Type type : *possible)
                if (
                    (type.base == "number" && (isdigit(current) || current == '-')) ||
                    (type.base == "letter" && isalpha(current)) ||
                    (type.base == "letter_and_number" && isalnum(current)) ||
                    (type.base == "x-number" && isxdigit(current)) ||
                    (type.base == "name" && (isalpha(current) || current == '_')) ||
                    (type.base == "name_and_number" && (isalnum(current) || current == '_')) ||
                    (type.base == "all")
                ) return true;
            return false;
        };

        auto removeUnsuitable = [](char current, std::vector<Type>* possible) {
            unsigned int num = 0;

            for (Type type : *possible) {
                if (type.base == "all") continue;
                else if (type.base == "number") num += !(isdigit(current) || current == '.');
                else if (type.base == "letter") num += !isalpha(current);
                else if (type.base == "letter_and_number") num += !isalnum(current);
                else if (type.base == "x-number") num += !isxdigit(current);
                else if (type.base == "name") num += !(isalpha(current) || current == '_');
                else if (type.base == "name_and_number") num += !(isalnum(current) || current == '_');
            }

            if (num != possible->size())
                for (unsigned int i = 0; i < possible->size(); ++i) {
                    std::string base = possible->at(i).base;
                    
                    if (
                        (base == "number" && (!isdigit(current) || current == '.')) ||
                        (base == "letter" && !isalpha(current)) ||
                        (base == "letter_and_number" && !isalnum(current)) ||
                        (base == "x-number" && !isxdigit(current)) ||
                        (base == "name" && !(isalpha(current) || current == '_')) ||
                        (base == "name_and_number" && !(isalnum(current) || current == '_'))
                    ) {
                            possible->erase(possible->begin() + i);
                    }
                }
        };

        std::string value = "";

        while (isBasic(getChar(0), &this->possibles)) {
            Log::debug() << "BASE:" << value;
            
            value += getChar(0);
            removeUnsuitable(getChar(0), &this->possibles);

            next();
        }

        Log::debug() << "VALUE:" << value << "Types:" << this->possibles[0].name;

        return postfix(value);
    }

    LexicalAnalyzer::Token LexicalAnalyzer::postfix(std::string value) {
        return { "", "" };
    }

    LexicalAnalyzer::Token LexicalAnalyzer::unknown() {
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

    void LexicalAnalyzer::rememberPosition() {
        this->memo = this->position;
        this->memoLine = this->line;
        this->memoRow = this->row;
    } 

    void LexicalAnalyzer::memorizedPosition() {
        this->position = this->memo;
        this->line = this->memoLine;
        this->row = this->memoRow;
    }
}