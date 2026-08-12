#include "Game/Match3Game.h"
#include "Core/Log.h"
#include "Renderer/Texture2D.h"
#include <sol/sol.hpp>

namespace NK {

    Match3Game::Match3Game()
        : m_Lua(Engine::Get().GetLuaManager())
    {
        // v0.2: создаём локальный World + Match3System.
        // Match3System owns 100 tile-entities (см. Start()).
        m_World  = std::make_unique<NK::ECS::World>();
        m_System = std::make_unique<NK::Game::ECS::Match3System>(*m_World, 10, 10, 64.0f, 100.0f);

        SetupLuaBindings();
    }

    void Match3Game::SetupLuaBindings() {
        // v0.2: биндим Match3System (раньше Match3Board). API сохранён 1:1 — Lua не заметит.
        m_Lua.BindClass<NK::Game::ECS::Match3System>("Match3Board",
            // Конструктор НЕ биндим (Match3System создаётся в C++). Lua получает указатель через реестр.
            sol::no_constructor,

            "FillRandom",         &NK::Game::ECS::Match3System::FillRandom,
            "GetTile",            &NK::Game::ECS::Match3System::GetTile,
            "SetTile",            &NK::Game::ECS::Match3System::SetTile,
            "IsValidCell",        &NK::Game::ECS::Match3System::IsValidCell,
            "Swap",               &NK::Game::ECS::Match3System::Swap,
            "FindMatches",        &NK::Game::ECS::Match3System::FindMatches,
            "RemoveTiles",        &NK::Game::ECS::Match3System::RemoveTiles,
            "ApplyGravity",       &NK::Game::ECS::Match3System::ApplyGravity,
            "FillEmpty",          &NK::Game::ECS::Match3System::FillEmpty,
            "HasPossibleMoves",   &NK::Game::ECS::Match3System::HasPossibleMoves,
            "Mix",                &NK::Game::ECS::Match3System::Mix,
            "GetCellPosition", [](NK::Game::ECS::Match3System& sys, int r, int c) -> std::tuple<float, float> {
                auto pos = sys.GetCellPosition(r, c);
                return { pos.x, pos.y };
            },
            "GetRows",            &NK::Game::ECS::Match3System::GetRows,
            "GetCols",            &NK::Game::ECS::Match3System::GetCols,
            "OnTileChanged",      &NK::Game::ECS::Match3System::OnTileChanged
        );

        // Глобальная функция: получить указатель на board (Match3System) для Lua.
        // Lua-скрипт пишет: local board = GetBoard()
        m_Lua.RegisterFunction("GetBoard", [this]() -> NK::Game::ECS::Match3System* {
            return m_System.get();
        });

        // Создание цветной 1x1 текстуры (специфично для Match3) — оставлено как было.
        m_Lua.RegisterFunction("CreateSolidColorTexture", [](int r, int g, int b, int a) -> std::shared_ptr<Texture2D> {
            return Texture2D::CreateSolidColor((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a);
        });
    }

    void Match3Game::Start() {
        // v0.2: сначала грузим скрипт + ставим OnTileChanged callback,
        // ПОТОМ спавним entities. Иначе 100 событий FillRandom потеряются.
        m_Lua.RunScript(m_ScriptPath);
        m_Lua.CallFunction("OnStart");
        m_System->Start();
    }

    void Match3Game::Update(float deltaTime) {
        m_System->Update(deltaTime);
        m_Lua.CallFunction("OnUpdate", deltaTime);
    }

}
