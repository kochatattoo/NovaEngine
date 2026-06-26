#pragma once

namespace NK {
	class LuaManager;
    class LuaFuncBindings {
    public:
        static void RegisterAll(LuaManager& lua);
    };
}