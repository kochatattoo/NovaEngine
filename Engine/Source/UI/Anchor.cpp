#include "UI/Anchor.h"
#include "Core/GameObject.h"
#include "Core/Transform.h"
#include <Core/Engine.h>
#include "Core/Scene.h"

namespace NK {

	Anchor::Anchor(GameObject* owner) : Component(owner) {}

	void Anchor::SetPreset(AnchorPreset preset) {
		m_Preset = preset;
	}

	void Anchor::OnStart(){
		Engine::Get().GetScene().RegisterAnchor(this);
	}

	void Anchor::UpdatePosition(uint32_t windowWidth, uint32_t windowHeight) {
		auto* transform = m_Owner->GetComponent<Transform>();
		if (!transform) return;

		float x = 0.0f, y = 0.0f;
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
		transform->SetPosition(glm::vec3(x, y, 0.0f));
	}

}