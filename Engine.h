#pragma once
#include "GUIObject.h"

namespace EngineCore {
	struct Core {
	public:
		static void (*Start) (void);
		static void (*Update) (void);
		static void (*Draw) (void);

		static GameObject* scene;
	private:
		static std::map<std::string, GameObject*> scenes;
	public:
		static void addScene(std::string name, GameObject* scene);
		static void setScene(std::string name);

		static int loop();
	};
}