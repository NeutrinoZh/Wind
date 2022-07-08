#include "Window.h"

int main(int argc, char** args) {
	Log::config([](auto& self) {
		self.outConsole = true;
		self.folderToSave = "./logs/";
		self.numSpace = 2;
	});

	if (!EngineCore::Window::init()) {
		Log::error() << "Error init window";
		return false;
	}

	EngineCore::Window::loop();
	EngineCore::Window::free();

	return EXIT_SUCCESS;
};