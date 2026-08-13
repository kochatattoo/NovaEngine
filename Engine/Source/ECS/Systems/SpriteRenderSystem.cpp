#include "ECS/Systems/SpriteRenderSystem.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Sprite.h"
#include "Core/Log.h"

#include <glad/gl.h>

namespace NK::ECS {

    // Quad VAO/VBO (статический, инициализируется один раз)
    static uint32_t s_QuadVAO = 0;
    static uint32_t s_QuadVBO = 0;
    static uint32_t s_QuadIBO = 0;
    static bool s_Initialized = false;

    static void InitQuad()
    {
        if (s_Initialized)
        {
            return;
        }

        // Quad: 2D вершины (-0.5..+0.5) + UV
        constexpr float vertices[] = {
            // x, y,    u, v
            -0.5f, -0.5f, 0.0f, 0.0f,  // bottom-left
             0.5f, -0.5f, 1.0f, 0.0f,  // bottom-right
             0.5f,  0.5f, 1.0f, 1.0f,  // top-right
            -0.5f,  0.5f, 0.0f, 1.0f   // top-left
        };
        constexpr uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

        glGenVertexArrays(1, &s_QuadVAO);
        glBindVertexArray(s_QuadVAO);

        glGenBuffers(1, &s_QuadVBO);
        glBindBuffer(GL_ARRAY_BUFFER, s_QuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &s_QuadIBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_QuadIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // layout(location = 0) in vec2 a_Position;
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        // layout(location = 1) in vec2 a_TexCoord;
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);
        s_Initialized = true;
    }

    void SpriteRenderSystem::Render(
        World& world,
        const NK::OrthographicCamera& camera,
        std::shared_ptr<NK::Shader> shader
    )
    {
        if (!shader)
        {
            NK_CORE_WARN("SpriteRenderSystem::Render: shader is null, skipping");
            return;
        }

        InitQuad();

        const glm::mat4& viewProjection = camera.GetViewProjectionMatrix();
        entt::registry& registry = world.GetRegistry();
        auto view = registry.view<TransformComponent, SpriteComponent>();

        for (auto entity : view)
        {
            TransformComponent& transform = view.get<TransformComponent>(entity);
            SpriteComponent& sprite = view.get<SpriteComponent>(entity);

            // Размер: 0.5x0.5 base quad * Scale
            const float renderW = 0.5f * transform.Scale.x;
            const float renderH = 0.5f * transform.Scale.y;

            glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.Position);
            model = glm::scale(model, glm::vec3(renderW, renderH, 1.0f));

            shader->Bind();

            if (sprite.Texture)
            {
                sprite.Texture->Bind(0);
                shader->SetUniform1i("u_Texture", 0);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            shader->SetUniform4f("u_Color", sprite.Color.r, sprite.Color.g, sprite.Color.b, sprite.Color.a);
            shader->SetUniformMat4("u_ViewProjection", viewProjection);
            shader->SetUniformMat4("u_Model", model);

            glBindVertexArray(s_QuadVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        shader->Unbind();
    }

} // namespace NK::ECS
