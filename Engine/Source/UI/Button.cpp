#include "UI/Button.h"
#include "Core/GameObject.h"
#include "Core/Transform.h"
#include "Renderer/SpriteRenderer.h"
#include "Renderer/TextRenderer.h"
#include "Input/Input.h"

namespace NK {

	Button::Button(GameObject* owner) : Component(owner) {}

	void Button::OnStart() {
		m_Sprite = m_Owner->GetComponent<SpriteRenderer>();
		m_Text = m_Owner->GetComponent<TextRenderer>();
	}

	void Button::OnUpdate(float deltaTime) {
		auto* transform = m_Owner->GetComponent<Transform>();
		if (!transform) return;

		glm::vec3 pos = transform->GetPosition();
		int mouseX, mouseY;
		Input::GetMousePosition(mouseX, mouseY);

		bool inside = (mouseX >= pos.x && mouseX <= pos.x + m_Size.x &&
			mouseY >= pos.y && mouseY <= pos.y + m_Size.y);

		if (inside) {
			if (!m_Hovered) {
				m_Hovered = true;
				// Здесь можно сменить текстуру на hover-версию
			}
			if (Input::IsMouseButtonDown(VK_LBUTTON)) {
				if (m_Callback) m_Callback();
			}
		}
		else if (m_Hovered) {
			m_Hovered = false;
			// Вернуть нормальную текстуру
		}
	}

}