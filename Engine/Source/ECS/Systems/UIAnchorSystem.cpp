#include "ECS/Systems/UIAnchorSystem.h"
#include "ECS/Components/UI/UIElementComponent.h"
#include "ECS/Components/Transform.h"

namespace NK::ECS {

    void UIAnchorSystem::Update(World& world, uint32_t windowWidth, uint32_t windowHeight)
    {
        entt::registry& registry = world.GetRegistry();
        auto view = registry.view<UIElementComponent, TransformComponent>();

        const float w = static_cast<float>(windowWidth);
        const float h = static_cast<float>(windowHeight);

        for (auto entity : view)
        {
            UIElementComponent& ui = view.get<UIElementComponent>(entity);
            TransformComponent& transform = view.get<TransformComponent>(entity);

            const float screenX = ui.ScreenAnchor.x * w;
            const float screenY = ui.ScreenAnchor.y * h;

            // Y-invert: screen Y вниз, world Y вверх
            const float worldX = screenX - ui.ObjectAnchor.x * ui.Size.x;
            const float worldY = h - (screenY - ui.ObjectAnchor.y * ui.Size.y) - ui.Size.y;

            transform.Position = glm::vec3(worldX, worldY, 0.0f);
            transform.Scale = glm::vec3(ui.Size.x, ui.Size.y, 1.0f);
        }
    }

} // namespace NK::ECS
