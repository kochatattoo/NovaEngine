#include "Core/Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace NK {

	Transform::Transform(GameObject* owner) : Component(owner) {}

	glm::mat4 Transform::GetModelMatrix() const {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, m_Position);
		model = glm::rotate(model, m_Rotation, glm::vec3(0.0f, 0.0f, 1.0f)); // вращение вокруг Z
		model = glm::scale(model, glm::vec3(m_Scale, 1.0f));
		return model;
	}

}