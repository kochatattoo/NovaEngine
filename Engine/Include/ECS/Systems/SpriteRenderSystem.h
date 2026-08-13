#pragma once
#include "ECS/World.h"
#include "Renderer/OrthographicCamera.h"
#include "Renderer/Shader.h"
#include <memory>

namespace NK::ECS {

    // v0.2.6: SpriteRenderSystem — рендер entities с (TransformComponent, SpriteComponent)
    // напрямую через OpenGL, без GameObject/Component.
    //
    // Quad VAO/VBO — статический (один на всё приложение).
    // Transform::Scale уважается (в отличие от старого SpriteRenderer::Render).
    //
    // Использование:
    //   SpriteRenderSystem::Render(world, camera, shader);
    class SpriteRenderSystem
    {
    public:
        // Принимает World& (non-const) — EnTT view требует неконстантный registry.
        static void Render(
            World& world,
            const NK::OrthographicCamera& camera,
            std::shared_ptr<NK::Shader> shader
        );
    };

} // namespace NK::ECS
