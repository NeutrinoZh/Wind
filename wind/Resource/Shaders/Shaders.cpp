#include "Shaders.h"

namespace WindEngine {
	Shaders* Shaders::shaders = new Shaders("shaders");

	void Shaders::load_o(JText::Object& obj_shader) {
		struct _ {
			std::string vtx = "", fgt = "";
			std::string name = "";

			_(JText::Object& shader) {
				name = shader["name"]._str(name);
				vtx = shader["vtx"]._str(vtx);
				fgt = shader["fgt"]._str(fgt);
			}
		} meta(obj_shader);

		Shader* shader = new Shader();
		shader->create(meta.vtx, meta.fgt);
		this->add(meta.name, shader);
	}

	void Shaders::free(Shader* shader) {
		delete shader;
	}

	Shaders& shaders() {
		return *Shaders::shaders;
	}
}