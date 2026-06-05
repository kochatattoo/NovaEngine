#pragma once
#include "Core/Component.h"
#include <glm/glm.hpp>

namespace NK {

	class Transform : public Component {
	public:
		Transform(GameObject* owner);

		void SetPosition(const glm::vec3& pos) { m_Position = pos; }
		const glm::vec3& GetPosition() const { return m_Position; }

		void SetRotation(float angleDegrees) { m_Rotation = glm::radians(angleDegrees); }
		float GetRotationDegrees() const { return glm::degrees(m_Rotation); }

		void SetScale(const glm::vec2& scale) { m_Scale = scale; }
		const glm::vec2& GetScale() const { return m_Scale; }

		// Матрица модели (из локальных координат в мировые)
		glm::mat4 GetModelMatrix() const;

	private:
		glm::vec3 m_Position = glm::vec3(0.0f);
		float m_Rotation = 0.0f;   // радианы
		glm::vec2 m_Scale = glm::vec2(1.0f);
	};

} // namespace NK
