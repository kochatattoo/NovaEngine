#pragma once
#include <string>
#include <glad/gl.h>    // типы OpenGL
// #include <glm/glm.hpp>
#include <unordered_map>

namespace NK {

	class Shader {
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		// Активировать программу (последующие вызовы glDraw* будут использовать её)
		void Bind() const;
		void Unbind() const;

		// Установить uniform-переменную vec4 (для цвета)
		void SetUniform4f(const std::string& name, float r, float g, float b, float a) const;

	private:
		uint32_t m_RendererID;
		// Кэш для uniform-локаций
		mutable std::unordered_map<std::string, int> m_UniformLocationCache;

		int GetUniformLocation(const std::string& name) const;
		uint32_t CompileShader(uint32_t type, const std::string& source);
	};

} // namespace NK
