#pragma once
#include "ECS/World.h"

#include <cstdint>

namespace NK::ECS {

    // v0.3.1: UIButtonSystem — обновляет UIButtonStateComponent (Hovered/Pressed),
    // вызывает OnClick callback при отпускании внутри.
    //
    // mouseX/mouseY — клиентские координаты.
    class UIButtonSystem
    {
    public:
        static void Update(World& world, int mouseX, int mouseY, uint32_t windowWidth, uint32_t windowHeight);
    };

} // namespace NK::ECS
