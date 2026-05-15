#pragma once
#include <cstdint>

namespace NK {

    class Renderer {
    public:
        // Очистить задний буфер текущим цветом
        static void Clear();

        // Установить цвет очистки (RGB + альфа, но альфа пока не используется)
        static void SetClearColor(float r, float g, float b, float a);

        // Инициализация (получает нативное окно для получения контекста)
        static void Init(void* nativeWindow);

    private:
        static float s_ClearColor[4];
        static void* s_WindowHandle;  // HWND
    };

} // namespace NK