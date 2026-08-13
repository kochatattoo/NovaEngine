#include "Lua/LuaComponentBindings.h"
#include "Lua/LuaManager.h"
#include "Scene/Scene.h"
#include "Renderer/Font.h"

namespace NK {

    // v0.3.2: все legacy компоненты (Transform, SpriteRenderer, TextRenderer, Button, Anchor)
    // удалены вместе с GameObject/Component. ECS-компоненты биндятся в LuaECSBindings.cpp.
    //
    // Здесь остался только Scene (обёртка над камерами, см. LuaClassBindings).
    void LuaComponentBindings::RegisterAll(LuaManager& /*lua*/) {
        // no-op
    }
}
