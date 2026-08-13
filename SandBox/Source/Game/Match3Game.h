#pragma once
#include <memory>
#include <string>
#include "Game/ECS/Match3System.h"
#include "ECS/World.h"
#include "Lua/LuaManager.h"
#include "Core/Engine.h"

namespace NK {

    // v0.2: Match3Game owns ECS World + Match3System.
    // v0.2.6: + owns sprite shader + 1x1 white texture, рендерит плитки через SpriteRenderSystem.
    class Match3Game {
    public:
        Match3Game();
        ~Match3Game() = default;

        void Start();                   // ������ ��� ������ ����������
        void Update(float deltaTime);   // ������ ������ ����
        void Render();                  // v0.2.6: ������ ECS-������� ����� SpriteRenderSystem

    private:
        void SetupLuaBindings();        // ������������ �������� ��� Match3

        // v0.2: World owned Match3Game'�� (����� ������ �� Engine).
        std::unique_ptr<NK::ECS::World> m_World;

        // v0.2: Match3System (������ Match3Board) — owns grid + ECS-entities.
        std::unique_ptr<NK::Game::ECS::Match3System> m_System;

        // v0.2.6: sprite shader (используется SpriteRenderSystem).
        std::shared_ptr<NK::Shader> m_SpriteShader;
        // v0.2.6: 1x1 белая текстура (общая для всех плиток; цвет в SpriteComponent::Color).
        std::shared_ptr<NK::Texture2D> m_WhiteTexture;

        LuaManager& m_Lua;              // ������ �� Lua-�������� ������
        std::string m_ScriptPath = "assets/scripts/game_match3.lua";
    };

}
