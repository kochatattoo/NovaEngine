#include "ECS/Systems/UIButtonSystem.h"
#include "ECS/Components/UI/UIElementComponent.h"
#include "ECS/Components/UI/UIButtonComponent.h"
#include "ECS/Components/Transform.h"

#include "Input/InputSystem.h"
#include "Input/KeyCode/KeyCodes.h"

namespace NK::ECS {

    void UIButtonSystem::Update(World& world, int mouseX, int mouseY, uint32_t /*windowWidth*/, uint32_t /*windowHeight*/)
    {
        entt::registry& registry = world.GetRegistry();
        auto view = registry.view<UIElementComponent, UIButtonComponent, UIButtonStateComponent, TransformComponent>();

        InputSystem& input = InputSystem::Get();
        const bool mouseDown = input.GetMouseButton(MouseButton::Left);

        for (auto entity : view)
        {
            UIElementComponent& ui = view.get<UIElementComponent>(entity);
            UIButtonComponent& button = view.get<UIButtonComponent>(entity);
            UIButtonStateComponent& state = view.get<UIButtonStateComponent>(entity);
            TransformComponent& transform = view.get<TransformComponent>(entity);

            const float x = transform.Position.x;
            const float y = transform.Position.y;
            const float w = ui.Size.x;
            const float h = ui.Size.y;

            const bool inside = (mouseX >= x && mouseX <= x + w &&
                                 mouseY >= y && mouseY <= y + h);

            if (inside && !state.Hovered)
            {
                state.Hovered = true;
            }
            else if (!inside && state.Hovered)
            {
                state.Hovered = false;
            }

            if (mouseDown)
            {
                if (inside && !state.Pressed)
                {
                    state.Pressed = true;
                }
            }
            else
            {
                if (state.Pressed)
                {
                    if (inside && button.OnClick)
                    {
                        button.OnClick();
                    }
                    state.Pressed = false;
                }
            }
        }
    }

} // namespace NK::ECS
