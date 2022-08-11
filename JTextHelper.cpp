#include "JText.h"

namespace EngineCore {
	namespace JText {
		Token JText::get(size_t offset) {
			size_t pos = this->position + offset;
			if (pos >= tokens.size())
				return { TokenType::T_EOF, "" };
			return tokens[pos];
		}
		Token JText::next() {
			position += 1;
			curr = get();
			return curr;
		}
		bool JText::requestToken(Token token) {
			if (curr.first != token.first || curr.second != token.second) {
				Log::error() << "JText. Required '" << token.second << "' but is '" << curr.second << "'";
				return false;
			}

			next();
			return true;
		}
		bool JText::requestTokenType(TokenType type) {
			if (curr.first != type) {
				Log::error() << "JText. Required type '" << (int)type << "' but is " << (int)curr.first << "(" << curr.second << ")";
				return false;
			}

			next();
			return true;
		}
	}
}