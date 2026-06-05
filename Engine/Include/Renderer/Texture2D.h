#pragma once
#include <string>
#include <glad/gl.h>

namespace NK {

	class Texture2D {
	public:
		Texture2D(); // пустой конструктор
		// Загрузка из файла
		explicit Texture2D(const std::string& filepath);
		~Texture2D();

		void CreateFromData(int width, int height, const void* data); // новый метод

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
