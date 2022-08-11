#pragma once
#include "Object.h"

namespace EngineCore {
	namespace JText {
		class JText {
		private:
			std::vector<Token> tokens;
			size_t position = 0;
			Token curr;

			std::map<std::string, Object*> consts = {};
		private:
			Token get(size_t offset=0);
			Token next();

			bool requestToken(Token token);
			bool requestTokenType(TokenType type);
		private:
			Object* array(std::string name);
			Object* container(std::string name);
			Object* value_const(std::string name);
			Object* value(std::string name);

			Object* variable();
			Object* _const();
			Object* link();

			Object* _parse();
			static JText parse(std::string pathToFile);
			friend JText parse(std::string pathToFile);
		public:
			Object* root = NULL;
		};

		JText parse(std::string pathToFile);
	}
}