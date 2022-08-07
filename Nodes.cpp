#include "Nodes.h"

namespace EngineCore {
	Nodes* Nodes::nodes = new Nodes("nodes");

	Config Nodes::load(std::string path) {
		Log::info() << "Load node:" << path;

		Config node = ConfigReader::read(path);
		
		if (node.isVar("name"))
			this->add(node.getStringValue("name"), node);
		else
			Log::error() << "Couldn't load node: missing node name";

		return node;
	}

	Nodes& nodes() {
		return *Nodes::nodes;
	}
}