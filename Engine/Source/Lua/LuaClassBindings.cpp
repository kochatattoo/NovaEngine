#include "Lua/LuaClassBindings.h"
#include <sol/sol.hpp>
#include <glm/glm.hpp>
#include "Lua/LuaManager.h"
#include "Renderer/Font.h"
#include "Renderer/SpriteRenderer.h"
#include "Renderer/TextRenderer.h"
#include "Scene/GameObject.h"
#include "Scene/Scene.h"
#include "Scene/ScriptComponent.h"
#include "Scene/Transform.h"
#include "UI/Anchor.h"
#include "UI/Button.h"

namespace NK
{
    void LuaClassBindings::RegisterAll(LuaManager& lua)
    {
        // GameObject
        lua.BindClass<GameObject>("GameObject",
            "AddComponent_Transform", [](GameObject& obj) -> Transform* {
                auto* t = obj.GetComponent<Transform>();
                if (t) return t;
                return obj.AddComponent<Transform>();
            },
            "AddComponent_SpriteRenderer", [](GameObject& obj) { return obj.AddComponent<SpriteRenderer>(); },
            "AddComponent_TextRenderer", [](GameObject& obj) { return obj.AddComponent<TextRenderer>(); },
            "AddComponent_Button", [](GameObject& obj) { return obj.AddComponent<Button>(); },
            "AddComponent_Script", [](GameObject& obj, const std::string& path) { return obj.AddComponent<ScriptComponent>(path); },
            "AddComponent_Anchor", [](GameObject& obj) { return obj.AddComponent<Anchor>(); },
            "GetTransform", [](GameObject& obj) { return obj.GetComponent<Transform>(); },
            "GetSpriteRenderer", [](GameObject& obj) { return obj.GetComponent<SpriteRenderer>(); },
            "GetTextRenderer", [](GameObject& obj) { return obj.GetComponent<TextRenderer>(); },
            "GetButton", [](GameObject& obj) { return obj.GetComponent<Button>(); },
            "SetZOrder", &GameObject::SetZOrder,
            "GetZOrder", &GameObject::GetZOrder,
            "GetName", &GameObject::GetName,
            "OnStart", &GameObject::OnStart,
            "OnUpdate", &GameObject::OnUpdate
        );
        
        // Scene
        lua.BindClass<Scene>("Scene",
            "CreateGameObject", &Scene::CreateGameObject,
            "CreateUIObject", &Scene::CreateUIObject,
            "OnStart", &Scene::OnStart,
            "OnUpdate", &Scene::OnUpdate,
            "OnRender", &Scene::OnRender,
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
