#pragma once

namespace NK {
    class LuaManager;
    class LuaComponentBindings {
    public:
        static void RegisterAll(LuaManager& lua);
    };
}