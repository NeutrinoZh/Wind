#include "Shaders.h"

namespace EngineCore {
	Shaders* Shaders::shaders = new Shaders("shaders");

	Shader* Shaders::load(std::string path) {
		struct _meta {
			std::string vtx = "", fgt = "";
			std::string name = "";

			_meta(std::string path) {
				Log::info() << "Load metadata shader: " << path;

				Config config = ConfigReader::read(path);

				if (config.isVar("vtx")) vtx = config.getStringValue("vtx");
				if (config.isVar("fgt")) fgt = config.getStringValue("fgt");
				if (config.isVar("name")) name = config.getStringValue("name");
			}
		} meta(path);

		Shader* shader = new Shader();
		shader->create(meta.vtx, meta.fgt);
		this->add(meta.name, shader);
		return shader;
	}

	void Shaders::free(Shader* shader) {
		delete shader;
	}

	Shaders& shaders() {
		return *Shaders::shaders;
	}
}