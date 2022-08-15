#pragma once
#include "Token.h"

namespace WindEngine {
	namespace JText {
		class Tokenizer {
		private:
			std::vector<Token> tokens;

			size_t position = 0, line = 1;
			std::string jtext = "";
			char curr = '\0';
		private:
			char next();
			char get(size_t offset=0);

			bool charIs(char c, std::string str);
		private:
			bool ignore();
			bool comment();
			bool word();
			bool number();
			bool operators();
			bool string();

			std::vector<Token> _tokenization();
		public:
			static bool tokenization(std::string pathToFile, std::vector<Token>& out);
		};
	}
}