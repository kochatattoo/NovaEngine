#pragma once
#include <glm/glm.hpp>

namespace NK::ECS {

    // v0.3.1: UI-элемент с якорем, размером и фоном.
    //
    // Позиция вычисляется UIAnchorSystem на основе ScreenAnchor и ObjectAnchor.
    // Background — цвет заливки (если alpha=0 — фон не рисуется).
    // ZOrder — порядок отрисовки (выше = поверх).
    struct UIElementComponent
    {
        // (0,0)=top-left, (1,1)=bottom-right (screen coords)
        glm::vec2 ScreenAnchor = glm::vec2(0.0f, 0.0f);
        // Точка в объекте, которая привязывается
        glm::vec2 ObjectAnchor = glm::vec2(0.0f, 0.0f);

        // Размер (пиксели)
        glm::vec2 Size = glm::vec2(100.0f, 50.0f);

        // Фон (RGBA)
        glm::vec4 Background = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

        int ZOrder = 0;
    };

} // namespace NK::ECS
