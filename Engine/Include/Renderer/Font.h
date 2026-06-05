#pragma once
#include <string>
#include <memory>
#include <cstdint>
#include <vector>
#include "Renderer/Texture2D.h"
#include <stb/stb_truetype.h>

namespace NK {

	class Font {
	public:
		// Загружает шрифт из файла .ttf
		explicit Font(const std::string& filepath);
		~Font();

		// Создаёт текстуру с указанным текстом, размером шрифта и цветом (RGBA)
		std::shared_ptr<Texture2D> CreateTextTexture(const std::string& text, float fontSize,
			uint8_t r = 255, uint8_t g = 255,
			uint8_t b = 255, uint8_t a = 255);

	private:
		std::vector<unsigned char> m_FontData;
		stbtt_fontinfo m_FontInfo;
		float m_Scale; // масштаб для преобразования единиц шрифта в пиксели
		bool m_Valid = false;
	};

} // namespace NK