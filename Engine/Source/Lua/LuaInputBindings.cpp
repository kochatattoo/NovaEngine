#include "Lua/LuaInputBindings.h"
#include "Lua/LuaManager.h"
#include "Input/Input.h"
#include "Input/InputSystem.h"
#include "Input/KeyCode/KeyCodes.h"
#include <sol/sol.hpp>

namespace NK {

    void LuaInputBindings::RegisterAll(LuaManager& lua) {
        // ����������� ������������
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
        {"LeftShift", KeyCode::LeftShift}, {"LeftControl", KeyCode::LeftControl}, {"LeftAlt", KeyCode::LeftAlt},
        {"RightShift", KeyCode::RightShift}, {"RightControl", KeyCode::RightControl}, {"RightAlt", KeyCode::RightAlt}
            });

        lua.RegisterEnum<MouseButton>("MouseButton", {
        {"Left", MouseButton::Left},
        {"Right", MouseButton::Right},
        {"Middle", MouseButton::Middle}
            });

        // ���������� ������� ����� (����������� �� ������ ����� � �����)
        lua.RegisterFunction("IsKeyDown", [](int key) -> bool {
            return Input::IsKeyDown(key);
            });
        lua.RegisterFunction("IsMouseButtonDown", [](int button) -> bool {
            return Input::IsMouseButtonDown(button);
            });
        lua.RegisterFunction("GetMousePosition", []() -> std::tuple<int, int> {
            int x, y;
            Input::GetMousePosition(x, y);
            return { x, y };
            });

        // ����� ������� (���������� InputSystem)
        lua.RegisterFunction("GetKey", [](KeyCode k) -> bool {
            return InputSystem::Get().GetKey(k);
            });
        lua.RegisterFunction("IsKeyDown", [](KeyCode k) -> bool {
            // Алиас для GetKey — более привычное имя (Unity-style)
            return InputSystem::Get().GetKey(k);
            });
        lua.RegisterFunction("GetKeyDown", [](KeyCode k) -> bool {
            return InputSystem::Get().GetKeyDown(k);
            });
        lua.RegisterFunction("GetKeyUp", [](KeyCode k) -> bool {
            return InputSystem::Get().GetKeyUp(k);
            });
        lua.RegisterFunction("GetMouseButton", [](MouseButton b) -> bool {
            return InputSystem::Get().GetMouseButton(b);
            });
        lua.RegisterFunction("GetMouseButtonDown", [](MouseButton b) -> bool {
            return InputSystem::Get().GetMouseButtonDown(b);
            });
        lua.RegisterFunction("GetMouseButtonUp", [](MouseButton b) -> bool {
            return InputSystem::Get().GetMouseButtonUp(b);
            });
        lua.RegisterFunction("GetMousePos", []() -> std::tuple<int, int> {
            auto pos = InputSystem::Get().GetMousePosition();
            return { static_cast<int>(pos.x), static_cast<int>(pos.y) };
            });
    }

} // namespace NK