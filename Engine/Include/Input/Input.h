#pragma once

#define NOMINMAX
#include <Windows.h>
#include <cstdint>

#include "KeyCode/KeyCodes.h"

namespace NK {
    
    class Input {
    public:
        // Возвращает true, если клавиша удерживается в данный момент
        static bool IsKeyDown(KeyCode keycode);

        // Кнопки мыши (используем константы Windows)
        static bool IsMouseButtonDown(int button);

        // Получить положение курсора в экранных координатах
        static void GetMousePosition(int32_t& outX, int32_t& outY);

        // Установить положение курсора
        static void SetMousePosition(int32_t x, int32_t y);
    };

} // namespace NK
