#pragma once
#include "ECS/World.h"
#include "Renderer/Shader.h"
#include "Renderer/OrthographicCamera.h"
#include <memory>

namespace NK::ECS {

    // v0.2.6: SpriteRenderSystem — рендер entities с (TransformComponent, SpriteComponent)
    // напрямую через OpenGL, без GameObject/Component.
    //
    // Логика позаимствована из NK::SpriteRenderer::Render, но без привязки к GameObject.
    // Quad VAO/VBO — статический (один на всё приложение).
    //
    // Использование:
    //   SpriteRenderSystem::Render(world, camera, shader);
    class SpriteRenderSystem {
    public:
        // Рендерит все entities с (TransformComponent, SpriteComponent) в `world`
        // через указанные `camera` (viewProjection) и `shader` (должен иметь u_ViewProjection, u_Model, u_Texture, u_Color).
        static void Render(
            const World& world,
            const NK::OrthographicCamera& camera,
            std::shared_ptr<NK::Shader> shader
        );
    };

} // namespace NK::ECS
