#include "Graphics.h"

namespace EngineCore {
	glm::vec3 GL_Context::color = { 0, 0, 0 };
	void* GL_Context::context = nullptr;
	void (*GL_Context::Draw) (void) = nullptr;

	bool GL_Context::preInit() {
		Log::info() << "PreInit OpenGL";

		struct _config {
			Uint8 r = 3, g = 3, b = 3, a = 0;
			Uint8 bufferSize = 0;
			bool doubleBuffer = false,
				 accelerated = false,
				 contextReleaseBehavior = false;
			Uint8 GL_Major = 3, GL_Minor = 1,
					contextProfile = 0;
			glm::vec3 color = {};

			_config() {
				Log::info() << "Read OpenGL config file";

				Config config = ConfigReader::read("./opengl.txt");

				if (config.isVar("red")) r = config.getIntValue("red");
				if (config.isVar("green")) g = config.getIntValue("green");
				if (config.isVar("blue")) b = config.getIntValue("blue");
				if (config.isVar("alpha")) a = config.getIntValue("alpha");

				if (config.isVar("bufferSize")) bufferSize = config.getIntValue("bufferSize");
				if (config.isVar("doubleBuffer")) doubleBuffer = config.getBoolValue("doubleBuffer");
				if (config.isVar("accelerated")) accelerated = config.getBoolValue("accelerated");
				if (config.isVar("contextReleaseBehavior")) contextReleaseBehavior = config.getBoolValue("contextReleaseBehavior");
				
				if (config.isVar("GLMajorVersion")) GL_Major = config.getIntValue("GLMajorVersion");
				if (config.isVar("GLMinorVersion")) GL_Minor = config.getIntValue("GLMinorVersion");

				if (config.isVar("contextProfile")) {
					std::string value = config.getStringValue("contextProfile");
					if (value == "SDL_GL_CONTEXT_PROFILE_CORE")
						this->contextProfile = SDL_GL_CONTEXT_PROFILE_CORE;
					else if (value == "SDL_GL_CONTEXT_PROFILE_COMPATIBILITY")
						this->contextProfile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
					else if (value == "SDL_GL_CONTEXT_PROFILE_ES")
						this->contextProfile = SDL_GL_CONTEXT_PROFILE_ES;
				}

				if (config.isVar("clearColorR")) color.r = config.getFloatValue("clearColorR");
				if (config.isVar("clearColorG")) color.g = config.getFloatValue("clearColorG");
				if (config.isVar("clearColorB")) color.b = config.getFloatValue("clearColorB");
			}
		} config;

		Log::info() << "Set OpenGL parameters";

		GL_Context::color = config.color;

		UINT8 success =
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, config.doubleBuffer) +
			SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, config.accelerated) +
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, config.GL_Major) +
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, config.GL_Minor) +
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, config.contextProfile) +
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_RELEASE_BEHAVIOR, config.contextReleaseBehavior) +
			SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, config.bufferSize) +
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, config.r) +
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, config.g) +
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, config.b) +
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, config.a);

		if (success != 0) {
			Log::error() << SDL_GetError();
			return false;
		}

		return true;
	}

	bool GL_Context::postInit() {
		Log::info() << "PostInit OpenGL";

		if (!Window::window) { // !!!
			Log::error() << "Counldn't create OpenGL context because window been NULL";
			return false;
		}

		context = SDL_GL_CreateContext(Window::window);
		if (!context) {
			Log::error() << SDL_GetError();
			return false;
		}

		Log::info() << "GLAD init";
		if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
			Log::error() << "Couldn't init glad";
			return false;
		}

		struct _config {
			glm::u32vec4 viewport = {
				0, 0,
				Window::size.x, Window::size.y
			};

			bool EnableBlend = true;
			Uint32 sfactor = GL_SRC_ALPHA, dfactor = GL_ONE_MINUS_SRC_ALPHA;

			_config() {
				Log::info() << "Read OpenGL config";

				Config config = ConfigReader::read("./opengl.txt");

				if (config.isVar("EnableBlend")) EnableBlend = config.getBoolValue("EnableBlend");

				if (config.isVar("viewportX")) viewport.x = config.getIntValue("viewportX");
				if (config.isVar("viewportY")) viewport.y = config.getIntValue("viewportY");
				if (config.isVar("viewportW")) viewport.z = config.getIntValue("viewportW");
				if (config.isVar("viewportH")) viewport.w = config.getIntValue("viewportH");

				for (Uint8 i = 0; i < 2; i++) {
					if (config.isVar("BlendSfactor") || config.isVar("BlendDfactor")) {
						
						std::string name, value;
						if (config.isVar("BlendSfactorName")) {
							name = config.getStringValue("BlendSfactorName");
							if (config.isVar("BlendSfactor"))
								value = config.getStringValue("BlendSfactor");
						} else {
							name = config.getStringValue("BlendDfactorName");
							if (config.isVar("BlendDfactor"))
								value = config.getStringValue("BlendDfactor");
						}
					
						#define GL_BLEND_FACTOR(flag) if (value == #flag) { (name == "sfactor" ? sfactor : dfactor) = flag; }
						
							 GL_BLEND_FACTOR(GL_ZERO)
						else GL_BLEND_FACTOR(GL_ONE)
						else GL_BLEND_FACTOR(GL_SRC_COLOR)
						else GL_BLEND_FACTOR(GL_ONE_MINUS_SRC_COLOR)
						else GL_BLEND_FACTOR(GL_DST_COLOR)
						else GL_BLEND_FACTOR(GL_ONE_MINUS_DST_COLOR)
						else GL_BLEND_FACTOR(GL_SRC_ALPHA)
						else GL_BLEND_FACTOR(GL_ONE_MINUS_SRC_ALPHA)
						else GL_BLEND_FACTOR(GL_DST_ALPHA)
						else GL_BLEND_FACTOR(GL_ONE_MINUS_DST_ALPHA)
						else GL_BLEND_FACTOR(GL_CONSTANT_COLOR)
						else GL_BLEND_FACTOR(GL_ONE_MINUS_CONSTANT_COLOR)
						else GL_BLEND_FACTOR(GL_CONSTANT_ALPHA)
						else GL_BLEND_FACTOR(GL_ONE_MINUS_CONSTANT_ALPHA)
						else GL_BLEND_FACTOR(GL_SRC_ALPHA_SATURATE)
					}
				}
			}
		} config;

		Log::info() << "Set OpenGL parameters";
		glViewport(config.viewport.x, config.viewport.y, config.viewport.z, config.viewport.w);
		if (config.EnableBlend) {
			glEnable(GL_BLEND);
			glBlendFunc(config.sfactor, config.dfactor);
		}

		return true;
	}

	void GL_Context::draw() {
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(color.r, color.g, color.b, 1.f);

		if (Draw) Draw();

		glFlush();
		SDL_GL_SwapWindow(Window::window);
		// autoresize
	}

	void GL_Context::free() {
		Log::info() << "Free memory from OpenGL";

		SDL_GL_DeleteContext(context);
	}
}