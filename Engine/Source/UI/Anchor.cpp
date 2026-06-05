#include "UI/Anchor.h"
#include "Core/GameObject.h"
#include "Core/Transform.h"
#include <Core/Engine.h>
#include "Core/Scene.h"
#include "Window/Window.h"

namespace NK {

	Anchor::Anchor(GameObject* owner) : Component(owner) {}

	void Anchor::SetPreset(AnchorPreset preset) {
		m_Preset = preset;
		m_UsePreset = true;

		auto* window = Engine::Get().GetWindow();
		UpdatePosition(window->GetWidth(), window->GetHeight());
	}

	void Anchor::SetSize(const glm::vec2& size) {
		m_Size = size;
		auto* window = Engine::Get().GetWindow();
		UpdatePosition(window->GetWidth(), window->GetHeight());
	}

	void Anchor::SetScreenAnchor(float sx, float sy){ 
		m_ScreenAnchorX = sx; m_ScreenAnchorY = sy; 
		m_UsePreset = false;
		auto* window = Engine::Get().GetWindow();
		UpdatePosition(window->GetWidth(), window->GetHeight());
	}
	void Anchor::SetObjectAnchor(float ox, float oy) {

		m_ObjectAnchorX = ox; m_ObjectAnchorY = oy; 
		auto* window = Engine::Get().GetWindow();
		UpdatePosition(window->GetWidth(), window->GetHeight());
	}

	void Anchor::OnStart(){
		// Сразу обновляем позицию при старте
		auto* window = Engine::Get().GetWindow();
		UpdatePosition(window->GetWidth(), window->GetHeight());

		Engine::Get().GetScene().RegisterAnchor(this);
	}

	void Anchor::UpdatePosition(uint32_t windowWidth, uint32_t windowHeight) {
		auto* transform = m_Owner->GetComponent<Transform>();
		if (!transform) return;

		float x = 0.0f, y = 0.0f;
		if (m_UsePreset)
		{
			switch (m_Preset) {
			case AnchorPreset::TopLeft:
				x = 0.0f; y = 0.0f;
				break;
			case AnchorPreset::TopCenter:
				x = (windowWidth - m_Size.x) * 0.5f; y = 0.0f;
				break;
			case AnchorPreset::TopRight:
				x = windowWidth - m_Size.x; y = 0.0f;
				break;
			case AnchorPreset::MiddleLeft:
				x = 0.0f; y = (windowHeight - m_Size.y) * 0.5f;
				break;
			case AnchorPreset::MiddleCenter:
				x = (windowWidth - m_Size.x) * 0.5f; y = (windowHeight - m_Size.y) * 0.5f;
				break;
			case AnchorPreset::MiddleRight:
				x = windowWidth - m_Size.x; y = (windowHeight - m_Size.y) * 0.5f;
				break;
			case AnchorPreset::BottomLeft:
				x = 0.0f; y = windowHeight - m_Size.y;
				break;
			case AnchorPreset::BottomCenter:
				x = (windowWidth - m_Size.x) * 0.5f; y = windowHeight - m_Size.y;
				break;
			case AnchorPreset::BottomRight:
				x = windowWidth - m_Size.x; y = windowHeight - m_Size.y;
				break;
			default:
				break;
			}
		}
		else
		{
			float x = m_ScreenAnchorX * windowWidth - m_ObjectAnchorX * m_Size.x;
			float y = m_ScreenAnchorY * windowHeight - m_ObjectAnchorY * m_Size.y;
		}

		transform->SetPosition(glm::vec3(x, y, 0.0f));
	}
}