#include "Engine.h"

namespace EngineCore {

	namespace {
		void start() {
			Log::begin() << "Start load resource";

			EngineCore::textures().loadFolder("asset/meta-textures/");
			EngineCore::shaders().loadFolder("asset/meta-shaders/");
			EngineCore::fonts().loadFolder("asset/meta-fonts/");
			EngineCore::animations().loadFolder("asset/animations/");
			EngineCore::nodes().loadFolder("asset/nodes/");
			EngineCore::scenes().loadFolder("asset/scenes/");

			Log::end() << "Success finish load resource";

			GUI::shader = shaders()["std-shader"]; // !

			if (Core::user_start)
				Core::user_start();

			if (Node::root)
				Node::root->start();
		}

		void render() {
			if (Node::root)
				Node::root->render();
		}

		void update() {
			Net::update();

			if (Core::user_update)
				Core::user_update();

			if (Node::root)
				Node::root->update();

			EngineCore::GL_Context::render();
		}
	}

	//======================================================//

	void (*Core::user_start)  (void) = nullptr;
	void (*Core::user_update) (void) = nullptr;

	bool init() {
		Log::begin() << "Started initialization";

		JText::Object config;
		if (!JText::parse("./asset/config.jt", config)) {
			Log::error() << "EngineCore. Couldn't read engine config file";
			return false;
		}

		if (!SDLSystems::init(config["SDL"])) {
			Log::error() << "EngineCore. Couldn't init SDL libs";
			return false;
		}

		if (!GL_Context::preInit(config["OpenGL"])) {
			Log::error() << "EngineCore. Couldn't pre-init OpenGL";
			return false;
		}

		if (!Window::init(config["Window"])) {
			Log::error() << "EngineCore. Couldn't create window";
			return false;
		}

		if (!GL_Context::postInit(config["OpenGL"])) {
			Log::error() << "EngineCore. Couldn't post-init OpenGL";
			return false;
		}

		if (!Net::init(config["Server"])) {
			Log::error() << "EngineCore. Couldn't init Net-System";
			return false;
		}

		if (!GUI::init()) {
			Log::error() << "EngineCore. Couldn't init GUI-System";
			return false;
		}

		Log::end() << "Success finish initialization";

		return true;
	}

	void free() {
		Log::begin() << "Memory cleaning procedure started";

		if (Node::root) {
			Node::root->free();
			delete Node::root;
		}

		animations().freeAll();
		fonts().freeAll();
		shaders().freeAll();
		textures().freeAll();
		nodes().freeAll();
		scenes().freeAll();

		Log::info() << "Free memory from systems";

		Net::free();
		Window::free();
		GL_Context::free();

		SDLSystems::free();

		Log::end() << "Memory cleaning procedure finished";
	}

	int Core::loop() {
		Log::config([](auto& self) {
			self.outConsole = true;
			self.folderToSave = "./asset/logs/";
			self.numSpace = 2;
		});

		EngineCore::Window::Start = start;
		EngineCore::Window::Update = update;
		EngineCore::GL_Context::user_render = render;

		if (!init())
			return EXIT_FAILURE;
		Window::loop();
		free();

		return EXIT_SUCCESS;
	}
}