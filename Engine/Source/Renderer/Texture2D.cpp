#include "Renderer/Texture2D.h"
#include "Core/Log.h"
#include <stb/stb_image.h>

namespace NK {

	Texture2D::Texture2D(const std::string& filepath) {
		// «агружаем изображение с переворотом по Y (OpenGL ожидает низ изображени€ внизу)
		stbi_set_flip_vertically_on_load(1);
		int channels;
		unsigned char* data = stbi_load(filepath.c_str(), &m_Width, &m_Height, &channels, 4); // требуем RGBA
		if (!data) {
			NK_CORE_ERROR("Failed to load texture: {0}", filepath);
			return;
		}

		// √енерируем текстуру
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		// «агружаем данные в OpenGL
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		// √енерируем мип-уровни (дл€ уменьшенных версий)
		glGenerateMipmap(GL_TEXTURE_2D);

		// Ќастройка фильтрации: линейна€ дл€ увеличени€ и уменьшени€ с мип-уровн€ми
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// «ажим координат за границы [0,1] Ч повтор€ть крайние пиксели
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		stbi_image_free(data); // освобождаем пам€ть, данные уже в OpenGL
	}

	Texture2D::~Texture2D() {
		glDeleteTextures(1, &m_RendererID);
	}

	void Texture2D::Bind(uint32_t slot) const {
		// ¬ыбираем текстурный блок (GL_TEXTURE0 + slot)
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

	void Texture2D::Unbind() const {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

} // namespace NK