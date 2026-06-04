#include "Renderer/OrthographicCamera.h"

namespace NK {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_Left(left), m_Right(right), m_Bottom(bottom), m_Top(top) {
		m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		RecalculateView();
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top) {
		m_Left = left; m_Right = right; m_Bottom = bottom; m_Top = top;
		m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		RecalculateView();
	}

	void OrthographicCamera::RecalculateView() {
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
			glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0, 0, 1));
		m_View = glm::inverse(transform);
		m_ViewProjection = m_Projection * m_View;
	}

	void OrthographicCamera::OnWindowResized(uint32_t width, uint32_t height) {
		// Сохраняем соотношение сторон так, чтобы видимая область сохраняла нужный размер по высоте, а ширина подстраивалась
		float aspectRatio = (float)width / (float)height;
		// Предположим, мы хотим видеть 10 единиц по вертикали
		float verticalSize = 10.0f;
		float horizontalSize = verticalSize * aspectRatio;

		SetProjection(-horizontalSize / 2.0f, horizontalSize / 2.0f,
			-verticalSize / 2.0f, verticalSize / 2.0f);
	}

} // namespace NK