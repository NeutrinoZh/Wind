#pragma once
#include "LexicalAnalyzer.h"

namespace EngineCore {
	struct JObject {
		std::string name = "";
		unsigned int type = 0;

		enum {
			ARRAY,
			CONTAINER,
			NUMBER,
			STRING,
			PERCENT
		};

		static JObject* copy(JObject* org, std::string newName);

		JObject() {};
		JObject(std::string name, unsigned int type) :
			name(name), type(type) {};
	};

	struct JNumber : public JObject {
		float value;

		JNumber(std::string name, float value) :
			JObject(name, JObject::NUMBER), value(value) {};
	};

	struct JPercent : public JObject {
		float value;

		JPercent(std::string name, float value) :
			JObject(name, JObject::PERCENT), value(value) {};
	};

	struct JString : public JObject {
		std::string value;

		JString(std::string name, std::string value) :
			JObject(name, JObject::STRING), value(value) {};
	};

	struct JArray : public JObject {
		std::vector<JObject*> objects;

		JArray(std::string name) :
			JObject(name, JObject::ARRAY) {};
	};

	struct JContainer : public JObject {
		std::vector<JObject*> objects; // toMap!!

		JObject* findByName(std::string name) {
			for (JObject* object : objects)
				if (object->name == name)
					return object;
			return NULL;
		}

		void inherit(JContainer* container) {
			for (JObject* p_obj : container->objects) {
				JObject* c_obj = findByName(p_obj->name);

				if (!c_obj)
					this->objects.push_back(JObject::copy(p_obj, p_obj->name));
				else if (c_obj->type == JObject::CONTAINER && p_obj->type == JObject::CONTAINER)
					((JContainer*)c_obj)->inherit((JContainer*)p_obj);
			}
		}

		JContainer(std::string name) :
			JObject(name, JObject::CONTAINER) {};
	};

	class JText {
	private:
		std::vector<LexicalAnalyzer::Token> tokens;
		size_t position = 0;
		LexicalAnalyzer::Token curr;

		std::map<std::string, JObject*> consts = {};
	private:
		LexicalAnalyzer::Token get(size_t offset);
		LexicalAnalyzer::Token next();

		bool requestToken(LexicalAnalyzer::Token token);
	private:
		JObject* value(std::string name);
		JObject* variable();
		JObject* _const();
		JObject* link();

		JContainer* _parse();
	public:
		JContainer* root = NULL;
		static JText parse(std::string pathToFile);
	};
}