#pragma once
#include "Core/Component.h"
#include <glm/glm.hpp>
#include <vector>

namespace NK {

	enum class AnchorPreset {
		TopLeft, TopCenter, TopRight,
		MiddleLeft, MiddleCenter, MiddleRight,
		BottomLeft, BottomCenter, BottomRight,
		StretchAll // пока не реализуем, но зарезервируем
	};

	class Anchor : public Component {
	public:
		Anchor(GameObject* owner);
		void SetPreset(AnchorPreset preset);
		void SetScreenAnchor(float sx, float sy);
		void SetObjectAnchor(float ox, float oy);
		void SetSize(const glm::vec2& size);
		void OnStart();
		void UpdatePosition(uint32_t windowWidth, uint32_t windowHeight);

	private:
		AnchorPreset m_Preset = AnchorPreset::TopLeft;
		float m_ScreenAnchorX = 0.0f;
		float m_ScreenAnchorY = 0.0f;
		float m_ObjectAnchorX = 0.0f;
		float m_ObjectAnchorY = 0.0f;
		glm::vec2 m_Size = glm::vec2(100.0f, 50.0f);
		bool m_UsePreset = false;
	};

}