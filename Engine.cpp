#include "Engine.h"

namespace EngineCore {
	void (*Core::Start) (void)  = nullptr;
	void (*Core::Update) (void) = nullptr;
	void (*Core::Draw) (void)   = nullptr;

	GameObject* Core::scene = new GameObject();

	int Core::loop() {
		Log::config([](auto& self) {
			self.outConsole = true;
			self.folderToSave = "./asset/logs/";
			self.numSpace = 2;
		});

		EngineCore::Window::PreInit = preInit;
		EngineCore::Window::PostInit = postInit;
		EngineCore::Window::Start = start;
		EngineCore::Window::Update = update;
		EngineCore::GL_Context::Draw = draw;

		if (!EngineCore::Window::init()) {
			Log::error() << "Error init window";
			return EXIT_FAILURE;
		}

		EngineCore::Window::loop();
		free();

		return EXIT_SUCCESS;
	}
}