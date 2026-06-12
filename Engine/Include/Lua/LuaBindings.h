#pragma once

namespace NK {
    class LuaManager;

    class LuaBindings {
    public:
        // Регистрирует все общие для движка C++ классы и функции в Lua
        static void RegisterAll(LuaManager& lua);
    };
}
