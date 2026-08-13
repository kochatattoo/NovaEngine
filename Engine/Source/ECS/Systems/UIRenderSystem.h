#pragma once
#include "ECS/World.h"
#include "Renderer/OrthographicCamera.h"
#include "Renderer/Shader.h"

#include <memory>

namespace NK::ECS {

    // v0.3.1: UIRenderSystem — рисует UI elements (background quads + text) через UI camera.
    //
    // Использует:
    //   - UIElementComponent (Size, Background, ZOrder)
    //   - TransformComponent (Position/Scale — обновляется UIAnchorSystem)
    //   - опционально: UIButtonComponent + UIButtonStateComponent (цвета по state)
    //   - опционально: UITextComponent (текст — TODO v0.3.2)
    class UIRenderSystem
    {
    public:
        static void Render(
            World& world,
            const NK::OrthographicCamera& uiCamera,
            std::shared_ptr<NK::Shader> shader
        );
    };

} // namespace NK::ECS
