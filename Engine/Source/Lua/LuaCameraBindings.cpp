#include "Lua/LuaCameraBindings.h"
#include "Lua/LuaManager.h"
#include "Renderer/OrthographicCamera.h"

namespace NK
{
    void LuaCameraBindings::RegisterAll(LuaManager& lua)
    {
        // OrthographicCamera
        lua.BindClass<OrthographicCamera>("OrthographicCamera",
            "GetLeft", &OrthographicCamera::GetLeft,
            "GetRight", &OrthographicCamera::GetRight,
            "GetBottom", &OrthographicCamera::GetBottom,
            "GetTop", &OrthographicCamera::GetTop,
            "ScreenToWorldPoint", &OrthographicCamera::ScreenToWorldPoint,
            "ScreenToUIPoint", &OrthographicCamera::ScreenToUIPoint
        );
    }
}
