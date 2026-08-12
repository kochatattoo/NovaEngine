#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace NK::ECS {

    struct TransformComponent {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // identity
        glm::vec3 Scale    = glm::vec3(1.0f);
    };

}