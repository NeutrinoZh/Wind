#include "Shaders.h"

bool preInit() {
	return EngineCore::GL_Context::preInit();
}

bool postInit() {
	return EngineCore::GL_Context::postInit();
}

void draw() {

}

void update() {
	EngineCore::GL_Context::draw();
}

void free() {
	Log::begin() << "Memory cleaning procedure started";

	EngineCore::shaders().freeAll();
	EngineCore::textures().freeAll();
	EngineCore::Window::free();
	EngineCore::GL_Context::free();

	Log::end() << "Memory cleaning procedure finished";
}

void start() {
	EngineCore::textures().loadFolder("asset/textures/");
	EngineCore::shaders().loadFolder("asset/meta-shaders/");
}

int main(int argc, char** args) {
	Log::config([](auto& self) {
		self.outConsole = true;
		self.folderToSave = "./logs/";
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
}; 