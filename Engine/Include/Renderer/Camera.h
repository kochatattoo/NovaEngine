#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace NK {

	class Camera {
	public:
		Camera(float fovDegrees, float aspectRatio, float nearPlane, float farPlane);

		// Пересчитывает матрицы (вызывать, когда изменилось положение/угол)
		void Update();

		// Установить позицию и цель
		void SetPosition(const glm::vec3& position);
		void SetTarget(const glm::vec3& target);

		// Геттеры для матриц
		const glm::mat4& GetViewMatrix() const { return m_View; }
		const glm::mat4& GetProjectionMatrix() const { return m_Projection; }
		glm::mat4 GetViewProjectionMatrix() const { return m_Projection * m_View; }

		// Для внешнего управления (можно менять положение напрямую)
		glm::vec3& GetPosition() { return m_Position; }
		const glm::vec3& GetPosition() const { return m_Position; }

	private:
		glm::mat4 m_Projection; // матрица проекции (перспектива)
		glm::mat4 m_View;       // матрица вида (положение и ориентация)

		float m_Fov, m_Aspect, m_Near, m_Far;
		glm::vec3 m_Position;
		glm::vec3 m_Target;
		glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f); // направление "вверх"
	};

} // namespace NK
