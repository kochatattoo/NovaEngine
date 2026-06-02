#include "Renderer/Shader.h"
#include "Core/Log.h"
#include <glad/gl.h>
#include<glm/gtc/type_ptr.hpp>

namespace NK {

	Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc) {
		// Создаём пустую программу
		m_RendererID = glCreateProgram();

		// Компилируем вершинный и фрагментный шейдеры
		uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexSrc);
		uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

		// Прикрепляем шейдеры к программе
		glAttachShader(m_RendererID, vs);
		glAttachShader(m_RendererID, fs);
		glLinkProgram(m_RendererID);
		glValidateProgram(m_RendererID);

		// Проверяем статус линковки
		int success;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetProgramInfoLog(m_RendererID, 512, nullptr, infoLog);
			NK_CORE_ERROR("Shader program linking failed: {0}", infoLog);
		}

		// Шейдеры больше не нужны, их можно удалить
		glDeleteShader(vs);
		glDeleteShader(fs);
	}

	Shader::~Shader() {
		glDeleteProgram(m_RendererID);
	}

	void Shader::Bind() const {
		glUseProgram(m_RendererID);
	}

	void Shader::Unbind() const {
		glUseProgram(0);
	}

	void Shader::SetUniform4f(const std::string& name, float r, float g, float b, float a) const {
		int location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location != -1)
			glUniform4f(location, r, g, b, a);
	}

	void Shader::SetUniformMat4(const std::string& name, const glm::mat4& matrix) {
		int location = GetUniformLocation(name);
		if (location != -1)
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::SetUniform1i(const std::string& name, int value) {
		int location = GetUniformLocation(name);
		if (location != -1)
			glUniform1i(location, value);
	}

	int Shader::GetUniformLocation(const std::string& name) const {
		// Если уже кэшировано, возвращаем
		auto it = m_UniformLocationCache.find(name);
		if (it != m_UniformLocationCache.end())
			return it->second;

		int location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location == -1)
			NK_CORE_WARN("Uniform {0} not found in shader", name);

		m_UniformLocationCache[name] = location;
		return location;
	}

	// Приватная функция компиляции одного шейдера
	uint32_t Shader::CompileShader(uint32_t type, const std::string& source) {
		uint32_t shader = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

		int success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			NK_CORE_ERROR("Shader compilation error ({0}): {1}", type, infoLog);
		}
		return shader;
	}

} // namespace NK