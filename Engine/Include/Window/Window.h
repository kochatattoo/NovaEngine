#pragma once

// Включаем Windows API, чтобы были доступны HWND, UINT, LRESULT и т.д.
// NOMINMAX убирает макросы min/max, которые могут конфликтовать с std::min/max
#define NOMINMAX
#include <Windows.h>

#include <string>
#include <functional>
#include "Core/Log.h"

namespace NK {

    // Пока заготовка для событий
    struct WindowProperties {
        std::string Title;
        uint32_t Width;
        uint32_t Height;
    };

    class Window {
    public:
        explicit Window(const WindowProperties& props);
        ~Window();

        // Обработать очередь сообщений Windows
        void OnUpdate();
        // Должно ли окно закрыться?
        bool ShouldClose() const { return m_ShouldClose; }
        // Получить нативное окно (HWND) для OpenGL/DirectX
        void* GetNativeWindow() const { return m_Hwnd; }
        uint32_t GetWidth() const { return m_Data.Width; }
        uint32_t GetHeight() const { return m_Data.Height; }

    private:
        void Init();    // Регистрация класса окна и создание
        void Shutdown();// Уничтожение окна

        // Статическая функция обратного вызова (как WndProc в WinForms)
        static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        struct WindowData {
            std::string Title;
            uint32_t Width, Height;
            bool ShouldClose = false;
        };

        HWND m_Hwnd = nullptr;
        WindowData m_Data;
        bool m_ShouldClose = false;
    };

} // namespace NK
