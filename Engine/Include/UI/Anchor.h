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
		void SetSize(const glm::vec2& size) { m_Size = size; }
		void OnStart();
		void UpdatePosition(uint32_t windowWidth, uint32_t windowHeight);

	private:
		AnchorPreset m_Preset = AnchorPreset::TopLeft;
		glm::vec2 m_Size = glm::vec2(100.0f, 50.0f);
	};

}