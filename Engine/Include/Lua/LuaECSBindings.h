#pragma once

namespace NK {
    class LuaManager;

    // v0.2.8: биндинги для ECS API.
    // Lua-скрипты могут создавать entities, добавлять компоненты, рендерить через SpriteRenderSystem.
    //
    // API:
    //   local world = GetECSWorld()           -- глобальная функция
    //   local e = world:CreateEntity("foo")   -- или без имени
    //   world:AddTransform(e, x, y, z)
    //   world:AddSprite(e, r, g, b, a)        -- без текстуры (белая 1x1)
    //   world:SetPosition(e, x, y, z)
    //   world:SetSpriteColor(e, r, g, b, a)
    //   world:SetScale(e, sx, sy)
    //   world:DestroyEntity(e)
    //   world:GetEntityName(e) -> string
    //
    // ВАЖНО: World* — указатель на C++ World, передаётся через GetECSWorld().
    //        Никогда не сохраняй указатель между кадрами (World может пересоздаваться).
    class LuaECSBindings {
    public:
        static void RegisterAll(LuaManager& lua);
    };

} // namespace NK
