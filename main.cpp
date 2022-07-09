#include "Graphics.h"

bool PreInit() {
	return EngineCore::GL_Context::preInit();
}

bool PostInit() {
	return EngineCore::GL_Context::postInit();
}

void update() {
	/*glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(EngineCore::Window::window);*/
	EngineCore::GL_Context::draw();
}

void free() {
	Log::begin() << "Memory cleaning procedure started";

	EngineCore::Window::free();
	EngineCore::GL_Context::free();

	Log::end() << "Memory cleaning procedure finished";
}

int main(int argc, char** args) {
	Log::config([](auto& self) {
		self.outConsole = true;
		self.folderToSave = "./logs/";
		self.numSpace = 2;
	});

	EngineCore::Window::PreInit = PreInit;
	EngineCore::Window::PostInit = PostInit;
	EngineCore::Window::Update = update;

	if (!EngineCore::Window::init()) {
		Log::error() << "Error init window";
		return false;
	}

	EngineCore::Window::loop();
	free();
}; 