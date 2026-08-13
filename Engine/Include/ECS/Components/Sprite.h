#pragma once
#include <memory>
#include <glm/glm.hpp>

#include "Renderer/Texture2D.h"

namespace NK::ECS {

    // v0.2: POD-компонент визуального представления entity.
    // Texture опциональна — если nullptr, рендерится только цветной quad.
    struct SpriteComponent
    {
        std::shared_ptr<NK::Texture2D> Texture;
        glm::vec4 Color = glm::vec4(1.0f);
    };

} // namespace NK::ECS
