#include "LexicalAnalyzer.h"

namespace EngineCore {
	std::string LexicalAnalyzer::fileRead(std::string pathToFile) {
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
	char LexicalAnalyzer::get(size_t offset = 0) {
		size_t pos = this->position + offset;
		if (pos >= jtext.size())
			return '\0';
		return jtext[pos];
	}
	char LexicalAnalyzer::next() {
		this->position += 1;
		curr = get();
		return curr;
	}

	bool LexicalAnalyzer::charIs(char c, std::string str) {
		return std::count(str.begin(), str.end(), c) != 0;
	}

	bool LexicalAnalyzer::ignore() {
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
	bool LexicalAnalyzer::comment() {
		if (curr == '#') {
			while (curr != '\n' && curr != '\0')
				curr = next();
			return true;
		}

		return false;
	}
	bool LexicalAnalyzer::word() {
		std::string word = "";
		if (isalpha(curr) || curr == '_')
			while ((isalnum(curr) || curr == '_' || curr == '.') && curr != '\0') {
				word += curr;
				next();
			}

		if (word.empty())
			return false;

		if (word == "const")
			tokens.push_back({ KEYWORD_CONST, word });
		else if (word == "link")
			tokens.push_back({ KEYWORD_LINK, word });
		else if (word == "as")
			tokens.push_back({ KEYWORD_AS, word });
		else if (word == "inherit")
			tokens.push_back({ KEYWORD_INHERIT, word });
		else
			tokens.push_back({ WORD, word });

		return true;
	}
	bool LexicalAnalyzer::number() {
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

		tokens.push_back({ NUMBER, number });
		return true;
	}
	bool LexicalAnalyzer::operators() {
		std::string op = "";
		if (charIs(curr, "{}[]()=,%")) {
			op += curr;
			next();
		}

		if (op.empty())
			return false;

		tokens.push_back({ OPERATOR, op });
		return true;
	}
	bool LexicalAnalyzer::string() {
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

		tokens.push_back({ STRING, text });
		return true;
	}

	std::vector<LexicalAnalyzer::Token> LexicalAnalyzer::_analyz() {
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
	std::vector<LexicalAnalyzer::Token> LexicalAnalyzer::analyz(std::string path) {
		LexicalAnalyzer analyzer;

		analyzer.jtext = analyzer.fileRead(path);
		if (analyzer.jtext == "")
			return {};

		return analyzer._analyz();
	}
}#include "LexicalAnalyzer.h"

namespace EngineCore {
	std::string LexicalAnalyzer::fileRead(std::string pathToFile) {
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
	char LexicalAnalyzer::get(size_t offset = 0) {
		size_t pos = this->position + offset;
		if (pos >= jtext.size())
			return '\0';
		return jtext[pos];
	}
	char LexicalAnalyzer::next() {
		this->position += 1;
		curr = get();
		return curr;
	}

	bool LexicalAnalyzer::charIs(char c, std::string str) {
		return std::count(str.begin(), str.end(), c) != 0;
	}

	bool LexicalAnalyzer::ignore() {
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
	bool LexicalAnalyzer::comment() {
		if (curr == '#') {
			while (curr != '\n' && curr != '\0')
				curr = next();
			return true;
		}

		return false;
	}
	bool LexicalAnalyzer::word() {
		std::string word = "";
		if (isalpha(curr) || curr == '_')
			while ((isalnum(curr) || curr == '_' || curr == '.') && curr != '\0') {
				word += curr;
				next();
			}

		if (word.empty())
			return false;

		if (word == "const")
			tokens.push_back({ KEYWORD_CONST, word });
		else if (word == "link")
			tokens.push_back({ KEYWORD_LINK, word });
		else if (word == "as")
			tokens.push_back({ KEYWORD_AS, word });
		else if (word == "inherit")
			tokens.push_back({ KEYWORD_INHERIT, word });
		else
			tokens.push_back({ WORD, word });

		return true;
	}
	bool LexicalAnalyzer::number() {
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

		tokens.push_back({ NUMBER, number });
		return true;
	}
	bool LexicalAnalyzer::operators() {
		std::string op = "";
		if (charIs(curr, "{}[]()=,%")) {
			op += curr;
			next();
		}

		if (op.empty())
			return false;

		tokens.push_back({ OPERATOR, op });
		return true;
	}
	bool LexicalAnalyzer::string() {
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

		tokens.push_back({ STRING, text });
		return true;
	}

	std::vector<LexicalAnalyzer::Token> LexicalAnalyzer::_analyz() {
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
	std::vector<LexicalAnalyzer::Token> LexicalAnalyzer::analyz(std::string path) {
		LexicalAnalyzer analyzer;

		analyzer.jtext = analyzer.fileRead(path);
		if (analyzer.jtext == "")
			return {};

		return analyzer._analyz();
	}
}