#pragma once
#include "../../Physics/Physics.h"

namespace WindEngine {
	class Widget {
	protected:
		virtual void start() {};
		virtual void update() {};
		virtual void draw() {};
		virtual void free() {};

		bool isMouseOver();
	public:
		~Widget() {
			Free();
		}

		glm::vec2 absolute = { 0, 0 };
		glm::vec2 position = { 0, 0 };
		glm::vec2 size = { 0, 0 };

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
}