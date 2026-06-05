#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace NK {

	class OrthographicCamera {
	public:
		// left, right, bottom, top – границы видимой области
		OrthographicCamera(float left, float right, float bottom, float top);

		// Установить границы
		void SetProjection(float left, float right, float bottom, float top);

		// Позиция и поворот (для будущего использования)
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateView(); }
		const glm::vec3& GetPosition() const { return m_Position; }

		void SetRotation(float angleDegrees) { m_Rotation = glm::radians(angleDegrees); RecalculateView(); }
		float GetRotationDegrees() const { return glm::degrees(m_Rotation); }

		// Матрица "проекция * вид"
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjection; }

		// Обновить размеры (вызывать при изменении окна)
		void OnWindowResized(uint32_t width, uint32_t height);

	private:
		void RecalculateView();

		glm::mat4 m_Projection;
		glm::mat4 m_View;
		glm::mat4 m_ViewProjection;

		glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
		float m_Rotation = 0.0f; // радианы

		float m_Left, m_Right, m_Bottom, m_Top;
	};

} // namespace NK
