#include "Tokenizer.h"

namespace EngineCore {
	namespace JText {
		std::string Tokenizer::fileRead(std::string pathToFile) {
			std::ifstream file(pathToFile);

			if (!file.is_open()) {
				Log::error() << "LexicalAnalyzer: Couldn't open JT file:" << pathToFile;
				return "";
			}

			std::string text = "", line = "";
			while (std::getline(file, line)) {
				text += line + "\n";
			}

			file.close();
			return text;
		}
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