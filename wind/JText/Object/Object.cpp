#include "Object.h"

namespace WindEngine {
	namespace JText {
		Object Object::NONE = Object("", "");

		// Add child object with pair { object->name, object }
		void Object::addChild(Object* object) {
			this->children.insert(
				std::make_pair(
					object->name,
					object
				)
			);
		}

		// Container inherit (only for containers!)
		void Object::inherit(Object* parent) {
			for (auto p_obj : parent->children) {
				auto c_obj = this->children.find(p_obj.first);

				if (c_obj == this->children.end())
					this->addChild(p_obj.second->copy());
				else 
					c_obj->second->inherit(p_obj.second);
			}
		}

		// Creates a depth copy of an object
		Object* Object::copy() {
			Object* _copy = new Object(this->name, this->value);
			for (auto child : this->children)
				_copy->children.insert(
					std::make_pair(
						child.first,
						child.second->copy()
					)
				);
			return _copy;
		}
	}
}