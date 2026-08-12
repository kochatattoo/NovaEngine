#include "Input/InputSystem.h"
#include "Event/Event.h"

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
                KeyCode k = static_cast<KeyCode>(key.KeyCode);
                m_KeysHeld[k] = true;
                m_KeyJustPressed[k] = true;
            })
            .Dispatch<KeyReleasedEvent>([this](auto& keyUp) {
                KeyCode k = static_cast<KeyCode>(keyUp.KeyCode);
                m_KeysHeld[k] = false;
                m_KeyJustReleased[k] = true;
            })
            .Dispatch<MouseMovedEvent>([this](auto& mouseMove) {
                m_MousePosition = glm::vec2(mouseMove.MouseX, mouseMove.MouseY);
            })
            .Dispatch<MouseButtonPressedEvent>([this](auto& mouseBtn) {
                m_MouseButtonsHeld[mouseBtn.m_Button] = true;
                m_MouseJustPressed[mouseBtn.m_Button] = true;
            })
            .Dispatch<MouseButtonReleasedEvent>([this](auto& mouseBtnUp) {
                m_MouseButtonsHeld[mouseBtnUp.m_Button] = false;
                m_MouseJustReleased[mouseBtnUp.m_Button] = true;
            })
            .Dispatch<MouseScrolledEvent>([this](auto& scroll) {
                // Суммируем, так как за кадр может прилететь несколько событий
                m_MouseScroll += glm::vec2(scroll.XOffset, scroll.YOffset);
            });
    }

    bool InputSystem::GetKey(KeyCode key) const {
        auto it = m_KeysHeld.find(key);
        return it != m_KeysHeld.end() && it->second;
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
        auto it = m_MouseButtonsHeld.find(button);
        return it != m_MouseButtonsHeld.end() && it->second;
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
    glm::vec2 InputSystem::GetMouseScroll() const {
        return m_MouseScroll;
    }

    void InputSystem::ResetOneShotStates() {
        m_KeyJustPressed.clear();
        m_KeyJustReleased.clear();
        m_MouseJustPressed.clear();
        m_MouseJustReleased.clear();
        m_MouseScroll = glm::vec2(0.0f);
    }

} // namespace NK