#pragma once
#include "Log.h"

namespace EngineCore {
	class LexicalAnalyzer {
	public:
		typedef std::pair<unsigned int, std::string> Token;

		enum {
			T_EOF,
			WORD,
			NUMBER,
			OPERATOR,
			STRING,

			KEYWORD_CONST,
			KEYWORD_LINK,
			KEYWORD_AS,
			KEYWORD_INHERIT
		};
	private:
		std::vector<Token> tokens;

		size_t position = 0, line = 1;
		std::string jtext = "";
		char curr = '\0';
	private:
		std::string fileRead(std::string pathToFile);

		char next();
		char get(size_t offset);

		bool charIs(char c, std::string str);
	private:
		bool ignore();
		bool comment();
		bool word();
		bool number();
		bool operators();
		bool string();

		std::vector<Token> _analyz();
	public:
		static std::vector<Token> analyz(std::string pathToFile);
	};
}