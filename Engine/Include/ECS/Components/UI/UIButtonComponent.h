#pragma once
#include <functional>
#include <glm/glm.hpp>

namespace NK::ECS {

    // v0.3.1: Кнопка. Callback вызывается при отпускании мыши внутри.
    // OnClick — std::function<void()>, совместим с Lua (sol2 автоматически конвертит).
    struct UIButtonComponent
    {
        using Callback = std::function<void()>;

        Callback OnClick;

        // Цвета для разных состояний
        glm::vec4 ColorNormal  = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
        glm::vec4 ColorHovered = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
        glm::vec4 ColorPressed = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    };

    // Runtime state (обновляется UIButtonSystem каждый кадр).
    struct UIButtonStateComponent
    {
        bool Hovered = false;
        bool Pressed = false;
    };

} // namespace NK::ECS
