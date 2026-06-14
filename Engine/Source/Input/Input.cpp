#include "Input/Input.h"
#include <Windows.h>

namespace NK {

    bool Input::IsKeyDown(KeyCode keycode) {
        // Старший бит результата GetAsyncKeyState говорит, нажата ли клавиша сейчас
        return (GetAsyncKeyState(static_cast<int>(keycode)) & 0x8000) != 0;
    }

    bool Input::IsMouseButtonDown(int button) {
        // button: VK_LBUTTON, VK_RBUTTON, VK_MBUTTON
        return (GetAsyncKeyState(static_cast<int>(button)) & 0x8000) != 0;
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

    // =============== Method Overloading ================

    bool Input::IsKeyDown(uint32_t keycode) {
        // Перегрузка для сырых чисел (кастим к строгому enum)
        return IsKeyDown(static_cast<KeyCode>(keycode));
    }

    bool Input::IsKeyDown(int keycode) {
        // Дополнительная перегрузка для знаковых int
        return IsKeyDown(static_cast<KeyCode>(keycode));
    }

    bool Input::GetKey(uint32_t rawKey)
    {
        return GetKey(static_cast<KeyCode>(rawKey));
    }

    bool Input::GetKey(int rawKey)
    {
        return GetKey(static_cast<KeyCode>(rawKey));
    }

    bool Input::GetKeyDown(uint32_t rawKey)
    {
        return GetKeyDown(static_cast<KeyCode>(rawKey));
    }

    bool Input::GetKeyDown(int rawKey)
    {
        return GetKeyDown(static_cast<KeyCode>(rawKey));
    }

    bool Input::GetKeyUp(uint32_t rawKey)
    {
        return GetKeyUp(static_cast<KeyCode>(rawKey));
    }

    bool Input::GetKeyUp(int rawKey)
    {
        return GetKeyUp(static_cast<KeyCode>(rawKey));
    }

    bool Input::GetMouseButton(uint8_t rawKey)
    {
        return GetMouseButton(static_cast<MouseButton>(rawKey));
    }

    bool Input::GetMouseButton(int rawKey)
    {
        return GetMouseButton(static_cast<MouseButton>(rawKey));
    }

    bool Input::GetMouseButtonDown(uint8_t rawKey)
    {
        return GetMouseButtonDown(static_cast<MouseButton>(rawKey));
    }

    bool Input::GetMouseButtonDown(int rawKey)
    {
        return GetMouseButtonDown(static_cast<MouseButton>(rawKey));
    }

    bool Input::GetMouseButtonUp(uint8_t rawKey)
    {
        return GetMouseButtonUp(static_cast<MouseButton>(rawKey));
    }

    bool Input::GetMouseButtonUp(int rawKey)
    {
        return GetMouseButtonUp(static_cast<MouseButton>(rawKey));
    }
} // namespace NK