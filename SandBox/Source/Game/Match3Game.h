#pragma once
#include <memory>
#include <string>
#include "Game/Match3Board.h"
#include "Core/LuaManager.h"
#include "Core/Scene.h"
#include "Core/Engine.h"

namespace NK {

    class Match3Game {
    public:
        Match3Game();
        ~Match3Game() = default;

        void Start();                   // вызывается при старте приложения
        void Update(float deltaTime);   // вызывается каждый кадр

    private:
        void SetupLuaBindings();        // регистрирует биндинги для Match3

        std::unique_ptr<Match3Board> m_Board;
        Scene* m_Scene;                 // ссылка на сцену движка (удобно)
        LuaManager& m_Lua;              // ссылка на Lua-менеджер движка
        std::string m_ScriptPath = "assets/scripts/game_match3.lua";
    };

}