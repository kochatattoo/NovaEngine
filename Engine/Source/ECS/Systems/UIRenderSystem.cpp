#include "ECS/Systems/UIRenderSystem.h"
#include "ECS/Components/UI/UIElementComponent.h"
#include "ECS/Components/UI/UIButtonComponent.h"
#include "ECS/Components/UI/UITextComponent.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Sprite.h"

#include "Core/Log.h"

#include <glad/gl.h>

namespace NK::ECS {

    // Quad VAO/VBO (статический, инициализируется один раз).
    // Отдельный от SpriteRenderSystem, чтобы не зависеть от порядка вызовов.
    static uint32_t s_UIVAO = 0;
    static uint32_t s_UIVBO = 0;
    static uint32_t s_UIIBO = 0;
    static bool s_UIInitialized = false;

    static void InitUIQuad()
    {
        if (s_UIInitialized)
        {
            return;
        }

        constexpr float vertices[] = {
            -0.5f, -0.5f, 0.0f, 0.0f,
             0.5f, -0.5f, 1.0f, 0.0f,
             0.5f,  0.5f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 1.0f
        };
        constexpr uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

        glGenVertexArrays(1, &s_UIVAO);
        glBindVertexArray(s_UIVAO);

        glGenBuffers(1, &s_UIVBO);
        glBindBuffer(GL_ARRAY_BUFFER, s_UIVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &s_UIIBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_UIIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);
        s_UIInitialized = true;
    }

    void UIRenderSystem::Render(
        World& world,
        const NK::OrthographicCamera& uiCamera,
        std::shared_ptr<NK::Shader> shader
    )
    {
        if (!shader)
        {
            NK_CORE_WARN("UIRenderSystem::Render: shader is null");
            return;
        }

        InitUIQuad();

        const glm::mat4& viewProjection = uiCamera.GetViewProjectionMatrix();
        entt::registry& registry = world.GetRegistry();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        auto view = registry.view<UIElementComponent, TransformComponent>();
        for (auto entity : view)
        {
            UIElementComponent& ui = view.get<UIElementComponent>(entity);
            TransformComponent& transform = view.get<TransformComponent>(entity);

            // Цвет: либо кнопка в разных состояниях, либо просто Background
            glm::vec4 color = ui.Background;
            if (registry.all_of<UIButtonComponent>(entity) && registry.all_of<UIButtonStateComponent>(entity))
            {
                UIButtonStateComponent& state = registry.get<UIButtonStateComponent>(entity);
                UIButtonComponent& button = registry.get<UIButtonComponent>(entity);
                if (state.Pressed)      color = button.ColorPressed;
                else if (state.Hovered) color = button.ColorHovered;
                else                    color = button.ColorNormal;
            }

            if (color.a <= 0.0f)
            {
                continue;  // прозрачный — пропускаем
            }

            const float renderW = 0.5f * transform.Scale.x;
            const float renderH = 0.5f * transform.Scale.y;

            glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.Position);
            model = glm::scale(model, glm::vec3(renderW, renderH, 1.0f));

            shader->Bind();
            glBindTexture(GL_TEXTURE_2D, 0);
            shader->SetUniform1i("u_Texture", 0);
            shader->SetUniform4f("u_Color", color.r, color.g, color.b, color.a);
            shader->SetUniformMat4("u_ViewProjection", viewProjection);
            shader->SetUniformMat4("u_Model", model);

            glBindVertexArray(s_UIVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }

        // TODO: рендер текста (UITextComponent) — v0.3.2 (нужен TextRenderSystem + Font pipeline).

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        shader->Unbind();
    }

} // namespace NK::ECS
