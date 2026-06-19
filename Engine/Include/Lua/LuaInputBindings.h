#pragma once

namespace NK {
    class LuaManager;
    class LuaInputBindings {
    public:
        static void RegisterAll(LuaManager& lua);
    };
}