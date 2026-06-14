#include "Input/Input.h"
#include <Windows.h>

namespace NK {

    bool Input::IsKeyDown(KeyCode keycode) {
        // Старший бит результата GetAsyncKeyState говорит, нажата ли клавиша сейчас
        return (GetAsyncKeyState(keycode) & 0x8000) != 0;
    }

    bool Input::IsMouseButtonDown(int button) {
        // button: VK_LBUTTON, VK_RBUTTON, VK_MBUTTON
        return (GetAsyncKeyState(button) & 0x8000) != 0;
    }

    void Input::GetMousePosition(int32_t& outX, int32_t& outY) {
        POINT pos;
        GetCursorPos(&pos);
        outX = pos.x;
        outY = pos.y;
    }

    void Input::SetMousePosition(int32_t x, int32_t y) {
        SetCursorPos(x, y);
    }

    bool Input::GetKey(KeyCode key) {
        return IsKeyDown(static_cast<uint32_t>(key));
    }

    bool Input::GetKeyDown(KeyCode key) {
        // Заглушка: в будущем можно хранить предыдущее состояние
        return IsKeyDown(static_cast<uint32_t>(key));
    }

    bool Input::GetKeyUp(KeyCode key) {
        return !IsKeyDown(static_cast<uint32_t>(key));
    }

    bool Input::GetMouseButton(MouseButton button) {
        return IsMouseButtonDown(static_cast<int>(button));
    }

    bool Input::GetMouseButtonDown(MouseButton button) {
        return IsMouseButtonDown(static_cast<int>(button));
    }

    bool Input::GetMouseButtonUp(MouseButton button) {
        return !IsMouseButtonDown(static_cast<int>(button));
    }

} // namespace NK