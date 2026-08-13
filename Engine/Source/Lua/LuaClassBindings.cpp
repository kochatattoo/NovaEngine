#include "Lua/LuaClassBindings.h"

#include <sol/sol.hpp>
#include <glm/glm.hpp>
#include "Lua/LuaManager.h"
#include "Renderer/Font.h"
#include "Scene/Scene.h"

namespace NK
{
    void LuaClassBindings::RegisterAll(LuaManager& lua)
    {
        // Scene (v0.3.2: упрощена до обёртки над камерами)
        lua.BindClass<Scene>("Scene",
            sol::no_constructor,
            "OnStart", &Scene::OnStart,
            "OnWindowResized", &Scene::OnWindowResized,
            "GetGameCamera", &Scene::GetGameCamera,
            "GetUICamera", &Scene::GetUICamera
        );

        // Font
        lua.BindClass<Font>("Font",
            "CreateTextTexture", &Font::CreateTextTexture
        );

        // glm::vec2
        lua.BindClass<glm::vec2>("vec2",
            sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
            "x", &glm::vec2::x,
            "y", &glm::vec2::y
        );
    }
}
