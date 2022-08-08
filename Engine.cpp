#include "Engine.h"

namespace EngineCore {

	namespace {
		bool preInit() {
			return EngineCore::GL_Context::preInit();
		}

		bool postInit() {
			return (
				EngineCore::GL_Context::postInit() &&
				EngineCore::Net::init()
			);
		}

		void start() {
			GUI::init();

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

		void free() {
			Log::begin() << "Memory cleaning procedure started";

			if (Node::root) {
				Node::root->free();
				delete Node::root;
			}

			EngineCore::animations().freeAll();
			EngineCore::fonts().freeAll();
			EngineCore::shaders().freeAll();
			EngineCore::textures().freeAll();
			EngineCore::nodes().freeAll();
			EngineCore::scenes().freeAll();

			EngineCore::Net::free();
			EngineCore::Window::free();
			EngineCore::GL_Context::free();

			Log::end() << "Memory cleaning procedure finished";
		}
	}

	//======================================================//

	void (*Core::user_start)  (void) = nullptr;
	void (*Core::user_update) (void) = nullptr;

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
		EngineCore::GL_Context::user_render = render;

		if (!EngineCore::Window::init()) {
			Log::error() << "Error init window";
			return EXIT_FAILURE;
		}

		EngineCore::Window::loop();
		free();

		return EXIT_SUCCESS;
	}

	//======================================================//
}