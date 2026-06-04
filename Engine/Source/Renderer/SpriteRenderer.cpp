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

	void SpriteRenderer::Render() {
		if (!m_Texture || !m_Shader) return;
		auto* transform = m_Owner->GetComponent<Transform>();
		if (!transform) return;

		// Берём глобальную камеру из Engine (если её нет, используем единичную матрицу)
		glm::mat4 viewProjection = glm::mat4(1.0f);
		// В будущем здесь будет Engine::Get().GetMainCamera().GetViewProjectionMatrix();

		m_Shader->Bind();
		m_Texture->Bind(0);
		m_Shader->SetUniform1i("u_Texture", 0);
		m_Shader->SetUniformMat4("u_ViewProjection", viewProjection);
		m_Shader->SetUniformMat4("u_Model", transform->GetModelMatrix());

		InitQuad(); // убедимся, что квад создан
		glBindVertexArray(s_QuadVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		m_Shader->Unbind();


		GLenum err = glGetError();
		if (err) NK_CORE_ERROR("OpenGL error: %d", err);
	}

	void SpriteRenderer::Render(const glm::mat4& viewProjection) {
		if (!m_Texture || !m_Shader) return;
		auto* transform = m_Owner->GetComponent<Transform>();
		if (!transform) return;

		m_Shader->Bind();
		m_Texture->Bind(0);
		m_Shader->SetUniform1i("u_Texture", 0);
		m_Shader->SetUniformMat4("u_ViewProjection", viewProjection);
		m_Shader->SetUniformMat4("u_Model", transform->GetModelMatrix());

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