#pragma once
#include "KeyCode/KeyCodes.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <memory>

namespace NK {

    class Event;

    class InputManager {
    public:
        static InputManager& Get();

        // Обновляет внутреннее состояние (вызывается каждый кадр)
        void Update();

        // Принять событие из Window
        void PushEvent(std::unique_ptr<Event> event);

        // Клавиатура: GetKeyDown – было нажато в этом кадре
        bool GetKeyDown(KeyCode key) const;
        // Клавиатура: GetKeyUp – было отпущено в этом кадре
        bool GetKeyUp(KeyCode key) const;
        // Клавиатура: GetKey – удерживается прямо сейчас
        bool GetKey(KeyCode key) const;

        // Мышь: кнопки
        bool GetMouseButtonDown(MouseButton button) const;
        bool GetMouseButtonUp(MouseButton button) const;
        bool GetMouseButton(MouseButton button) const;

        // Позиция мыши
        void SetWindowHandle(HWND hwnd) { m_WindowHandle = hwnd; }
        glm::vec2 GetMousePosition() const;

    private:
        InputManager() = default;
        void ProcessEvent(Event& e);
        void ResetOneShotStates();

        // Состояния клавиш
        std::unordered_map<KeyCode, bool> m_KeyJustPressed;
        std::unordered_map<KeyCode, bool> m_KeyJustReleased;
        // Мышь
        std::unordered_map<MouseButton, bool> m_MouseJustPressed;
        std::unordered_map<MouseButton, bool> m_MouseJustReleased;

        HWND m_WindowHandle = nullptr;
        glm::vec2 m_MousePosition = glm::vec2(0.0f);
        std::vector<std::unique_ptr<Event>> m_EventQueue;
    };

} // namespace NK