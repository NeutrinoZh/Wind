#include "JText.h"

namespace EngineCore {
	LexicalAnalyzer::Token JText::get(size_t offset = 0) {
		size_t pos = this->position + offset;
		if (pos >= tokens.size())
			return { LexicalAnalyzer::T_EOF, "" };
		return tokens[pos];
	}
	LexicalAnalyzer::Token JText::next() {
		position += 1;
		curr = get();
		return curr;
	}
	bool JText::requestToken(LexicalAnalyzer::Token token) {
		if (curr.first != token.first || curr.second != token.second) {
			Log::error() << "JText. Required '" << token.second << "' but is '" << curr.second << "'";
			return false;
		}

		next();
		return true;
	}

	JContainer* JText::_parse() {
		JContainer* root = new JContainer("root");

		curr = get();
		while (curr.first != LexicalAnalyzer::T_EOF) {
			JObject* obj = link();

			if (obj)
				root->objects.push_back(obj);
			else
				break;

			next();
		}

		Log::info() << "JText. Finish parsing";
		return root;
	}

	JObject* JText::value(std::string name) {
		if (curr.first == LexicalAnalyzer::WORD) {
			auto c = consts.find(curr.second);
			if (c == consts.end()) {
				Log::error() << "JText. No constant named: " << curr.second;
				return NULL;
			}
			return JObject::copy(c->second, name);
		}
		else if (curr.first == LexicalAnalyzer::NUMBER) {
			float number = (float)atof(curr.second.c_str());

			if (get(1).second == "%") {
				next();
				return new JPercent(name, number);
			}

			return new JNumber(name, number);
		}
		else if (curr.first == LexicalAnalyzer::STRING) {
			return new JString(name, curr.second);
		}
		else if (curr.second == "[") {
			next();

			JArray* array = new JArray(name);
			unsigned int i = 0;

			while (curr.second != "]") {
				if (i != 0 && !requestToken({ LexicalAnalyzer::OPERATOR, "," }))
					return NULL;

				JObject* obj = value(std::to_string(i));
				if (!obj)
					return NULL;

				array->objects.push_back(obj);

				i += 1;
				next();
			}

			return array;
		}
		else if (curr.second == "{") {
			next();

			JContainer* container = new JContainer(name);
			while (curr.second != "}") {
				JObject* obj = variable();
				if (!obj)
					return NULL;

				container->objects.push_back(obj);
				next();
			}

			return container;
		}

		Log::error() << "JText. '" << curr.second << "' not a variable type";
		return NULL;
	}

	JObject* JText::variable() {
		if (curr.first == LexicalAnalyzer::WORD) {
			std::string name;

			name = curr.second;
			next();

			if (curr.second == "{")
				return value(name);
			else if (curr.first == LexicalAnalyzer::KEYWORD_INHERIT) {
				next();

				if (curr.first != LexicalAnalyzer::WORD) {
					Log::error() << "JText. '" << curr.second << "' not an object to inherit";
					return NULL;
				}

				auto parent = consts.find(curr.second);
				if (parent == consts.end()) {
					Log::error() << "JText. Don't exists const to inherit by name: '" << curr.second << "'";
					return NULL;
				}
				next();

				if (curr.second != "{" || parent->second->type != JObject::CONTAINER) {
					Log::error() << "JText. Only containers can use inheritance: '" << parent->second->name << "'";
					return NULL;
				}

				JContainer* object = (JContainer*)value(name);
				if (object)
					object->inherit((JContainer*)parent->second);
				return object;
			}
			else if (!requestToken({ LexicalAnalyzer::OPERATOR, "=" }))
				return NULL;

			return value(name);
		}

		Log::error() << "JText. '" << curr.second << "' not a definition";
		return NULL;
	}

	JObject* JText::_const() {
		if (curr.first == LexicalAnalyzer::KEYWORD_CONST) {
			next();

			JObject* obj = variable();
			if (consts.find(obj->name) != consts.end()) {
				Log::error() << "JText. Const with this name already exists:" << obj->name;
				return NULL;
			}

			consts.insert(std::make_pair(obj->name, obj));
			return obj;
		}

		return variable();
	}

	JObject* JText::link() {
		if (curr.first == LexicalAnalyzer::KEYWORD_LINK) {
			next();

			std::string path, alias;

			if (curr.first != LexicalAnalyzer::STRING) {
				Log::error() << "JText. Link to JT File should be string";
				return NULL;
			}
			path = curr.second;
			next();

			if (!requestToken({ LexicalAnalyzer::KEYWORD_AS, "as" }))
				return NULL;

			if (curr.first != LexicalAnalyzer::WORD) {
				Log::error() << "JText. Alias JT File should be word";
				return NULL;
			}
			alias = curr.second;
			next();

			Log::info() << "JText. Link " << path << " as " << alias;

			auto link_consts = JText::parse(path).consts;
			for (auto _const : link_consts)
				consts.insert(std::make_pair(
					alias + "." + _const.first,
					_const.second
				));

			return link();
		}

		return _const();
	}

	JText JText::parse(std::string pathToFile) {
		JText jtext;
		jtext.tokens = LexicalAnalyzer::analyz(pathToFile);

		Log::info() << "JText. Finish lexical analyz";

		jtext.root = jtext._parse();
		return jtext;
	}

	JObject* JObject::copy(JObject* org, std::string name) {
		JObject* c = NULL;

		if (org->type == JObject::NUMBER)
			c = new JNumber(name, ((JNumber*)org)->value);
		else if (org->type == JObject::PERCENT)
			c = new JPercent(name, ((JPercent*)org)->value);
		else if (org->type == JObject::STRING)
			c = new JString(name, ((JString*)org)->value);
		else if (org->type == JObject::ARRAY) {
			c = new JArray(name);
			((JArray*)c)->objects = ((JArray*)org)->objects;
		}
		else if (org->type == JObject::CONTAINER) {
			c = new JContainer(name);
			((JContainer*)c)->objects = ((JContainer*)org)->objects;
		}

		return c;
	}
}