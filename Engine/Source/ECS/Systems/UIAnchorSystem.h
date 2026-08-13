#pragma once
#include "ECS/World.h"

#include <cstdint>

namespace NK::ECS {

    // v0.3.1: UIAnchorSystem — обновляет Position в TransformComponent на основе
    // ScreenAnchor/ObjectAnchor.
    //
    // Логика (Y-invert):
    //   worldX = screenX - ObjectAnchor.x * Size.x
    //   worldY = windowH - (screenY - ObjectAnchor.y * Size.y) - Size.y
    //
    // Запускать в OnUpdate (или после Window resize).
    class UIAnchorSystem
    {
    public:
        static void Update(World& world, uint32_t windowWidth, uint32_t windowHeight);
    };

} // namespace NK::ECS
