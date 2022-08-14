#include "Graphics.h"

namespace EngineCore {
	glm::vec3 GL_Context::color = { 0, 0, 0 };
	glm::vec2 GL_Context::currentSize = {0, 0};
	glm::mat4 GL_Context::proj = glm::mat4(1);
	bool GL_Context::autoResize = false;
	void* GL_Context::context = nullptr;
	void (*GL_Context::user_render) (void) = nullptr;

	namespace {
		Uint32 VAO, VBO, EBO;

		float verticesQuad[16] = {
			0,  0,  0, 0,
			1,  0,  1, 0,
			0, -1,  0, 1,
			1, -1,  1, 1
		};

		const Uint32 indicesQuad[6] = {
			0, 1, 2, 1, 2, 3
		};
	}

	bool GL_Context::preInit(JText::Object& obj_config) {
		Log::info() << "PreInit OpenGL";

		struct _ {
			Uint8 depthR = 3,
				  depthG = 3,
				  depthB = 3,
				  depthA = 0;

			Uint8 bufferSize = 0;
			Uint8 depthSize = 16;

			bool doubleBuffer = false,
				 accelerated = false,
				 contextReleaseBehavior = false;

			Uint8 GL_Major = 3, GL_Minor = 1;
				
			Uint8 contextProfile = 0;

			glm::vec3 clearColor = {};

			_(JText::Object& config) {
				depthR = config["depthColor"][0]._uint8(depthR);
				depthG = config["depthColor"][1]._uint8(depthG);
				depthB = config["depthColor"][2]._uint8(depthB);
				depthA = config["depthColor"][3]._uint8(depthA);

				bufferSize = config["bufferSize"]._uint8(bufferSize);
				depthSize  = config["depthSize"]._uint8(depthSize);

				doubleBuffer = config["doubleBuffer"]._bool(doubleBuffer);
				accelerated  = config["accelerated"]._bool(accelerated);
				contextReleaseBehavior = config["contextReleaseBehavior"]._bool(contextReleaseBehavior);

				GL_Major = config["version"]._major(GL_Major);
				GL_Minor = config["version"]._minor(GL_Minor);

				std::string profile = config["contextProfile"]._str("");
				if	    (profile == "SDL_GL_CONTEXT_PROFILE_CORE")			this->contextProfile = SDL_GL_CONTEXT_PROFILE_CORE;
				else if (profile == "SDL_GL_CONTEXT_PROFILE_COMPATIBILITY") this->contextProfile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
				else if (profile == "SDL_GL_CONTEXT_PROFILE_ES")			this->contextProfile = SDL_GL_CONTEXT_PROFILE_ES;
				
				clearColor.r = config["clearColor"][0]._float(clearColor.r);
				clearColor.g = config["clearColor"][1]._float(clearColor.g);
				clearColor.b = config["clearColor"][2]._float(clearColor.b);
			}
		} config(obj_config);

		Log::info() << "OpenGL Version: " << static_cast<int>(config.GL_Major) << "."
										  << static_cast<int>(config.GL_Minor);

		Log::info() << "Set OpenGL parameters";

		GL_Context::color = config.clearColor;

		UINT8 success =
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, config.depthSize) +
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, config.doubleBuffer) +
			SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, config.accelerated) +
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, config.GL_Major) +
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, config.GL_Minor) +
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, config.contextProfile) +
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_RELEASE_BEHAVIOR, config.contextReleaseBehavior) +
			SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, config.bufferSize) +
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   config.depthR) +
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, config.depthG) +
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  config.depthB) +
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, config.depthA);

		if (success != 0) {
			Log::error() << SDL_GetError();
			return false;
		}

		return true;
	}

	bool GL_Context::postInit(JText::Object& obj_config) {
		Log::info() << "PostInit OpenGL";

		struct _ {
			glm::u32vec4 viewport = {
				0, 0,
				Window::size.x, Window::size.y
			};

			bool EnableBlend = true, autoResize = false;
			Uint32 sfactor = GL_SRC_ALPHA, dfactor = GL_ONE_MINUS_SRC_ALPHA;

			_(JText::Object& config) {
				EnableBlend = config["EnableBlend"]._bool(EnableBlend);
				autoResize  = config["autoResize"]._bool(autoResize);

				viewport.x = config["viewport"][0]._uint32(viewport.x);
				viewport.y = config["viewport"][1]._uint32(viewport.y);
				viewport.z = config["viewport"][2]._uint32(viewport.z);
				viewport.w = config["viewport"][3]._uint32(viewport.w);

				if (EnableBlend)
					for (Uint8 i = 0; i < 2; i++) {
						std::string value;

						if (i) value = config["blendSfactor"]._str("");
						else   value = config["blendDfactor"]._str("");

						#define GL_BLEND_FACTOR(flag) if (value == #flag) { (i ? sfactor : dfactor) = flag; }

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
		} config(obj_config);

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

		Log::info() << "Set OpenGL parameters";
		GL_Context::autoResize = config.autoResize;
		currentSize = Window::size;
		glViewport(config.viewport.x, config.viewport.y, config.viewport.z, config.viewport.w);
		if (config.EnableBlend) {
			glEnable(GL_BLEND);
			glBlendFunc(config.sfactor, config.dfactor);
		}

		Log::info() << "Creating VAO, VBO and EBO for sprites";
		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO); 
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, verticesQuad, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 6, indicesQuad, GL_DYNAMIC_DRAW);

		Log::info() << "VAO(" << VAO << "); VBO(" << VBO << "); EBO(" << EBO << ")";

		Log::info() << "Create ortho projection matrix";

		GL_Context::proj = glm::ortho(0.0f, Window::size.x / 100.f,
									  0.0f, Window::size.y / 100.f, -1.f, 1.0f);

		return true;
	}

	void GL_Context::render() {
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(color.r, color.g, color.b, 1.f);

		if (user_render) user_render();

		glFlush();
		SDL_GL_SwapWindow(Window::window);
		if (autoResize && currentSize != Window::size) {
			glViewport(0, 0, (int)Window::size.x, (int)Window::size.y);
			GL_Context::proj = glm::ortho(0.0f, Window::size.x / 100.f,
										  0.0f, Window::size.y / 100.f, -1.f, 1.0f);
			currentSize = Window::size;
		}
	}

	void GL_Context::free() {
		Log::info() << "Free memory from OpenGL";

		SDL_GL_DeleteContext(context);
	}
}