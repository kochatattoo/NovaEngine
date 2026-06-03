#pragma once
#include <string>
#include <glad/gl.h>

namespace NK {

	class Texture2D {
	public:
		// Загрузка из файла
		explicit Texture2D(const std::string& filepath);
		~Texture2D();

		// Активировать текстуру на определённом слоте (0, 1, ...)
		void Bind(uint32_t slot = 0) const;
		void Unbind() const;

		// Получить размеры
		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }

	private:
		uint32_t m_RendererID;
		int m_Width, m_Height;
	};

} // namespace NK
