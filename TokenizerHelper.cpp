#include "Tokenizer.h"

namespace EngineCore {
	namespace JText {
		char Tokenizer::get(size_t offset) {
			size_t pos = this->position + offset;
			if (pos >= jtext.size())
				return '\0';
			return jtext[pos];
		}
		char Tokenizer::next() {
			this->position += 1;
			curr = get();
			return curr;
		}

		bool Tokenizer::charIs(char c, std::string str) {
			return std::count(str.begin(), str.end(), c) != 0;
		}
	}
}