#include "Renderer/Renderer.h"
#include <Windows.h>
#include <cstdio>   // для printf в debug

namespace NK {

    float Renderer::s_ClearColor[4] = { 0.1f, 0.1f, 0.15f, 1.0f };
    void* Renderer::s_WindowHandle = nullptr;

    void Renderer::Init(void* nativeWindow) {
        s_WindowHandle = nativeWindow;
    }

    void Renderer::SetClearColor(float r, float g, float b, float a) {
        s_ClearColor[0] = r;
        s_ClearColor[1] = g;
        s_ClearColor[2] = b;
        s_ClearColor[3] = a;
    }

    void Renderer::Clear() {
        if (!s_WindowHandle) return;

        HWND hwnd = static_cast<HWND>(s_WindowHandle);
        HDC hdc = GetDC(hwnd);

        // Преобразуем float [0..1] в BYTE [0..255]
        BYTE r = static_cast<BYTE>(s_ClearColor[0] * 255.0f);
        BYTE g = static_cast<BYTE>(s_ClearColor[1] * 255.0f);
        BYTE b = static_cast<BYTE>(s_ClearColor[2] * 255.0f);

        // Получаем размеры клиентской области окна
        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        // Создаём кисть нужного цвета и заливаем прямоугольник
        HBRUSH brush = CreateSolidBrush(RGB(r, g, b));
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);

        ReleaseDC(hwnd, hdc);
    }

} // namespace NK