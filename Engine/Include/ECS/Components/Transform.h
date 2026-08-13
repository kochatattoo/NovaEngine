#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace NK::ECS {

    // v0.2: POD-компонент позиции/поворота/масштаба entity.
    // Все поля public — стандарт для ECS-компонентов (data-oriented).
    struct TransformComponent
    {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);  // identity
        glm::vec3 Scale    = glm::vec3(1.0f);
    };

} // namespace NK::ECS
