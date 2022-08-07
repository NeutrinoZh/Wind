#pragma once
#include "TileMap.h"

namespace EngineCore {
	class Behavior;

	class Node {
	protected:
		std::vector<Node*> children;
		std::vector<Behavior*> behavior;
	public:
		std::string name = "Node";
		bool active = true;
	public:
		void addChild(Node* child, bool runtime=true);
		void addBehavior(Behavior* behavior);

		void start();
		void update();
		void free();
	};
}