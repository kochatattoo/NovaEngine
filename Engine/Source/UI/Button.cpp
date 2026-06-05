#include "UI/Button.h"
#include "Core/GameObject.h"
#include "Core/Transform.h"
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

		if (inside) {
			if (!m_Hovered) {
				m_Hovered = true;
			}
			if (Input::IsMouseButtonDown(VK_LBUTTON)) {
				if (m_Callback) {
					m_Callback();
					NK_INFO("Button clicked!");
				}
			}
		}
		else {
			m_Hovered = false;
		}
	}

	void Button::SyncSize() {
		if (m_Sprite) {
			m_Sprite->SetCustomSize(m_Size);
		}
	}
}