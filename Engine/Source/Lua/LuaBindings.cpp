#include "Lua/LuaBindings.h"
#include "Lua/LuaCameraBindings.h"
#include "Lua/LuaClassBindings.h"
#include "Lua/LuaComponentBindings.h"
#include "Lua/LuaInputBindings.h"
#include "Lua/LuaFuncBindings.h"

namespace NK {

    void LuaBindings::RegisterAll(LuaManager& lua) {
        LuaClassBindings::RegisterAll(lua);
        LuaComponentBindings::RegisterAll(lua);
        LuaCameraBindings::RegisterAll(lua);
        LuaFuncBindings::RegisterAll(lua);
        LuaInputBindings::RegisterAll(lua);
    }

} // namespace NK