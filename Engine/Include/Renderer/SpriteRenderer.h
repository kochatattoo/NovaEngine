#pragma once
#include "Scene/Component.h"
#include <memory>
#include <string>
#include <glad/gl.h>
#include "Renderer/Texture2D.h"
#include "Renderer/Shader.h"

namespace NK {

	class SpriteRenderer : public Component {
	public:
		SpriteRenderer(GameObject* owner);

		// ”становить текстуру (принимает путь, но лучше загружать через ResourceManager)
		void SetTexture(std::shared_ptr<Texture2D> texture);
		// ”становить шейдер
		void SetShader(std::shared_ptr<Shader> shader);
		// –исует спрайт, использу€ матрицу камеры из Engine
		void Render(const glm::mat4& viewProjection);
		void SetAlignment(float hAlign, float vAlign) { m_HAlign = hAlign; m_VAlign = vAlign; }

		void SetCustomSize(const glm::vec2& size) { m_CustomSize = size; }
		glm::vec2 GetCustomSize() const { return m_CustomSize; }

		void SetColor(float r, float g, float b, float a);
		void SetUseColor(bool use) { m_UseColor = use; }

		void OnStart() override;
		void OnUpdate(float deltaTime) override; // тут будет отрисовка

		void SetIsUI(bool ui) { m_IsUI = ui; }

		void SetPixelsPerUnit(float ppu) { m_PixelsPerUnit = ppu; }
		float GetPixelsPerUnit() const { return m_PixelsPerUnit; }

	private:
		std::shared_ptr<Texture2D> m_Texture;
		std::shared_ptr<Shader> m_Shader;
		glm::vec4 m_Color = glm::vec4(1.0f);
		bool m_UseColor = false;
		float m_HAlign = 0.0f;
		float m_VAlign = 0.0f;
		static uint32_t s_QuadVAO;
		static uint32_t s_QuadVBO;
		static bool s_Initialized;

		bool m_IsUI = false;
		float m_PixelsPerUnit = 100.0f; // дл€ игровых объектов

		static void InitQuad();

		glm::vec2 m_CustomSize = glm::vec2(0.0f); // (0,0) означает "использовать размер текстуры"
	};

} // namespace NK
