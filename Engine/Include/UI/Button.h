#pragma once
#include "Scene/Component.h"
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

		void SetOnClick(std::function<void()> callback) { m_OnClick = callback; }
		void SetOnPointerDown(std::function<void()> callback) { m_OnPointerDown = callback; }
		void SetOnPointerUp(std::function<void()> callback) { m_OnPointerUp = callback; }
		void SetOnPointerEnter(std::function<void()> callback) { m_OnPointerEnter = callback; }
		void SetOnPointerExit(std::function<void()> callback) { m_OnPointerExit = callback; }

		void OnStart() override;
		void OnUpdate(float deltaTime) override;

	private:
		void TryGetRenderers();   // новый метод
		std::function<void()> m_Callback;
		std::function<void()> m_OnClick;
		std::function<void()> m_OnPointerDown;
		std::function<void()> m_OnPointerUp;
		std::function<void()> m_OnPointerEnter;
		std::function<void()> m_OnPointerExit;
		glm::vec2 m_Size = glm::vec2(100.0f, 50.0f);
		bool m_Hovered = false;
		bool m_Pressed = false;  // была ли нажата мышь внутри
		SpriteRenderer* m_Sprite = nullptr;
		TextRenderer* m_Text = nullptr;
		bool m_RenderersCached = false;
	};

}
