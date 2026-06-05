#pragma once
#include "Core/Component.h"
#include "Renderer/Texture2D.h"
#include "Renderer/Shader.h"
#include <memory>
#include <string>

namespace NK {

	class Font;

	class TextRenderer : public Component {
	public:
		TextRenderer(GameObject* owner);

		void SetFont(std::shared_ptr<Font> font);
		void SetText(const std::string& text);
		void SetFontSize(float size);
		void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

		void OnStart() override;
		void Render(const glm::mat4& viewProj); // вызывается Scene

	private:
		void UpdateTexture();

		std::shared_ptr<Font> m_Font;
		std::string m_Text;
		float m_FontSize = 24.0f;
		uint8_t m_Color[4] = { 255, 255, 255, 255 };
		std::shared_ptr<Texture2D> m_Texture;
		std::shared_ptr<Shader> m_Shader;
		bool m_TextChanged = true;

		// Статический меш (квад 1x1)
		static uint32_t s_QuadVAO;
		static uint32_t s_QuadVBO;
		static bool s_Initialized;
		static void InitQuad();
	};

} // namespace NK
