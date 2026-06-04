#pragma once
#include "Core/Component.h"
#include <memory>
#include <string>
#include <glad/gl.h>
#include "Renderer/Texture2D.h"
#include "Renderer/Shader.h"

namespace NK {

	class SpriteRenderer : public Component {
	public:
		SpriteRenderer(GameObject* owner);

		// Установить текстуру (принимает путь, но лучше загружать через ResourceManager)
		void SetTexture(std::shared_ptr<Texture2D> texture);
		// Установить шейдер
		void SetShader(std::shared_ptr<Shader> shader);
		// Рисует спрайт, используя матрицу камеры из Engine
		void Render(); 

		void OnStart() override;
		void OnUpdate(float deltaTime) override; // тут будет отрисовка

	private:
		std::shared_ptr<Texture2D> m_Texture;
		std::shared_ptr<Shader> m_Shader;
		// VAO и VBO для квада (можно сделать статическими общими для всех спрайтов)
		static uint32_t s_QuadVAO;
		static uint32_t s_QuadVBO;
		static bool s_Initialized;

		static void InitQuad();
	};

} // namespace NK
