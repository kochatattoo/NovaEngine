#pragma once

#define NOMINMAX
#include <Windows.h>
#include <cstdint>
#include "KeyCode/KeyCodes.h"

namespace NK {

    // Виртуальные коды клавиш Windows (можно использовать напрямую, но для удобства заведём using)
    using KeyCode = uint32_t;

    class Input {
    public:
        static bool IsKeyDown(KeyCode keycode);
        static bool IsMouseButtonDown(int button);
        static void GetMousePosition(int32_t& outX, int32_t& outY);
        static void SetMousePosition(int32_t x, int32_t y);

        static bool GetKey(KeyCode key);
        static bool GetKeyDown(KeyCode key);
        static bool GetKeyUp(KeyCode key);
        static bool GetMouseButton(MouseButton button);
        static bool GetMouseButtonDown(MouseButton button);
        static bool GetMouseButtonUp(MouseButton button);
    };

} // namespace NK
