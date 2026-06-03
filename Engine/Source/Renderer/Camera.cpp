#include "Renderer/Camera.h"

namespace NK {

	Camera::Camera(float fovDegrees, float aspectRatio, float nearPlane, float farPlane)
		: m_Fov(glm::radians(fovDegrees)), m_Aspect(aspectRatio), m_Near(nearPlane), m_Far(farPlane) {
		m_Position = glm::vec3(0.0f, 0.0f, 3.0f);   // камера смотрит с (0,0,3)
		m_Target = glm::vec3(0.0f, 0.0f, 0.0f);     // на начало координат
		Update();
	}

	void Camera::Update() {
		// Перспективная проекция
		m_Projection = glm::perspective(m_Fov, m_Aspect, m_Near, m_Far);
		// Матрица вида: положение, цель, верх
		m_View = glm::lookAt(m_Position, m_Target, m_Up);
	}

	void Camera::SetPosition(const glm::vec3& position) {
		m_Position = position;
	}

	void Camera::SetTarget(const glm::vec3& target) {
		m_Target = target;
	}

} // namespace NK