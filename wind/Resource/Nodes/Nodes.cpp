#include "Nodes.h"

namespace WindEngine {
	Nodes* Nodes::nodes = new Nodes("nodes");

	void Nodes::load_o(JText::Object& obj_node) {
		std::string name = obj_node["name"]._str("none");

		Log::info() << "Load node:" << name;
		
		this->add(name, obj_node);
	}

	Nodes& nodes() {
		return *Nodes::nodes;
	}
}