#pragma once
#include "GUIObject.h"

namespace EngineCore {
	struct Core {
	public:
		static void (*Start) (void);
		static void (*Update) (void);
		static void (*Draw) (void);

		static Node* scene;
	private:
		static std::map<std::string, Node*> scenes;
	public:
		static void addScene(std::string name, Node* scene);
		static void setScene(std::string name);

		static int loop();
	};
}