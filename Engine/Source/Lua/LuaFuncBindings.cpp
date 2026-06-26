#include "Lua/LuaManager.h"
#include "lua/LuaFuncBindings.h"
#include "Scene/Scene.h"
#include "Core/Engine.h"
#include "Renderer/Font.h"
#include "Renderer/Texture2D.h"
#include "Renderer/Renderer.h"
#include "Input/Input.h"
#include "Window/Window.h"
#include "Core/Log.h"

namespace NK{

	void LuaFuncBindings::RegisterAll(LuaManager& lua) {
        // --- Глобальные функции ---
        lua.RegisterFunction("Log", [](const std::string& msg) {
            NK_INFO("%s", msg.c_str());
            });
        lua.RegisterFunction("IsKeyDown", [](int key) -> bool {
            return Input::IsKeyDown(key);
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
	}
}