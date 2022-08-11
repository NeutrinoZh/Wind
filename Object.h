#pragma once
#include "Tokenizer.h"

namespace EngineCore {
	namespace JText {
		struct Object {
			std::string name = "";

			std::map<std::string, Object*> children;
		protected:
			std::string value = "";
			static Object NONE;

			Object() {};
			Object(std::string name, std::string value) :
				name(name), value(value) {};
			Object(std::string name) :
				Object(name, "") {};
			~Object() {
				for (auto object : children)
					delete object.second;
				children.clear();
			}

			Object* copy();
			void inherit(Object* parent);
			void addChild(Object* object);

			friend class JText;
		public:
			int _int(int defaultValue) {
				if (value.empty())
					return defaultValue;
				return atoi(value.c_str());
			}
			float _float(float defaultValue) {
				if (value.empty())
					return defaultValue;
				return static_cast<float>(atof(value.c_str()));
			}
			double _double(double defaultValue) {
				if (value.empty())
					return defaultValue;
				return atof(value.c_str());
			}
			std::string _str(std::string defaultValue) {
				if (value.empty())
					return defaultValue;
				return value;
			}
			bool _bool(bool defaultValue) {
				if (value.empty())
					return defaultValue;
				return (value == "true" || value == "false");
			}

			Object& operator[](std::string name) {
				auto obj = children.find(name);
				if (obj == children.end())
					return Object::NONE;
				return *obj->second;
			}

			Object& operator[](int i) {
				return this->operator[](std::to_string(i));
			}
		};
	}
}