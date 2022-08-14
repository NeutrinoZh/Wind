#pragma once
#include "Tokenizer.h"

namespace EngineCore {
	namespace JText {
		struct Object {
			std::string name = "";

			std::map<std::string, Object*> children;

			Object() {};
			~Object() {
				for (auto object : children)
					delete object.second;
				children.clear();
			}
		protected:
			std::string value = "";
			static Object NONE;

			Object(std::string name, std::string value) :
				name(name), value(value) {};
			Object(std::string name) :
				Object(name, "") {};
			

			Object* copy();
			void inherit(Object* parent);
			void addChild(Object* object);

			friend class JText;
		public:
			bool is() {
				return !value.empty() || !children.empty();
			}

			int _int(int defaultValue) {
				if (value.empty())
					return defaultValue;
				return atoi(value.c_str());
			}
			uint8_t _uint8(uint8_t defaultValue) {
				return static_cast<uint8_t>(
					_int(
						static_cast<int>(defaultValue)
					)
				);
			}
			uint32_t _uint32(uint32_t defaultValue) {
				return static_cast<uint32_t>(
					_int(
						static_cast<int>(defaultValue)
					)
				);
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

			uint8_t _major(uint8_t defaultValue) {
				if (value.empty()) // && format("d.d")
					return defaultValue;
				return value[0] - 48;
			}
			uint8_t _minor(uint8_t defaultValue) {
				if (value.empty()) // && format("d.d")
					return defaultValue;
				return value[2] - 48;
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