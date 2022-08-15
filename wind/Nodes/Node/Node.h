#pragma once
#include "../../Graphics/TileMap/TileMap.h"

namespace WindEngine {
	class Behavior;

	class Node {
	protected:
		std::vector<Node*> children;
		std::vector<Behavior*> behavior;
	public:
		std::string name = "Node";
		bool active = true,
			 toRender = true;
	public:
		static Node* root;

		void addChild(Node* child, bool runtime=true);
		void addBehavior(Behavior* behavior);

		void start();
		void update();
		void render();
		void free();
	};
}