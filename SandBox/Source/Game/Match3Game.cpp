#include "Game/Match3Game.h"
#include "Core/Log.h"
#include "Renderer/Texture2D.h"
#include "Renderer/SpriteRenderer.h"
#include "Renderer/Shader.h"
#include "Renderer/OrthographicCamera.h"
#include "Core/ResourceManager.h"
#include "Input/Input.h"
#include "Window/Window.h"
#include <sol/sol.hpp>

namespace NK {

    Match3Game::Match3Game()
        : m_Scene(&Engine::Get().GetScene()),
        m_Lua(Engine::Get().GetLuaManager())
    {
        SetupLuaBindings();
    }

    void Match3Game::SetupLuaBindings() {
        auto& L = m_Lua.GetState();

        // Регистрация Match3Board
        L.new_usertype<Match3Board>("Match3Board",
            sol::constructors<Match3Board(int, int, double, double)>(),
            "FillRandom", &Match3Board::FillRandom,
            "GetTile", &Match3Board::GetTile,
            "SetTile", &Match3Board::SetTile,
            "IsValidCell", &Match3Board::IsValidCell,
            "Swap", &Match3Board::Swap,
            "FindMatches", &Match3Board::FindMatches,
            "RemoveTiles", &Match3Board::RemoveTiles,
            "ApplyGravity", &Match3Board::ApplyGravity,
            "FillEmpty", &Match3Board::FillEmpty,
            "HasPossibleMoves", &Match3Board::HasPossibleMoves,
            "Mix", &Match3Board::Mix,
            "GetCellPosition", [](Match3Board& board, int r, int c) -> std::tuple<float, float> {
                auto pos = board.GetCellPosition(r, c);
                return { pos.x, pos.y };
            },
            "GetRows", &Match3Board::GetRows,
            "GetCols", &Match3Board::GetCols,
            "OnTileChanged", &Match3Board::OnTileChanged
        );

        // Глобальные функции, нужные для игры
        L.set_function("CreateSolidColorTexture", [](int r, int g, int b, int a) -> std::shared_ptr<Texture2D> {
            return Texture2D::CreateSolidColor((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a);
            });
        L.set_function("GetMousePosition", []() -> std::tuple<int, int> {
            int x, y;
            Engine::Get().GetWindow()->GetMouseClientPosition(x, y);
            return { x, y };
            });
        L.set_function("GetWindowWidth", []() { return Engine::Get().GetWindow()->GetWidth(); });
        L.set_function("GetWindowHeight", []() { return Engine::Get().GetWindow()->GetHeight(); });
        L.set_function("IsMouseButtonDown", [](int button) -> bool {
            return Input::IsMouseButtonDown(button);
            });

        // Добавляем доступ к сцене и камере
        L.set_function("GetScene", [this]() -> Scene& { return *m_Scene; });
        // Остальные общие функции (GetTexture, GetShader, Log и т.д.) уже зарегистрированы в Engine
    }

    void Match3Game::Start() {
        m_Lua.RunScript(m_ScriptPath);
        m_Lua.CallFunction("OnStart");
    }

    void Match3Game::Update(float deltaTime) {
        m_Lua.CallFunction("OnUpdate", deltaTime);
    }

}