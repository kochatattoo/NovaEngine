#include "Input/InputSystem.h"
#include "Event/Event.h"
#include "Input/Input.h"

namespace NK {

    InputSystem& InputSystem::Get() {
        static InputSystem instance;
        return instance;
    }

    void InputSystem::Update() {
        for (auto& e : m_EventQueue) {
            ProcessEvent(*e);
        }
        m_EventQueue.clear();
        ResetOneShotStates();
    }

    void InputSystem::PushEvent(std::unique_ptr<Event> event) {
        m_EventQueue.push_back(std::move(event));
    }

    void InputSystem::ProcessEvent(Event& e) {
        EventDispatcher(e)
            .Dispatch<KeyPressedEvent>([this](auto& key) {
            m_KeyJustPressed[static_cast<KeyCode>(key.KeyCode)] = true;
                })
            .Dispatch<KeyReleasedEvent>([this](auto& keyUp) {
            m_KeyJustReleased[static_cast<KeyCode>(keyUp.KeyCode)] = true;
                })
            .Dispatch<MouseMovedEvent>([this](auto& mouseMove) {
            m_MousePosition = glm::vec2(mouseMove.MouseX, mouseMove.MouseY);
                })
            .Dispatch<MouseButtonPressedEvent>([this](auto& mouseBtn) {
            m_MouseJustPressed[mouseBtn.Button] = true;
                })
            .Dispatch<MouseButtonReleasedEvent>([this](auto& mouseBtnUp) {
            m_MouseJustReleased[mouseBtnUp.Button] = true;
                });
    }

    bool InputSystem::GetKey(KeyCode key) const {
        return Input::GetKey(key);
    }
    bool InputSystem::GetKeyDown(KeyCode key) const {
        auto it = m_KeyJustPressed.find(key);
        return it != m_KeyJustPressed.end() && it->second;
    }
    bool InputSystem::GetKeyUp(KeyCode key) const {
        auto it = m_KeyJustReleased.find(key);
        return it != m_KeyJustReleased.end() && it->second;
    }
    bool InputSystem::GetMouseButton(MouseButton button) const {
        return Input::GetMouseButton(button);
    }
    bool InputSystem::GetMouseButtonDown(MouseButton button) const {
        auto it = m_MouseJustPressed.find(button);
        return it != m_MouseJustPressed.end() && it->second;
    }
    bool InputSystem::GetMouseButtonUp(MouseButton button) const {
        auto it = m_MouseJustReleased.find(button);
        return it != m_MouseJustReleased.end() && it->second;
    }
    glm::vec2 InputSystem::GetMousePosition() const {
        return m_MousePosition;
    }

    void InputSystem::ResetOneShotStates() {
        m_KeyJustPressed.clear();
        m_KeyJustReleased.clear();
        m_MouseJustPressed.clear();
        m_MouseJustReleased.clear();
    }

} // namespace NK