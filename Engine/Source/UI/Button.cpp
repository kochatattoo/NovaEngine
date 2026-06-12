#include "UI/Button.h"
#include "Scene/GameObject.h"
#include "Scene/Transform.h"
#include "Renderer/SpriteRenderer.h"
#include "Renderer/TextRenderer.h"
#include "Input/Input.h"
#include <Core/Log.h>
#include <Core/Engine.h>
#include <Window/Window.h>

namespace NK {

	Button::Button(GameObject* owner) : Component(owner) {}

	void Button::TryGetRenderers() {
		if (!m_RenderersCached) {
			if (!m_Sprite) m_Sprite = m_Owner->GetComponent<SpriteRenderer>();
			if (!m_Text)   m_Text = m_Owner->GetComponent<TextRenderer>();
			if (m_Sprite && m_Text) m_RenderersCached = true;
		}
	}

	void Button::OnStart() {
		TryGetRenderers();
	}

	void Button::OnUpdate(float deltaTime) {
		TryGetRenderers();

		auto* transform = m_Owner->GetComponent<Transform>();
		if (!transform) return;

		if (m_Sprite) {
			SyncSize();
		}

		int mouseX, mouseY;
		Engine::Get().GetWindow() -> GetMouseClientPosition(mouseX, mouseY);

		glm::vec3 pos = transform->GetPosition();
		bool inside = (mouseX >= pos.x && mouseX <= pos.x + m_Size.x &&
			mouseY >= pos.y && mouseY <= pos.y + m_Size.y);

		// Обработка наведения
		if (inside && !m_Hovered) {
			m_Hovered = true;
			if (m_OnPointerEnter) m_OnPointerEnter();
		}
		else if (!inside && m_Hovered) {
			m_Hovered = false;
			if (m_OnPointerExit) m_OnPointerExit();
		}

		// Обработка нажатия/отпускания мыши
		if (Input::IsMouseButtonDown(VK_LBUTTON)) {
			if (inside && !m_Pressed) {
				m_Pressed = true;
				if (m_OnPointerDown) m_OnPointerDown();
			}
		}
		else {
			if (m_Pressed) {
				if (inside) {
					if (m_OnClick) m_OnClick();
					NK_INFO("Button clicked!");
				}
				m_Pressed = false;
				if (m_OnPointerUp) m_OnPointerUp();
			}
		}
	}

	void Button::SyncSize() {
		if (m_Sprite) {
			m_Sprite->SetCustomSize(m_Size);
		}
	}
}