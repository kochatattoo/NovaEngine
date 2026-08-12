#include "Game/Match3Game.h"
#include "Game/ECS/Match3BoardProxy.h"
#include "Core/Log.h"
#include "Renderer/Texture2D.h"
#include <sol/sol.hpp>

namespace NK {

    Match3Game::Match3Game()
        : m_Lua(Engine::Get().GetLuaManager())
    {
        // v0.2: создаём локальный World + Match3System.
        m_World  = std::make_unique<NK::ECS::World>();
        m_System = std::make_unique<NK::Game::ECS::Match3System>(*m_World, 10, 10, 64.0f, 100.0f);

        SetupLuaBindings();
    }

    void Match3Game::SetupLuaBindings() {
        // v0.2: биндим Match3BoardProxy (POD-обёртку вокруг Match3System) под именем
        // "Match3Board" для обратной совместимости с Lua-скриптом.
        //
        // Почему не BindClass<Match3System> напрямую:
        // 1. sol2 не умеет биндить std::function-поле как property без явного
        //    sol::property(getter, setter) — нужна обёртка.
        // 2. sol::no_constructor ломает остальные property-биндинги.
        // Решение: Match3BoardProxy — лёгкий POD, держит raw ptr, проксирует вызовы.
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

            // OnTileChanged — explicit sol::property для std::function
            "OnTileChanged", sol::property(
                &NK::Game::ECS::Match3BoardProxy::GetOnTileChanged,
                &NK::Game::ECS::Match3BoardProxy::SetOnTileChanged
            )
        );

        // Глобальная функция: получить board-прокси для Lua.
        m_Lua.RegisterFunction("GetBoard", [this]() -> NK::Game::ECS::Match3BoardProxy {
            return NK::Game::ECS::Match3BoardProxy{ m_System.get() };
        });

        // Создание цветной 1x1 текстуры (специфично для Match3) — оставлено как было.
        m_Lua.RegisterFunction("CreateSolidColorTexture", [](int r, int g, int b, int a) -> std::shared_ptr<Texture2D> {
            return Texture2D::CreateSolidColor((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a);
        });
    }

    void Match3Game::Start() {
        // v0.2: сначала грузим скрипт + ставим OnTileChanged callback,
        // ПОТОМ спавним entities. Иначе 100 событий FillRandom потеряются.
        NK_CORE_INFO("Match3Game::Start: loading script '%s'", m_ScriptPath.c_str());
        if (!m_Lua.RunScript(m_ScriptPath)) {
            NK_CORE_ERROR("Match3Game::Start: RunScript failed, aborting");
            return;
        }
        NK_CORE_INFO("Match3Game::Start: calling SafeOnStart");
        m_Lua.CallFunction("SafeOnStart");
        NK_CORE_INFO("Match3Game::Start: starting Match3System (spawn 100 entities)");
        m_System->Start();
    }

    void Match3Game::Update(float deltaTime) {
        m_System->Update(deltaTime);
        m_Lua.CallFunction("OnUpdate", deltaTime);
    }

}
