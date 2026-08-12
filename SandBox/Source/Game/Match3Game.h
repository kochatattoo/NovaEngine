#pragma once
#include <memory>
#include <string>
#include "Game/ECS/Match3System.h"
#include "ECS/World.h"
#include "Lua/LuaManager.h"
#include "Core/Engine.h"

namespace NK {

    // v0.2: Match3Game теперь owns ECS World + Match3System.
    // Match3Board (POD-логика) заменён на Match3System (логика + ECS-entities).
    class Match3Game {
    public:
        Match3Game();
        ~Match3Game() = default;

        void Start();                   // ������ ��� ������ ����������
        void Update(float deltaTime);   // ������ ������ ����

    private:
        void SetupLuaBindings();        // ������������ �������� ��� Match3

        // v0.2: World owned Match3Game'�� (����� ������ �� Engine).
        // ���� ��� ��� ���� — ���� ���� World ��� ���� �������.
        std::unique_ptr<NK::ECS::World> m_World;

        // v0.2: Match3System (������ Match3Board) — owns grid + ECS-entities.
        std::unique_ptr<NK::Game::ECS::Match3System> m_System;

        LuaManager& m_Lua;              // ������ �� Lua-�������� ������
        std::string m_ScriptPath = "assets/scripts/game_match3.lua";
    };

}
