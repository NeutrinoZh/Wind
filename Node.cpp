#include "Behavior.h"

namespace EngineCore {
	Node* Node::root = NULL;

	void Node::addChild(Node* child, bool runtime) {

		bool add = false;
		for (size_t i = 0; i < this->children.size(); ++i)
			if (!this->children[i]) {
				this->children[i] = child;
				add = true;
			}

		if (!add)
			this->children.push_back(child);

		if (runtime)
			child->start();
	}
	void Node::addBehavior(Behavior* behavior) {
		if (!behavior) {
			Log::error() << "Couldn't add NULL behavior to node";
			return;
		}

		Behavior* b = static_cast<Behavior*>(behavior);
		b->node = this;

		this->behavior.push_back(b);
	}

	void Node::start() {
		for (size_t i = 0; i < this->behavior.size(); ++i)
			this->behavior[i]->start();

		for (size_t i = 0; i < this->children.size(); ++i)
			if (this->children[i])
				this->children[i]->start();
	}
	void Node::update() {
		if (!active)
			return;

		for (size_t i = 0; i < this->behavior.size(); ++i)
			this->behavior[i]->update();

		for (size_t i = 0; i < this->children.size(); ++i)
			if (this->children[i])
				this->children[i]->update();
	}
	void Node::render() {
		if (!toRender)
			return;

		for (size_t i = 0; i < this->behavior.size(); ++i)
			this->behavior[i]->render();

		for (size_t i = 0; i < this->children.size(); ++i)
			if (this->children[i])
				this->children[i]->render();
	}
	void Node::free() {
		for (size_t i = 0; i < this->behavior.size(); ++i) {
			this->behavior[i]->free();
			delete this->behavior[i];
		}

		for (size_t i = 0; i < this->children.size(); ++i)
			if (this->children[i]) {
				this->children[i]->free();
				delete this->children[i];
			}
	}
}