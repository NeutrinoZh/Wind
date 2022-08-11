#include "Tokenizer.h"

namespace EngineCore {
	namespace JText {
		bool Tokenizer::ignore() {
			if (curr == '\n')
				line += 1;

			if (
				curr == ' ' ||
				curr == '\t' ||
				curr == '\n'
				) {
				next();
				return true;
			}

			return false;
		}
		bool Tokenizer::comment() {
			if (curr == '#') {
				while (curr != '\n' && curr != '\0')
					curr = next();
				return true;
			}

			return false;
		}
		bool Tokenizer::word() {
			std::string word = "";
			if (isalpha(curr) || curr == '_')
				while ((isalnum(curr) || curr == '_' || curr == '.') && curr != '\0') {
					word += curr;
					next();
				}

			if (word.empty())
				return false;

			if (word == "const")
				tokens.push_back({ TokenType::KEYWORD_CONST, word });
			else if (word == "link")
				tokens.push_back({ TokenType::KEYWORD_LINK, word });
			else if (word == "as")
				tokens.push_back({ TokenType::KEYWORD_AS, word });
			else if (word == "inherit")
				tokens.push_back({ TokenType::KEYWORD_INHERIT, word });
			else if (word == "true" || word == "false")
				tokens.push_back({ TokenType::STRING, word });
			else
				tokens.push_back({ TokenType::WORD, word });

			return true;
		}
		bool Tokenizer::number() {
			std::string number = "";

			if (isdigit(curr)) {
				bool point = false;
				while (isdigit(curr) || curr == '_' || (!point && curr == '.')) {
					if (!point)
						point = (curr == '.');

					number += curr;
					next();
				}
			}

			if (number.empty())
				return false;

			tokens.push_back({ TokenType::NUMBER, number });
			return true;
		}
		bool Tokenizer::operators() {
			TokenType type;
			switch (curr) {
				case '{':
					type = TokenType::L_CUR_BRACKET;
					break;
				case '}':
					type = TokenType::R_CUR_BRACKET;
					break;
				case '(':
					type = TokenType::L_BRACKET;
					break;
				case ')':
					type = TokenType::R_BRACKET;
					break;
				case '[':
					type = TokenType::L_SQRT_BRACKET;
					break;
				case ']':
					type = TokenType::R_SQRT_BRACKET;
					break;
				case '=':
					type = TokenType::EQ;
					break;
				case ',':
					type = TokenType::COMMA;
					break;
				case '%':
					type = TokenType::PERCENT;
					break;
			}

			if (charIs(curr, "{}[]()=,%")) {
				tokens.push_back({ type, std::string(1, curr) });
				next();
				return true;
			}

			return false;
		}
		bool Tokenizer::string() {
			std::string text = "";

			if (curr == '"' || curr == '\'') {
				char bracket = curr;
				next();

				while (curr != bracket) {
					text += curr;
					next();

					if (curr == '\0') {
						Log::error() << "LexicalAnalyzer. Missing closed bracket: " << bracket << "";
						return true;
					}
				}

				next();
			}

			if (text.empty())
				return false;

			tokens.push_back({ TokenType::STRING, text });
			return true;
		}

		std::vector<Token> Tokenizer::_tokenization() {
			curr = get();
			while (curr != '\0') {
				if (!(
					ignore() ||
					comment() ||
					word() ||
					number() ||
					operators() ||
					string()
					)) {
					Log::error() << "Couldn't analyz char: '" << curr << "' in line:" << line;
					return {};
				}
			}

			return tokens;
		}
		bool Tokenizer::tokenization(std::string path, std::vector<Token>& out) {
			Tokenizer tokenizer;

			if (!FileSystem::readFile(path, tokenizer.jtext))
				return false;

			out = tokenizer._tokenization();
			return true;
		}
	}
}