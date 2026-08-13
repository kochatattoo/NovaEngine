#pragma once
#include <memory>
#include <string>
#include <glm/glm.hpp>

namespace NK { class Font; }

namespace NK::ECS {

    // v0.3.1: Текст внутри UI-элемента.
    // TODO v0.3.2: рендер через ECS (TextRenderSystem).
    struct UITextComponent
    {
        std::string Text;
        std::shared_ptr<NK::Font> Font;
        float FontSize = 24.0f;
        glm::vec4 Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        // Pivot внутри объекта (0..1, (0.5,0.5) = center)
        glm::vec2 Pivot = glm::vec2(0.5f, 0.5f);
    };

} // namespace NK::ECS
