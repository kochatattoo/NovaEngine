#include "Renderer/TextRenderer.h"
#include "Renderer/Font.h"
#include "Core/GameObject.h"
#include "Core/Transform.h"
#include "Core/ResourceManager.h"
#include "Core/Engine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/gl.h>

namespace NK {

	uint32_t TextRenderer::s_QuadVAO = 0;
	uint32_t TextRenderer::s_QuadVBO = 0;
	bool TextRenderer::s_Initialized = false;

	TextRenderer::TextRenderer(GameObject* owner) : Component(owner) {}

	void TextRenderer::SetFont(std::shared_ptr<Font> font) {
		m_Font = font;
		m_TextChanged = true;
	}

	void TextRenderer::SetText(const std::string& text) {
		m_Text = text;
		m_TextChanged = true;
	}

	void TextRenderer::SetFontSize(float size) {
		m_FontSize = size;
		m_TextChanged = true;
	}

	void TextRenderer::SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		m_Color[0] = r; m_Color[1] = g; m_Color[2] = b; m_Color[3] = a;
		m_TextChanged = true;
	}

	void TextRenderer::OnStart() {
		NK_CORE_INFO("TextRenderer::OnStart called");
		if (!m_Shader) {
			m_Shader = Engine::Get().GetResourceManager().GetShader("DefaultSprite");
			if (!m_Shader) {
				NK_CORE_ERROR("TextRenderer: DefaultSprite shader not found");
			}
			else {
				NK_CORE_INFO("TextRenderer: DefaultSprite shader loaded");
			}
		}
	}

	void TextRenderer::UpdateTexture() {
		/*if (!m_Font) {
			NK_CORE_INFO("UpdateTexture: no font");
			return;
		}
		if (!m_TextChanged) {
			NK_CORE_INFO("UpdateTexture: text unchanged");
			return;
		}
		NK_CORE_INFO("UpdateTexture: creating texture for '%s'", m_Text.c_str());*/

		m_Texture = m_Font->CreateTextTexture(m_Text, m_FontSize, m_Color[0], m_Color[1], m_Color[2], m_Color[3]);

		/*	if (m_Texture) {
				NK_CORE_INFO("UpdateTexture: texture created, size %dx%d", m_Texture->GetWidth(), m_Texture->GetHeight());
			}
			else {
				NK_CORE_INFO("UpdateTexture: FAILED to create texture");
			}*/

		m_TextChanged = false;
	}

	void TextRenderer::Render(const glm::mat4& viewProj) {

		if(!m_Shader)
			NK_CORE_INFO("TextRenderer: shader is NULL");
		if(!m_Font)
			NK_CORE_INFO("TextRenderer: font is NULL");

		// if (!m_Shader || !m_Font) return;
		UpdateTexture();

		/*if (m_Texture)
			NK_CORE_INFO("TextRenderer: texture OK, size %dx%d", m_Texture->GetWidth(), m_Texture->GetHeight());
		else
			NK_CORE_INFO("TextRenderer: texture is NULL");*/

		if (!m_Texture) return;

		auto* transform = m_Owner->GetComponent<Transform>();
		if (!transform) return;

		int texW = m_Texture->GetWidth();
		int texH = m_Texture->GetHeight();
		glm::vec3 pos = transform->GetPosition();

		/*NK_CORE_INFO("Render Text: transform pos=(%.1f, %.1f), tex=%dx%d",
			pos.x, pos.y, texW, texH);*/

		// ћодельна€ матрица: позици€ + смещение на полразмера (чтобы левый верхний угол совпадал с позицией)
		glm::mat4 model = glm::translate(glm::mat4(1.0f), pos + glm::vec3(texW * 0.5f, texH * 0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(texW * 0.5f, texH * 0.5f, 1.0f));

		m_Shader->Bind();
		m_Texture->Bind(0);
		m_Shader->SetUniform1i("u_Texture", 0);
		m_Shader->SetUniformMat4("u_ViewProjection", viewProj);
		m_Shader->SetUniformMat4("u_Model", model);

		InitQuad();
		glBindVertexArray(s_QuadVAO);

		/*NK_CORE_INFO("Drawing text: VAO=%u, texW=%d, texH=%d, pos=(%.1f,%.1f)",
			s_QuadVAO, texW, texH, pos.x, pos.y);*/

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void TextRenderer::InitQuad() {
		if (s_Initialized) return;
		float vertices[] = {
			// позици€ (x,y)    UV (u,v)
			-0.5f, -0.5f,       0.0f, 0.0f,
			 0.5f, -0.5f,       1.0f, 0.0f,
			 0.5f,  0.5f,       1.0f, 1.0f,
			-0.5f,  0.5f,       0.0f, 1.0f
		};
		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

		glGenVertexArrays(1, &s_QuadVAO);
		glBindVertexArray(s_QuadVAO);

		glGenBuffers(1, &s_QuadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, s_QuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		uint32_t ibo;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		glBindVertexArray(0);
		s_Initialized = true;
	}

} // namespace NK