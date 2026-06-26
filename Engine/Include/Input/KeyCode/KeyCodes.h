#pragma once
#include <cstdint>
#define NOMINMAX
#include <Windows.h>

namespace NK {

    enum class KeyCode : uint32_t {
        None = 0,
        // Буквы
        A = 'A', B = 'B', C = 'C', D = 'D', E = 'E', F = 'F', G = 'G', H = 'H',
        I = 'I', J = 'J', K = 'K', L = 'L', M = 'M', N = 'N', O = 'O', P = 'P',
        Q = 'Q', R = 'R', S = 'S', T = 'T', U = 'U', V = 'V', W = 'W', X = 'X',
        Y = 'Y', Z = 'Z',
        // Цифры
        Num0 = '0', Num1 = '1', Num2 = '2', Num3 = '3', Num4 = '4',
        Num5 = '5', Num6 = '6', Num7 = '7', Num8 = '8', Num9 = '9',
        // Функциональные клавиши
        F1 = VK_F1, F2 = VK_F2, F3 = VK_F3, F4 = VK_F4,
        F5 = VK_F5, F6 = VK_F6, F7 = VK_F7, F8 = VK_F8,
        F9 = VK_F9, F10 = VK_F10, F11 = VK_F11, F12 = VK_F12,
        // Стрелки
        Left = VK_LEFT, Right = VK_RIGHT, Up = VK_UP, Down = VK_DOWN,
        // Специальные
        Space = VK_SPACE, Escape = VK_ESCAPE, Enter = VK_RETURN,
        Tab = VK_TAB, Backspace = VK_BACK,
        Insert = VK_INSERT, Delete = VK_DELETE,
        Home = VK_HOME, End = VK_END,
        PageUp = VK_PRIOR, PageDown = VK_NEXT,
        CapsLock = VK_CAPITAL, NumLock = VK_NUMLOCK, ScrollLock = VK_SCROLL,
        PrintScreen = VK_SNAPSHOT, Pause = VK_PAUSE,
        // Модификаторы
        LeftShift = VK_LSHIFT, LeftControl = VK_LCONTROL, LeftAlt = VK_LMENU,
        RightShift = VK_RSHIFT, RightControl = VK_RCONTROL, RightAlt = VK_RMENU,
        // Numpad
        Numpad0 = VK_NUMPAD0, Numpad1 = VK_NUMPAD1, Numpad2 = VK_NUMPAD2,
        Numpad3 = VK_NUMPAD3, Numpad4 = VK_NUMPAD4, Numpad5 = VK_NUMPAD5,
        Numpad6 = VK_NUMPAD6, Numpad7 = VK_NUMPAD7, Numpad8 = VK_NUMPAD8,
        Numpad9 = VK_NUMPAD9,
        NumpadAdd = VK_ADD, NumpadSubtract = VK_SUBTRACT,
        NumpadMultiply = VK_MULTIPLY, NumpadDivide = VK_DIVIDE,
        NumpadDecimal = VK_DECIMAL
    };

    enum class MouseButton : uint8_t {
        Left = VK_LBUTTON,
        Right = VK_RBUTTON,
        Middle = VK_MBUTTON
    };

}