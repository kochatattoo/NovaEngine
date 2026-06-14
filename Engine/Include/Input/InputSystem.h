#pragma once
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <memory>
#include <Event/Event.h>
#include "KeyCode/KeyCodes.h"

namespace NK {

    class Event;

    class InputSystem {
    public:
        static InputSystem& Get();

        void Update();  // вызывается каждый кадр
        void PushEvent(std::unique_ptr<Event> event);

        // Клавиатура
        bool GetKey(KeyCode key) const;
        bool GetKeyDown(KeyCode key) const;
        bool GetKeyUp(KeyCode key) const;

        // Мышь
        bool GetMouseButton(MouseButton button) const;
        bool GetMouseButtonDown(MouseButton button) const;
        bool GetMouseButtonUp(MouseButton button) const;
        glm::vec2 GetMousePosition() const;

    private:
        InputSystem() = default;
        void ProcessEvent(Event& e);
        void ResetOneShotStates();

        std::unordered_map<KeyCode, bool> m_KeyJustPressed;
        std::unordered_map<KeyCode, bool> m_KeyJustReleased;
        std::unordered_map<MouseButton, bool> m_MouseJustPressed;
        std::unordered_map<MouseButton, bool> m_MouseJustReleased;
        glm::vec2 m_MousePosition = glm::vec2(0.0f);
        std::vector<std::unique_ptr<Event>> m_EventQueue;
    };

} // namespace NK