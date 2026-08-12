#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "Renderer/Texture2D.h"

namespace NK::ECS {

    struct SpriteComponent {
        std::shared_ptr<Texture2D> Texture;
        glm::vec4 Color = glm::vec4(1.0f);
        // Дополнительные параметры рендеринга можно добавить позже
    };

}