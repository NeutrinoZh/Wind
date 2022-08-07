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

			Log::end() << "Success finish load resource";

			GUI::shader = shaders()["std-shader"]; // !

			if (Core::Start)
				Core::Start();

			if (Core::scene)
				Core::scene->start();
		}

		void draw() {
			if (Core::Draw)
				Core::Draw();
		}

		void update() {
			Net::update();

			if (Core::Update)
				Core::Update();

			if (Core::scene)
				Core::scene->update();

			EngineCore::GL_Context::draw();
		}

		void free() {
			Log::begin() << "Memory cleaning procedure started";

			EngineCore::animations().freeAll();
			EngineCore::fonts().freeAll();
			EngineCore::shaders().freeAll();
			EngineCore::textures().freeAll();
			EngineCore::nodes().freeAll();

			EngineCore::Net::free();
			EngineCore::Window::free();
			EngineCore::GL_Context::free();

			if (Core::scene) { // !
				Core::scene->free();
				delete Core::scene;
			}

			Log::end() << "Memory cleaning procedure finished";
		}
	}

	//======================================================//

	void (*Core::Start) (void)  = nullptr;
	void (*Core::Update) (void) = nullptr;
	void (*Core::Draw) (void)   = nullptr;

	Node* Core::scene = NULL;
	std::map<std::string, Node*> Core::scenes = std::map<std::string, Node*>();

	void Core::addScene(std::string name, Node* scene) {
		if (scene)
			Core::scenes.insert(std::make_pair(name, scene));
	}

	void Core::setScene(std::string name) {
		try {
			Core::scene = Core::scenes.at(name);
		} catch (std::out_of_range& ex) {
			Log::error() << "Couldn't set scene: " << ex.what();
		}
	}

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

	//======================================================//
}