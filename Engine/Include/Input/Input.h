#pragma once

#define NOMINMAX
#include <Windows.h>
#include <cstdint>
#include "KeyCode/KeyCodes.h"

namespace NK {

    class Input {
    public:
        static bool IsKeyDown(KeyCode keycode);
        static bool IsKeyDown(uint32_t rawKey);
        static bool IsKeyDown(int rawKey);

        static bool IsMouseButtonDown(int button);
        static void GetMousePosition(int32_t& outX, int32_t& outY);
        static void SetMousePosition(int32_t x, int32_t y);

        static bool GetKey(KeyCode key);
        static bool GetKey(uint32_t rawKey);
        static bool GetKey(int rawKey);

        static bool GetKeyDown(KeyCode key);
        static bool GetKeyDown(uint32_t rawKey);
        static bool GetKeyDown(int rawKey);

        static bool GetKeyUp(KeyCode key);
        static bool GetKeyUp(uint32_t rawKey);
        static bool GetKeyUp(int rawKey);

        static bool GetMouseButton(MouseButton button);
        static bool GetMouseButton(uint8_t rawKey);
        static bool GetMouseButton(int rawKey);

        static bool GetMouseButtonDown(MouseButton button);
        static bool GetMouseButtonDown(uint8_t rawKey);
        static bool GetMouseButtonDown(int rawKey);

        static bool GetMouseButtonUp(MouseButton button);
        static bool GetMouseButtonUp(uint8_t rawKey);
        static bool GetMouseButtonUp(int rawKey);
    };

} // namespace NK
