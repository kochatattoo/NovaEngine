#include "Lua/LuaECSBindings.h"
#include "Lua/LuaManager.h"
#include "ECS/World.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/Sprite.h"
#include "ECS/Components/NameComponent.h"
#include <sol/sol.hpp>
#include <glm/glm.hpp>
#include <cstdint>
#include <string>

namespace NK {

    // World биндится как userdata через указатель.
    // sol2 умеет биндить указатель на T, оборачивая в userdata. T должен иметь публичные
    // методы (которые биндим напрямую) или лямбды (для перегрузок и обёрток).
    //
    // entt::entity — uint32_t. sol2 не умеет с ним ООП-стиль в Lua, поэтому
    // все операции над entity — методы World (принимают entity как первый аргумент).

    void LuaECSBindings::RegisterAll(LuaManager& lua) {
        // === World (без конструктора — создаётся в C++) ===
        lua.BindClass<ECS::World>("ECSWorld",
            sol::no_constructor,

            // Создание entities
            "CreateEntity", sol::overload(
                [](ECS::World& w) -> entt::entity { return w.CreateEntity(); },
                [](ECS::World& w, const std::string& name) -> entt::entity { return w.CreateEntity(name); }
            ),

            "GetEntityByName", &ECS::World::GetEntityByName,
            "RenameEntity", &ECS::World::RenameEntity,
            "GetEntityName", &ECS::World::GetEntityName,
            "DestroyEntity", &ECS::World::DestroyEntity,
            "Clear", &ECS::World::Clear,

            // === Components: Transform ===
            "AddTransform", [](ECS::World& w, entt::entity e, float x, float y, float z) {
                auto& t = w.AddComponent<ECS::TransformComponent>(e);
                t.Position = glm::vec3(x, y, z);
                return &t;
            },

            "SetPosition", [](ECS::World& w, entt::entity e, float x, float y, float z) {
                if (!w.HasComponent<ECS::TransformComponent>(e)) return;
                w.GetComponent<ECS::TransformComponent>(e).Position = glm::vec3(x, y, z);
            },
            "GetPosition", [&lua](ECS::World& w, entt::entity e) -> sol::table {
                sol::state_view sv = sol::state_view(lua.GetState());
                sol::table t = sv.create_table();
                if (!w.HasComponent<ECS::TransformComponent>(e)) {
                    t["x"] = 0.0f; t["y"] = 0.0f; t["z"] = 0.0f;
                    return t;
                }
                auto& p = w.GetComponent<ECS::TransformComponent>(e).Position;
                t["x"] = p.x; t["y"] = p.y; t["z"] = p.z;
                return t;
            },

            "SetScale", [](ECS::World& w, entt::entity e, float sx, float sy) {
                if (!w.HasComponent<ECS::TransformComponent>(e)) return;
                w.GetComponent<ECS::TransformComponent>(e).Scale = glm::vec3(sx, sy, 1.0f);
            },

            // === Components: Sprite ===
            "AddSprite", [](ECS::World& w, entt::entity e, float r, float g, float b, float a) {
                auto& s = w.AddComponent<ECS::SpriteComponent>(e);
                s.Color = glm::vec4(r, g, b, a);
                s.Texture = nullptr;
                return &s;
            },
            "SetSpriteColor", [](ECS::World& w, entt::entity e, float r, float g, float b, float a) {
                if (!w.HasComponent<ECS::SpriteComponent>(e)) return;
                w.GetComponent<ECS::SpriteComponent>(e).Color = glm::vec4(r, g, b, a);
            },
            "GetSpriteColor", [&lua](ECS::World& w, entt::entity e) -> sol::table {
                sol::state_view sv = sol::state_view(lua.GetState());
                sol::table t = sv.create_table();
                if (!w.HasComponent<ECS::SpriteComponent>(e)) {
                    t["r"] = 0.0f; t["g"] = 0.0f; t["b"] = 0.0f; t["a"] = 0.0f;
                    return t;
                }
                auto& c = w.GetComponent<ECS::SpriteComponent>(e).Color;
                t["r"] = c.r; t["g"] = c.g; t["b"] = c.b; t["a"] = c.a;
                return t;
            },

            "HasComponent", sol::overload(
                [](ECS::World& w, entt::entity e) -> bool { return w.HasComponent<ECS::TransformComponent>(e); },
                [](ECS::World& w, entt::entity e, const std::string& type) -> bool {
                    if (type == "Transform") return w.HasComponent<ECS::TransformComponent>(e);
                    if (type == "Sprite")    return w.HasComponent<ECS::SpriteComponent>(e);
                    if (type == "Name")      return w.HasComponent<ECS::NameComponent>(e);
                    return false;
                }
            ),

            "RemoveComponent", sol::overload(
                [](ECS::World& w, entt::entity e) { w.RemoveComponent<ECS::TransformComponent>(e); },
                [](ECS::World& w, entt::entity e, const std::string& type) {
                    if (type == "Transform") w.RemoveComponent<ECS::TransformComponent>(e);
                    else if (type == "Sprite") w.RemoveComponent<ECS::SpriteComponent>(e);
                    else if (type == "Name")   w.RemoveComponent<ECS::NameComponent>(e);
                }
            ),

            "IsValid", [](ECS::World& w, entt::entity e) -> bool {
                return w.GetRegistry().valid(e);
            }
        );

        // Глобальная функция: получить World*. SandBox регистрирует свою: m_Lua.RegisterFunction("GetECSWorld", ...).
    }

} // namespace NK
