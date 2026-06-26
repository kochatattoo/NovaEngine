#pragma once

namespace NK {
    class LuaManager;
    class LuaClassBindings {
    public:
        static void RegisterAll(LuaManager& lua);
    };
}