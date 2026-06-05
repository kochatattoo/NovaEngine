#include "Renderer/SpriteRenderer.h"
#include "Core/Transform.h"
#include "Core/GameObject.h" // для доступа к Transform
#include "Core/ResourceManager.h"
#include "Core/Engine.h" // синглтон

namespace NK {

	uint32_t SpriteRenderer::s_QuadVAO = 0;
	uint32_t SpriteRenderer::s_QuadVBO = 0;
	bool SpriteRenderer::s_Initialized = false;

	SpriteRenderer::SpriteRenderer(GameObject* owner) : Component(owner) {}

	void SpriteRenderer::SetTexture(std::shared_ptr<Texture2D> texture) {
		m_Texture = texture;
	}

	void SpriteRenderer::SetShader(std::shared_ptr<Shader> shader) {
		m_Shader = shader;
	}

	void SpriteRenderer::InitQuad() {
		if (s_Initialized) return;

		float vertices[] = {
			// позиция (x,y)    UV (u,v)
			-0.5f, -0.5f,       0.0f, 0.0f,  // левый нижний
			 0.5f, -0.5f,       1.0f, 0.0f,  // правый нижний
			 0.5f,  0.5f,       1.0f, 1.0f,  // правый верхний
			-0.5f,  0.5f,       0.0f, 1.0f   // левый верхний
		};
		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

		glGenVertexArrays(1, &s_QuadVAO);
		glBindVertexArray(s_QuadVAO);

		glGenBuffers(1, &s_QuadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, s_QuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Индексный буфер
		uint32_t ibo;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		// VAO запоминает IBO

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		glBindVertexArray(0);
		s_Initialized = true;
	}

	void SpriteRenderer::Render(const glm::mat4& viewProjection) {
		if (!m_Texture || !m_Shader) return;
		auto* transform = m_Owner->GetComponent<Transform>();
		if (!transform) return;

		int texW = m_Texture->GetWidth();
		int texH = m_Texture->GetHeight();

		float ppu = m_IsUI ? 1.0f : m_PixelsPerUnit; // UI – 1:1 с пикселями, игра – PPU
		float renderW = (m_CustomSize.x > 0.0f) ? m_CustomSize.x : (float)texW / ppu;
		float renderH = (m_CustomSize.y > 0.0f) ? m_CustomSize.y : (float)texH / ppu;

		glm::vec3 pos = transform->GetPosition();

		glm::mat4 model;
		if (m_IsUI) {
			// Для UI: позиция задаёт левый верхний угол
			model = glm::translate(glm::mat4(1.0f), pos + glm::vec3(renderW * 0.5f, renderH * 0.5f, 0.0f));
			model = glm::scale(model, glm::vec3(renderW * 0.5f, renderH * 0.5f, 1.0f));
		}
		else {
			// Для игровых объектов: позиция задаёт центр спрайта
			model = glm::translate(glm::mat4(1.0f), pos);
			model = glm::scale(model, glm::vec3(renderW * 0.5f, renderH * 0.5f, 1.0f));
		}

		m_Shader->Bind();
		m_Texture->Bind(0);
		m_Shader->SetUniform1i("u_Texture", 0);
		m_Shader->SetUniformMat4("u_ViewProjection", viewProjection);
		m_Shader->SetUniformMat4("u_Model", model);

		InitQuad();
		glBindVertexArray(s_QuadVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		m_Shader->Unbind();
	}

	void SpriteRenderer::OnStart() {
		InitQuad();
		// Если не установлен шейдер, попробуем взять дефолтный из ресурсов
		if (!m_Shader) {
			auto& rm = Engine::Get().GetResourceManager();
			// Предполагаем, что есть дефолтный шейдер спрайта (зарегистрируем его в SetupLuaBindings или глобально)
			// Пока оставим как есть, шейдер должен быть передан извне.
		}
	}

	void SpriteRenderer::OnUpdate(float deltaTime) {
		if (!m_Texture || !m_Shader) return;

		auto* transform = m_Owner->GetComponent<Transform>();
		if (!transform) return;

		m_Shader->Bind();
		m_Texture->Bind(0);
		m_Shader->SetUniform1i("u_Texture", 0);

		// Получаем камеру из Engine или из Scene? Пока используем синглтон камеры, который мы передадим позже.
		// Для простоты будем использовать глобальную камеру из Engine::Get().GetMainCamera() (добавим позже).
		// Или временно пропустим матрицу вида, будем рисовать в NDC.
		// Но правильнее передать viewProjection. Пока оставим без матрицы.
		// Для реальной 2D-игры понадобится ортокамера.

		m_Shader->SetUniformMat4("u_ViewProjection", glm::mat4(1.0f) /* тут должна быть камера */);
		m_Shader->SetUniformMat4("u_Model", transform->GetModelMatrix());

		glBindVertexArray(s_QuadVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}

} // namespace NK