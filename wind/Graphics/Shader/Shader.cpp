#include "Shader.h"

namespace WindEngine {
	void Shader::create(const std::string vertexPath, const std::string fragmentPath) {
		Log::begin() << "Start create shader program";

		Log::info() << "Load vertex shader: " << vertexPath;
		Uint32 vertex; {
			std::string strSource; {
				std::ifstream file(vertexPath);

				if (!file.is_open()) {
					Log::error() << "Couldn't open shader file:" << vertexPath;
					return;
				}

				std::string line = "";
				while (std::getline(file, line)) {
					strSource += line + "\n";
				}

				file.close();
			}
			const GLchar* source = strSource.c_str();

			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &source, nullptr);
			glCompileShader(vertex);

			int success;
			glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
			if (success == 0) {
				char error[512]; glGetShaderInfoLog(vertex, 512, nullptr, error);
				Log::error() << error;
				return;
			}
		}

		Log::info() << "Load fragment shader: " << fragmentPath;
		Uint32 fragment; {
			std::string strSource; {
				std::ifstream file(fragmentPath);

				if (!file.is_open()) {
					Log::error() << "Couldn't open shader file:" << fragmentPath;
					return;
				}

				std::string line = "";
				while (std::getline(file, line)) {
					strSource += line + "\n";
				}

				file.close();
			}
			const GLchar* source = strSource.c_str();

			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &source, nullptr);
			glCompileShader(fragment);

			int success;
			glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
			if (success == 0) {
				char error[512]; glGetShaderInfoLog(fragment, 512, nullptr, error);
				Log::error() << error;
				return;
			}
		}

		Log::info() << "Link shader program";

		this->ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);

		int success;
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (success == 0) {
			char error[512]; glGetProgramInfoLog(ID, 512, nullptr, error);
			Log::error() << error;
			return;
		}

		Log::info() << "Delete vertex and fragment shaders";

		glDeleteShader(vertex);
		glDeleteShader(fragment);

		Log::end() << "Successfull finish create shader program:" << ID;
	}

	void Shader::use() {
		glUseProgram(ID);
	}

	void Shader::free() {
		glDeleteProgram(ID);
	}

	void Shader::setFloat(const char* name, const float& value) {
		glUniform1f(glGetUniformLocation(ID, name), value);
	}
	void Shader::setVec2f(const char* name, const glm::vec2& value) {
		glUniform2f(glGetUniformLocation(ID, name), value.x, value.y);
	}
	void Shader::setVec3f(const char* name, const glm::vec3& value) {
		glUniform3f(glGetUniformLocation(ID, name), value.x, value.y, value.z);
	}
	void Shader::setVec4f(const char* name, const glm::vec4& value) {
		glUniform4f(glGetUniformLocation(ID, name), value.x, value.y, value.z, value.w);
	}
	void Shader::setBool(const char* name, const bool& value) {
		glUniform1i(glGetUniformLocation(ID, name), value);
	}
	void Shader::setVec2b(const char* name, const glm::bvec2& value) {
		glUniform2i(glGetUniformLocation(ID, name), value.x, value.y);
	}
	void Shader::setVec3b(const char* name, const glm::bvec3& value) {
		glUniform3i(glGetUniformLocation(ID, name), value.x, value.y, value.z);
	}
	void Shader::setVec4b(const char* name, const glm::bvec4& value) {
		glUniform4i(glGetUniformLocation(ID, name), value.x, value.y, value.z, value.w);
	}
	void Shader::setInt(const char* name, const int& value) {
		glUniform1i(glGetUniformLocation(ID, name), value);
	}
	void Shader::setVec2i(const char* name, const glm::ivec2& value) {
		glUniform2i(glGetUniformLocation(ID, name), value.x, value.y);
	}
	void Shader::setVec3i(const char* name, const glm::ivec3& value) {
		glUniform3i(glGetUniformLocation(ID, name), value.x, value.y, value.z);
	}
	void Shader::setVec4i(const char* name, const glm::ivec4& value) {
		glUniform4i(glGetUniformLocation(ID, name), value.x, value.y, value.z, value.w);
	}
	void Shader::setMat(const char* name, glm::mat2x4& mat) {
		glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, false, glm::value_ptr(mat));
	}
	void Shader::setMat(const char* name, glm::mat4& mat) {
		glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, false, glm::value_ptr(mat));
	}
}