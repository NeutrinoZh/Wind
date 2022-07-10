#pragma once
#include "Window.h"

namespace EngineCore {
	class Shader {
	private:
		Uint32 ID = 0;
	public:
		void create(const std::string vertexPath, const std::string fragmentPath);
		void use();
		void free();

		void setFloat(const char* name, const float& value);
		void setVec2f(const char* name, const glm::vec2& value);
		void setVec3f(const char* name, const glm::vec3& value);
		void setVec4f(const char* name, const glm::vec4& value);

		void setBool(const char* name, const bool& value);
		void setVec2b(const char* name, const glm::bvec2& value);
		void setVec3b(const char* name, const glm::bvec3& value);
		void setVec4b(const char* name, const glm::bvec4& value);

		void setInt(const char* name, const int& value);
		void setVec2i(const char* name, const glm::ivec2& value);
		void setVec3i(const char* name, const glm::ivec3& value);
		void setVec4i(const char* name, const glm::ivec4& value);

		void setMat(const char* name, glm::mat2x4& mat);
		void setMat(const char* name, glm::mat4& mat);
	};
}