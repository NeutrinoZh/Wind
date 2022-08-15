#include "JText.h"

namespace WindEngine {
	namespace JText {
		Object* JText::array(std::string name) {
			next();

			Object* array = new Object(name);
			unsigned int i = 0;

			while (curr.first != TokenType::R_SQRT_BRACKET) {
				if (i != 0 && !requestTokenType(TokenType::COMMA))
					return NULL;

				Object* obj = value(std::to_string(i));
				if (!obj)
					return NULL;

				array->addChild(obj);

				i += 1;
				next();
			}

			return array;
		}
		Object* JText::container(std::string name) {
			next();

			Object* container = new Object(name);
			while (curr.first != TokenType::R_CUR_BRACKET) {
				Object* obj = variable();
				if (!obj)
					return NULL;

				container->addChild(obj);
				next();
			}

			return container;
		}
		Object* JText::value_const(std::string name) {
			auto c = consts.find(curr.second);
			if (c == consts.end()) {
				Log::error() << "JText. No constant named: " << curr.second;
				return NULL;
			}

			Object* _copy = c->second->copy();
			_copy->name = name;

			return _copy;
		}

		Object* JText::value(std::string name) {
			if (curr.first == TokenType::WORD)
				return value_const(name);
			else if (curr.first == TokenType::L_SQRT_BRACKET)
				return array(name);
			else if (curr.first == TokenType::L_CUR_BRACKET)
				return container(name);
			else if (
				curr.first == TokenType::NUMBER ||
				curr.first == TokenType::STRING
			)
				return new Object(name, curr.second);

			Log::error() << "JText. '" << curr.second << "' not a variable type";
			return NULL;
		}
		Object* JText::variable() {
			if (curr.first == TokenType::WORD) {
				std::string name;

				name = curr.second;
				next();

				if (curr.first == TokenType::L_CUR_BRACKET)
					return value(name);
				else if (curr.first == TokenType::KEYWORD_INHERIT) {
					next();

					if (curr.first != TokenType::WORD) {
						Log::error() << "JText. '" << curr.second << "' not an object to inherit";
						return NULL;
					}

					auto parent = consts.find(curr.second);
					if (parent == consts.end()) {
						Log::error() << "JText. Don't exists const to inherit by name: '" << curr.second << "'";
						return NULL;
					}
					next();

					if (curr.second != "{") {
						Log::error() << "JText. Only containers can use inheritance: '" << parent->second->name << "'";
						return NULL;
					}

					Object* object = value(name);
					if (object)
						object->inherit(parent->second);
					return object;
				} else if (!requestTokenType(TokenType::EQ))
					return NULL;

				return value(name);
			}

			Log::error() << "JText. '" << curr.second << "' not a definition";
			return NULL;
		}
		Object* JText::_const() {
			if (curr.first == TokenType::KEYWORD_CONST) {
				next();

				Object* obj = variable();
				if (consts.find(obj->name) != consts.end()) {
					Log::error() << "JText. Const with this name already exists:" << obj->name;
					return NULL;
				}

				consts.insert(std::make_pair(obj->name, obj));
				return obj;
			}

			return variable();
		}
		Object* JText::link() {
			if (curr.first == TokenType::KEYWORD_LINK) {
				next();

				std::string path, alias;

				if (curr.first != TokenType::STRING) {
					Log::error() << "JText. Link to JT File should be string";
					return NULL;
				}
				path = curr.second;
				next();

				if (!requestToken({ TokenType::KEYWORD_AS, "as" }))
					return NULL;

				if (curr.first != TokenType::WORD) {
					Log::error() << "JText. Alias JT File should be word";
					return NULL;
				}
				alias = curr.second;
				next();

				Log::info() << "JText. Link " << path << " as " << alias;

				JText jtext;
				if (!JText::parse(path, jtext)) {
					Log::error() << "JText. Couldn't link:" << path;
					return NULL;
				}

				auto link_consts = jtext.consts;
				for (auto _const : link_consts)
					consts.insert(std::make_pair(
						alias + "." + _const.first,
						_const.second
					));

				return link();
			}

			return _const();
		}

		Object* JText::_parse() {
			Object* root = new Object("root");

			curr = get();
			while (curr.first != TokenType::T_EOF) {
				Object* obj = link();

				if (obj)
					root->addChild(obj);
				else
					break;

				next();
			}

			Log::info() << "JText. Finish parsing";
			return root;
		}
		bool JText::parse(std::string pathToFile, JText& out) {
			JText jtext;
			
			if (!Tokenizer::tokenization(pathToFile, jtext.tokens))
				return false;

			Log::info() << "JText. Finish tokenization"; // !

			jtext.root = jtext._parse();

			out = jtext;
			return true;
		}
		bool parse(std::string pathToFile, Object& out) {
			JText jtext;
			
			if (!JText::parse(pathToFile, jtext))
				return false;

			out = *jtext.root;
			return true;
		}
	}
}