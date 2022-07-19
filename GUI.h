#pragma once
#include "Physics.h"

namespace EngineCore {
	class Widget;

	class GUI {
	public:
		static Shader* shader;
		static glm::mat4 proj;
		
		static void init();
		static void free();
	};

	class Widget {
	protected:
		virtual void start() {};
		virtual void update() {};
		virtual void draw() {};
		virtual void free() {};

		bool isMouseOver();
	public:
		glm::vec2 absolute = {0, 0};
		glm::vec2 position = {0, 0};
		glm::vec2 size = {0, 0};

		Widget* parent = nullptr;

		bool active = true,
			 paint = true;

		enum class State {
			NONE,
			OVER,
			CLICK,
			PRESSED
		};
		State state = State::NONE;

		void Start();
		void Update();
		void Draw();
		void Free();
	};

	class Button : public Widget {
	private:
		Texture texture = Texture(0);
		glm::vec4 color = { 1, 1, 1, 1 };
	protected:
		void start();
		void update();
		void draw();
	public:
		Texture t_none = Texture(0);
		Texture t_over = Texture(0);
		Texture t_pressed = Texture(0);
	
		glm::vec4 c_none    = { 0.9f, 0.9f, 0.9f, 1.f };
		glm::vec4 c_over    = { 1.f , 1.f , 1.f , 1.f };
		glm::vec4 c_pressed = { 0.8f, 0.8f, 0.8f, 1.f };
	};

	class Label : public Widget {
	private:
		Texture texture = Texture(0);
		std::string text = "";
	public:
		glm::vec4 color = {1, 1, 1, 1};
		TTF_Font* font = nullptr;

		void setText(std::string text);
	protected:
		void draw();
	};

	class Chat : public Widget {
	private:
		std::vector<EngineCore::Label*> chat = std::vector<EngineCore::Label*>();
		clock_t lastDestroy = 0;
	protected:
		void update();
		void draw();
	public:
		TTF_Font* font = nullptr;

		void send(std::string text);
	};
}