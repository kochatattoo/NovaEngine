#pragma once

namespace NK {
    class LuaManager;
    class LuaCameraBindings {
    public:
        static void RegisterAll(LuaManager& lua);
    };
}