#include "Game/Match3Game.h"
#include "Game/ECS/Match3BoardProxy.h"
#include "Core/Log.h"
#include "Renderer/Texture2D.h"
#include "Renderer/Shader.h"
#include "Renderer/OrthographicCamera.h"
#include "Core/Engine.h"
#include "Scene/Scene.h"
#include "ECS/Systems/SpriteRenderSystem.h"
#include <sol/sol.hpp>

namespace NK {

    // Шейдер для ECS-спрайтов (используется SpriteRenderSystem)
    static const char* s_SpriteVertexSrc = R"(
#version 330 core
layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;
uniform mat4 u_ViewProjection;
uniform mat4 u_Model;
out vec2 v_TexCoord;
void main() {
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 0.0, 1.0);
    v_TexCoord = a_TexCoord;
}
)";

    static const char* s_SpriteFragmentSrc = R"(
#version 330 core
in vec2 v_TexCoord;
out vec4 FragColor;
uniform sampler2D u_Texture;
uniform vec4 u_Color;
void main() {
    vec4 tex = texture(u_Texture, v_TexCoord);
    FragColor = tex * u_Color;
}
)";

    Match3Game::Match3Game()
        : m_Lua(Engine::Get().GetLuaManager())
    {
        // v0.2: создаём локальный World + Match3System.
        m_World  = std::make_unique<NK::ECS::World>();
        m_System = std::make_unique<NK::Game::ECS::Match3System>(*m_World, 10, 10, 64.0f, 100.0f);

        // v0.2.6: создаём sprite shader + 1x1 белую текстуру ОДИН раз.
        // Все 100 плиток будут использовать эту текстуру + свой Color из SpriteComponent.
        m_SpriteShader = std::make_shared<NK::Shader>(s_SpriteVertexSrc, s_SpriteFragmentSrc);
        m_WhiteTexture = NK::Texture2D::CreateSolidColor(255, 255, 255, 255);
        m_System->SetSpriteTexture(m_WhiteTexture);

        SetupLuaBindings();
    }

    void Match3Game::SetupLuaBindings() {
        // v0.2: биндим Match3BoardProxy (POD-обёртку вокруг Match3System) под именем
        // "Match3Board" для обратной совместимости с Lua-скриптом.
        m_Lua.BindClass<NK::Game::ECS::Match3BoardProxy>("Match3Board",
            sol::constructors<NK::Game::ECS::Match3BoardProxy(NK::Game::ECS::Match3System*)>(),

            "FillRandom",         &NK::Game::ECS::Match3BoardProxy::FillRandom,
            "GetTile",            &NK::Game::ECS::Match3BoardProxy::GetTile,
            "SetTile",            &NK::Game::ECS::Match3BoardProxy::SetTile,
            "IsValidCell",        &NK::Game::ECS::Match3BoardProxy::IsValidCell,
            "Swap",               &NK::Game::ECS::Match3BoardProxy::Swap,
            "FindMatches",        &NK::Game::ECS::Match3BoardProxy::FindMatches,
            "RemoveTiles",        &NK::Game::ECS::Match3BoardProxy::RemoveTiles,
            "ApplyGravity",       &NK::Game::ECS::Match3BoardProxy::ApplyGravity,
            "FillEmpty",          &NK::Game::ECS::Match3BoardProxy::FillEmpty,
            "HasPossibleMoves",   &NK::Game::ECS::Match3BoardProxy::HasPossibleMoves,
            "Mix",                &NK::Game::ECS::Match3BoardProxy::Mix,
            "GetCellPosition",    &NK::Game::ECS::Match3BoardProxy::GetCellPosition,
            "GetRows",            &NK::Game::ECS::Match3BoardProxy::GetRows,
            "GetCols",            &NK::Game::ECS::Match3BoardProxy::GetCols,

            "OnTileChanged", sol::property(
                &NK::Game::ECS::Match3BoardProxy::GetOnTileChanged,
                &NK::Game::ECS::Match3BoardProxy::SetOnTileChanged
            )
        );

        // Глобальная функция: получить board-прокси для Lua.
        m_Lua.RegisterFunction("GetBoard", [this]() -> NK::Game::ECS::Match3BoardProxy {
            return NK::Game::ECS::Match3BoardProxy{ m_System.get() };
        });

        // v0.2.8: глобальная функция — получить World* для прямого доступа к ECS из Lua.
        m_Lua.RegisterFunction("GetECSWorld", [this]() -> NK::ECS::World* {
            return m_World.get();
        });
    }

    void Match3Game::Start() {
        // v0.2.6: НЕ нужен post-build Lua-OnStart с GameObject'ами — рендер из ECS.
        // Lua нужен только для input (мышь + клики).
        m_Lua.RunScript(m_ScriptPath);
        m_Lua.CallFunction("SafeOnStart");
        m_System->Start();
    }

    void Match3Game::Update(float deltaTime) {
        m_System->Update(deltaTime);
        m_Lua.CallFunction("OnUpdate", deltaTime);
    }

    void Match3Game::Render() {
        // v0.2.6: рендер всех плиток из ECS через SpriteRenderSystem.
        // Render вызывается из Game.cpp::OnRender (после Scene::OnRender game objects).
        if (!m_World || !m_SpriteShader) return;

        Scene& scene = Engine::Get().GetScene();
        const auto& camera = scene.GetGameCamera();

        NK::ECS::SpriteRenderSystem::Render(*m_World, camera, m_SpriteShader);
    }

}
