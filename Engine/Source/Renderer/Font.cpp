#include "Renderer/Font.h"
#include "Core/Log.h"
#include <stb/stb_truetype.h>

namespace NK {

	Font::Font(const std::string& filepath) {
		// Читаем файл
		FILE* f = nullptr;
		errno_t err = fopen_s(&f, filepath.c_str(), "rb");
		if (err != 0 || !f) {
			NK_CORE_ERROR("Failed to open font file: %s", filepath.c_str());
			return;
		}
		fseek(f, 0, SEEK_END);
		long size = ftell(f);
		fseek(f, 0, SEEK_SET);
		m_FontData.resize(size);
		fread(m_FontData.data(), 1, size, f);
		fclose(f);

		if (!stbtt_InitFont(&m_FontInfo, m_FontData.data(), 0)) {
			NK_CORE_ERROR("Failed to initialize font");
			return;
		}
		m_Valid = true;
	}

	Font::~Font() {}

	std::shared_ptr<Texture2D> Font::CreateTextTexture(const std::string& text, float fontSize,
		uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		if (!m_Valid) return nullptr;

		m_Scale = stbtt_ScaleForPixelHeight(&m_FontInfo, fontSize);

		int ascent, descent, lineGap;
		stbtt_GetFontVMetrics(&m_FontInfo, &ascent, &descent, &lineGap);
		float baseline = ascent * m_Scale;

		// Определяем ширину текста и высоту
		float x = 0;
		float maxWidth = 0;
		for (char c : text) {
			int advance, lsb;
			stbtt_GetCodepointHMetrics(&m_FontInfo, c, &advance, &lsb);
			x += advance * m_Scale;
			if (x > maxWidth) maxWidth = x;
		}
		int width = (int)ceilf(maxWidth);
		int height = (int)ceilf((ascent - descent) * m_Scale);

		// Создаём временный буфер RGBA
		std::vector<unsigned char> pixels(width * height * 4, 0);

		x = 0;
		for (char c : text) {
			int advance, lsb, x0, y0, x1, y1;
			stbtt_GetCodepointHMetrics(&m_FontInfo, c, &advance, &lsb);
			stbtt_GetCodepointBitmapBox(&m_FontInfo, c, m_Scale, m_Scale, &x0, &y0, &x1, &y1);

			int glyphWidth = x1 - x0;
			int glyphHeight = y1 - y0;
			if (glyphWidth <= 0 || glyphHeight <= 0) {
				x += advance * m_Scale;
				continue;
			}

			std::vector<unsigned char> glyphPixels(glyphWidth * glyphHeight);
			stbtt_MakeCodepointBitmap(&m_FontInfo, glyphPixels.data(), glyphWidth, glyphHeight,
				glyphWidth, m_Scale, m_Scale, c);

			int offsetX = (int)x + x0;
			int offsetY = (int)(baseline + y0);

			// Копируем альфа-канал в RGBA буфер
			for (int row = 0; row < glyphHeight; ++row) {
				for (int col = 0; col < glyphWidth; ++col) {
					int px = offsetX + col;
					int py = offsetY + row;
					if (px < 0 || px >= width || py < 0 || py >= height) continue;
					int srcIndex = row * glyphWidth + col;
					int dstIndex = (py * width + px) * 4;
					unsigned char alpha = glyphPixels[srcIndex];
					if (alpha > 0) {
						pixels[dstIndex + 0] = r;
						pixels[dstIndex + 1] = g;
						pixels[dstIndex + 2] = b;
						pixels[dstIndex + 3] = alpha;
					}
				}
			}
			x += advance * m_Scale;
		}

		// Создаём текстуру
		auto texture = std::make_shared<Texture2D>();
		texture->CreateFromData(width, height, pixels.data());
		return texture;
	}

} // namespace NK