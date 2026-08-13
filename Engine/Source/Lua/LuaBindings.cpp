#include "Lua/LuaBindings.h"
#include "Lua/LuaCameraBindings.h"
#include "Lua/LuaClassBindings.h"
#include "Lua/LuaComponentBindings.h"
#include "Lua/LuaInputBindings.h"
#include "Lua/LuaFuncBindings.h"
#include "Lua/LuaECSBindings.h"

namespace NK {

    void LuaBindings::RegisterAll(LuaManager& lua) {
        LuaClassBindings::RegisterAll(lua);
        LuaComponentBindings::RegisterAll(lua);
        LuaCameraBindings::RegisterAll(lua);
        LuaFuncBindings::RegisterAll(lua);
        LuaInputBindings::RegisterAll(lua);
        LuaECSBindings::RegisterAll(lua);
    }

} // namespace NK