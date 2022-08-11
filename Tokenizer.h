#pragma once
#include "Token.h"

namespace EngineCore {
	namespace JText {
		class Tokenizer {
		private:
			std::vector<Token> tokens;

			size_t position = 0, line = 1;
			std::string jtext = "";
			char curr = '\0';
		private:
			std::string fileRead(std::string pathToFile);

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
			static std::vector<Token> tokenization(std::string pathToFile);
		};
	}
}