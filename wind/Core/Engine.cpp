#include "Engine.h"

namespace WindEngine {

	namespace {
		void start() {
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

			GL_Context::render();
		}
	}

	//======================================================//

	void (*Core::user_start)  (void) = nullptr;
	void (*Core::user_update) (void) = nullptr;

	bool Core::init(JText::Object& config) {
		Log::begin() << "Started initialization";

		if (!SDLSystems::init(config["SDL"])) {
			Log::error() << "WindEngine. Couldn't init SDL libs";
			return false;
		}

		if (!GL_Context::preInit(config["OpenGL"])) {
			Log::error() << "WindEngine. Couldn't pre-init OpenGL";
			return false;
		}

		if (!Window::init(config["Window"])) {
			Log::error() << "WindEngine. Couldn't create window";
			return false;
		}

		if (!GL_Context::postInit(config["OpenGL"])) {
			Log::error() << "WindEngine. Couldn't post-init OpenGL";
			return false;
		}

		if (!Net::init(config["Server"])) {
			Log::error() << "WindEngine. Couldn't init Net-System";
			return false;
		}

		if (!GUI::init()) {
			Log::error() << "WindEngine. Couldn't init GUI-System";
			return false;
		}

		Log::end() << "Success finish initialization";

		return true;
	}
	void Core::free() {
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

		WindEngine::Window::Start = start;
		WindEngine::Window::Update = update;
		WindEngine::GL_Context::user_render = render;

		JText::Object config;
		if (!JText::parse("./asset/config.jt", config)) {
			Log::error() << "WindEngine. Couldn't read engine config file";
			return EXIT_FAILURE;
		}

		if (!init(config))
			return EXIT_FAILURE;
		Resources::load("./asset/resources.jt");

		GUI::shader = shaders()["std-shader"]; // !!

		Window::loop();
		free();

		return EXIT_SUCCESS;
	}
}