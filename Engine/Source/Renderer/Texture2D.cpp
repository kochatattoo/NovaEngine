#include "Renderer/Texture2D.h"
#include "Core/Log.h"
#include <stb/stb_image.h>
#include <sol/usertype_storage.hpp>

namespace NK {
	Texture2D::Texture2D() : m_RendererID(0), m_Width(0), m_Height(0) {}

	std::shared_ptr<Texture2D> Texture2D::CreateSolidColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		const int w = 64, h = 64;
		std::vector<uint8_t> data(w * h * 4);
		for (int i = 0; i < w * h; ++i) {
			data[i * 4 + 0] = r;
			data[i * 4 + 1] = g;
			data[i * 4 + 2] = b;
			data[i * 4 + 3] = a;
		}
		auto tex = std::make_shared<Texture2D>();
		tex->CreateFromData(w, h, data.data());
		return tex;
	}

	void Texture2D::CreateFromData(int width, int height, const void* data) {
		if (m_RendererID) glDeleteTextures(1, &m_RendererID); // на вс€кий случай
		m_Width = width;
		m_Height = height;
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

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