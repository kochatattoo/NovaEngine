#pragma once
#include "Core/Component.h"
#include <functional>
#include <memory>
#include <glm/glm.hpp>

namespace NK {

	class SpriteRenderer;
	class TextRenderer;

	class Button : public Component {
	public:
		Button(GameObject* owner);

		void SetCallback(std::function<void()> callback) { m_Callback = callback; }
		void SetSize(const glm::vec2& size) { m_Size = size; SyncSize(); }
		void SetSize(float x, float y) { m_Size = glm::vec2(x, y); SyncSize(); }
		void SyncSize();

		void OnStart() override;
		void OnUpdate(float deltaTime) override;

	private:
		void TryGetRenderers();   // новый метод
		std::function<void()> m_Callback;
		glm::vec2 m_Size = glm::vec2(100.0f, 50.0f);
		bool m_Hovered = false;
		SpriteRenderer* m_Sprite = nullptr;
		TextRenderer* m_Text = nullptr;
		bool m_RenderersCached = false;
	};

}
