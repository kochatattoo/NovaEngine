#include "Lua/LuaBindings.h"
#include "Lua/LuaManager.h"
#include "Scene/Transform.h"
#include "Scene/GameObject.h"
#include "Scene/Scene.h"
#include "Core/Engine.h"
#include "Scene/ScriptComponent.h"
#include "Renderer/SpriteRenderer.h"
#include "Renderer/TextRenderer.h"
#include "UI/Button.h"
#include "UI/Anchor.h"
#include "Renderer/OrthographicCamera.h"
#include "Renderer/Font.h"
#include "Renderer/Texture2D.h"
#include "Renderer/Renderer.h"
#include "Input/Input.h"
#include "Window/Window.h"
#include "Core/Log.h"
#include <sol/sol.hpp>
#include <glm/glm.hpp>

#include "Input/InputManager.h"

namespace NK {

    void LuaBindings::RegisterAll(LuaManager& lua) {
        // Transform
        lua.BindClass<Transform>("Transform",
            "SetPosition", [](Transform& t, double x, double y, double z) {
                t.SetPosition(glm::vec3((float)x, (float)y, (float)z));
            },
            "GetPosition", [](Transform& t) -> std::tuple<float, float, float> {
                auto& p = t.GetPosition();
                return { p.x, p.y, p.z };
            },
            "SetRotation", &Transform::SetRotation,
            "GetRotationDegrees", &Transform::GetRotationDegrees,
            "SetScale", [](Transform& t, float x, float y) { t.SetScale(glm::vec2(x, y)); },
            "GetScale", [](Transform& t) -> std::tuple<float, float> {
                auto& s = t.GetScale();
                return { s.x, s.y };
            },
            sol::base_classes, sol::bases<Component>()
        );

        // SpriteRenderer
        lua.BindClass<SpriteRenderer>("SpriteRenderer",
            "SetTexture", &SpriteRenderer::SetTexture,
            "SetShader", &SpriteRenderer::SetShader,
            "SetAlignment", [](SpriteRenderer& sr, double h, double v) { sr.SetAlignment((float)h, (float)v); },
            "SetIsUI", &SpriteRenderer::SetIsUI,
            "SetColor", [](SpriteRenderer& sr, double r, double g, double b, double a) {
                sr.SetColor((float)r, (float)g, (float)b, (float)a);
            },
            "SetUseColor", &SpriteRenderer::SetUseColor,
            sol::base_classes, sol::bases<Component>()
        );

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

        // OrthographicCamera
        lua.BindClass<OrthographicCamera>("OrthographicCamera",
            "GetLeft", &OrthographicCamera::GetLeft,
            "GetRight", &OrthographicCamera::GetRight,
            "GetBottom", &OrthographicCamera::GetBottom,
            "GetTop", &OrthographicCamera::GetTop,
            "ScreenToWorldPoint", &OrthographicCamera::ScreenToWorldPoint
        );

        // Font
        lua.BindClass<Font>("Font",
            "CreateTextTexture", &Font::CreateTextTexture
        );

        // TextRenderer
        lua.BindClass<TextRenderer>("TextRenderer",
            "SetFont", &TextRenderer::SetFont,
            "SetText", &TextRenderer::SetText,
            "SetFontSize", [](TextRenderer& tr, double size) { tr.SetFontSize((float)size); },
            "SetAlignment", [](TextRenderer& tr, double h, double v) { tr.SetAlignment((float)h, (float)v); },
            "SetColor", [](TextRenderer& tr, int r, int g, int b, int a) {
                tr.SetColor((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a);
            },
            sol::base_classes, sol::bases<Component>()
        );

        // Button
        lua.BindClass<Button>("Button",
            "SetCallback", &Button::SetCallback,
            "SetOnClick", &Button::SetOnClick,
            "SetOnPointerDown", &Button::SetOnPointerDown,
            "SetOnPointerUp", &Button::SetOnPointerDown,
            "SetOnPointerExit", &Button::SetOnPointerExit,
            "SetOnPointerUp", &Button::SetOnPointerUp,
            "SetSize", [](Button& btn, double x, double y) {
                btn.SetSize((float)x, (float)y);
            },
            sol::base_classes, sol::bases<Component>()
        );

        // Anchor
        lua.BindClass<Anchor>("Anchor",
            "SetPreset", [](Anchor& a, int preset) { a.SetPreset(static_cast<AnchorPreset>(preset)); },
            "SetScreenAnchor", [](Anchor& a, double sx, double sy) { a.SetScreenAnchor((float)sx, (float)sy); },
            "SetObjectAnchor", [](Anchor& a, double ox, double oy) { a.SetObjectAnchor((float)ox, (float)oy); },
            "SetSize", [](Anchor& a, double w, double h) { a.SetSize(glm::vec2((float)w, (float)h)); },
            sol::base_classes, sol::bases<Component>()
        );

        // glm::vec2
        lua.BindClass<glm::vec2>("vec2",
            sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
            "x", &glm::vec2::x,
            "y", &glm::vec2::y
        );

        // --- ���������� ������� ---
        lua.RegisterFunction("Log", [](const std::string& msg) {
            NK_INFO("%s", msg.c_str());
            });
        lua.RegisterFunction("IsKeyDown", [](int key) -> bool {
            return Input::IsKeyDown(static_cast<KeyCode>(key));
            });
        lua.RegisterFunction("SetClearColor", [](float r, float g, float b, float a) {
            Renderer::SetClearColor(r, g, b, a);
            });
        lua.RegisterFunction("GetEngine", []() -> Engine& {
            return Engine::Get();
            });
        lua.RegisterFunction("GetScene", []() -> Scene& {
            return Engine::Get().GetScene();
            });
        lua.RegisterFunction("GetTexture", [](const std::string& path) {
            return Engine::Get().GetResourceManager().GetTexture(path);
            });
        lua.RegisterFunction("GetShader", [](const std::string& name, const std::string& vSrc, const std::string& fSrc) {
            return Engine::Get().GetResourceManager().GetShader(name, vSrc, fSrc);
            });
        lua.RegisterFunction("LoadFont", [](const std::string& path) -> std::shared_ptr<Font> {
            try {
                auto font = std::make_shared<Font>(path);
                return font;
            }
            catch (...) {
                return nullptr;
            }
            });
        lua.RegisterFunction("GetMousePosition", []() -> std::tuple<int, int> {
            int x, y;
            Engine::Get().GetWindow()->GetMouseClientPosition(x, y);
            return { x, y };
            });
        lua.RegisterFunction("GetWindowWidth", []() { return Engine::Get().GetWindow()->GetWidth(); });
        lua.RegisterFunction("GetWindowHeight", []() { return Engine::Get().GetWindow()->GetHeight(); });
        lua.RegisterFunction("IsMouseButtonDown", [](int button) -> bool {
            return Input::IsMouseButtonDown(button);
            });
        lua.RegisterFunction("CreateSolidColorTexture", [](int r, int g, int b, int a) -> std::shared_ptr<Texture2D> {
            return Texture2D::CreateSolidColor((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a);
            });
        
            // ===== Регистрация перечислений =====
        lua.RegisterEnum<KeyCode>("KeyCode", {
             {"A", KeyCode::A}, {"B", KeyCode::B}, {"C", KeyCode::C}, {"D", KeyCode::D},
            {"E", KeyCode::E}, {"F", KeyCode::F}, {"G", KeyCode::G}, {"H", KeyCode::H},
            {"I", KeyCode::I}, {"J", KeyCode::J}, {"K", KeyCode::K}, {"L", KeyCode::L},
            {"M", KeyCode::M}, {"N", KeyCode::N}, {"O", KeyCode::O}, {"P", KeyCode::P},
            {"Q", KeyCode::Q}, {"R", KeyCode::R}, {"S", KeyCode::S}, {"T", KeyCode::T},
            {"U", KeyCode::U}, {"V", KeyCode::V}, {"W", KeyCode::W}, {"X", KeyCode::X},
            {"Y", KeyCode::Y}, {"Z", KeyCode::Z},
            {"Num0", KeyCode::Num0}, {"Num1", KeyCode::Num1}, {"Num2", KeyCode::Num2},
            {"Num3", KeyCode::Num3}, {"Num4", KeyCode::Num4}, {"Num5", KeyCode::Num5},
            {"Num6", KeyCode::Num6}, {"Num7", KeyCode::Num7}, {"Num8", KeyCode::Num8},
            {"Num9", KeyCode::Num9},
            {"F1", KeyCode::F1}, {"F2", KeyCode::F2}, {"F3", KeyCode::F3}, {"F4", KeyCode::F4},
            {"F5", KeyCode::F5}, {"F6", KeyCode::F6}, {"F7", KeyCode::F7}, {"F8", KeyCode::F8},
            {"F9", KeyCode::F9}, {"F10", KeyCode::F10}, {"F11", KeyCode::F11}, {"F12", KeyCode::F12},
            {"Left", KeyCode::Left}, {"Right", KeyCode::Right}, {"Up", KeyCode::Up}, {"Down", KeyCode::Down},
            {"Space", KeyCode::Space}, {"Escape", KeyCode::Escape}, {"Enter", KeyCode::Enter},
            {"Tab", KeyCode::Tab}, {"Backspace", KeyCode::Backspace},
            {"Insert", KeyCode::Insert}, {"Delete", KeyCode::Delete},
            {"Home", KeyCode::Home}, {"End", KeyCode::End},
            {"PageUp", KeyCode::PageUp}, {"PageDown", KeyCode::PageDown},
            {"CapsLock", KeyCode::CapsLock}, {"NumLock", KeyCode::NumLock},
            {"ScrollLock", KeyCode::ScrollLock}, {"PrintScreen", KeyCode::PrintScreen},
            {"Pause", KeyCode::Pause},
            {"LeftShift", KeyCode::LeftShift}, {"LeftControl", KeyCode::LeftControl},
            {"LeftAlt", KeyCode::LeftAlt},
            {"RightShift", KeyCode::RightShift}, {"RightControl", KeyCode::RightControl},
            {"RightAlt", KeyCode::RightAlt},
            {"Numpad0", KeyCode::Numpad0}, {"Numpad1", KeyCode::Numpad1},
            {"Numpad2", KeyCode::Numpad2}, {"Numpad3", KeyCode::Numpad3},
            {"Numpad4", KeyCode::Numpad4}, {"Numpad5", KeyCode::Numpad5},
            {"Numpad6", KeyCode::Numpad6}, {"Numpad7", KeyCode::Numpad7},
            {"Numpad8", KeyCode::Numpad8}, {"Numpad9", KeyCode::Numpad9},
            {"NumpadAdd", KeyCode::NumpadAdd}, {"NumpadSubtract", KeyCode::NumpadSubtract},
            {"NumpadMultiply", KeyCode::NumpadMultiply}, {"NumpadDivide", KeyCode::NumpadDivide},
            {"NumpadDecimal", KeyCode::NumpadDecimal}
        });

        lua.RegisterEnum<MouseButton>("MouseButton", {
            {"Left", MouseButton::Left},
            {"Right", MouseButton::Right},
            {"Middle", MouseButton::Middle}
        });

        // ===== Регистрация функций ввода =====
        lua.RegisterFunction("GetKeyDown", [](KeyCode k) -> bool {
            return InputManager::Get().GetKeyDown(k);
        });
        lua.RegisterFunction("GetKeyUp", [](KeyCode k) -> bool {
            return InputManager::Get().GetKeyUp(k);
        });
        lua.RegisterFunction("GetKey", [](KeyCode k) -> bool {
            return InputManager::Get().GetKey(k);
        });
        lua.RegisterFunction("GetMouseButtonDown", [](MouseButton b) -> bool {
            return InputManager::Get().GetMouseButtonDown(b);
        });
        lua.RegisterFunction("GetMouseButtonUp", [](MouseButton b) -> bool {
            return InputManager::Get().GetMouseButtonUp(b);
        });
        lua.RegisterFunction("GetMouseButton", [](MouseButton b) -> bool {
            return InputManager::Get().GetMouseButton(b);
        });
        lua.RegisterFunction("GetMousePosition", []() -> std::tuple<int, int> {
            auto pos = InputManager::Get().GetMousePosition();
            return {static_cast<int>(pos.x), static_cast<int>(pos.y)};
        });
    }

} // namespace NK
