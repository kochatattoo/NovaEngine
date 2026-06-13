#include "Input/InputManager.h"
#include "Input/KeyCode/KeyCodes.h"
#include "Event/Event.h"   // или "Events/Event.h" после переименования
#include "Input/Input.h"   // временно, для опроса клавиш (позже уйдёт)

namespace NK {

    InputManager& InputManager::Get() {
        static InputManager instance;
        return instance;
    }

    void InputManager::Update() {
        // 1. Обработка всех событий из очереди
        for (auto& e : m_EventQueue) {
            ProcessEvent(*e);
        }
        m_EventQueue.clear();

        // 2. Опрос «удерживаемых» клавиш через старый Input (временный вызов)
        //    В будущем можно хранить состояния между кадрами.
        //    Пока вызываем Input::IsKeyDown для всех зарегистрированных клавиш.
        //    Мы просто обновим m_KeyHeld (не показано) через вызов Input::IsKeyDown,
        //    но у нас нет хранилища удерживаемых клавиш. Пока оставим как есть:
        //    методы GetKey будут напрямую дергать Input::IsKeyDown.
        //    (Это временное решение, позже мы можем кэшировать состояние внутри InputManager)

         // Сбрасываем «одноразовые» флаги (они будут установлены заново в следующем кадре)
        ResetOneShotStates();
    }

    void InputManager::PushEvent(std::unique_ptr<Event> event) {
        m_EventQueue.push_back(std::move(event));
    }

    void InputManager::ProcessEvent(Event& e) {
        // Здесь разбираем конкретные события и выставляем флаги
        if (auto* key = dynamic_cast<KeyPressedEvent*>(&e)) {
            m_KeyJustPressed[static_cast<KeyCode>(key->KeyCode)] = true;
        }
        else if (auto* keyUp = dynamic_cast<KeyReleasedEvent*>(&e)) {
            m_KeyJustReleased[static_cast<KeyCode>(keyUp->KeyCode)] = true;
        }
        else if (auto* mouseMove = dynamic_cast<MouseMovedEvent*>(&e)) {
            m_MousePosition = glm::vec2(mouseMove->MouseX, mouseMove->MouseY);
        }
        else if (auto* mouseBtn = dynamic_cast<MouseButtonPressedEvent*>(&e)) {
            m_MouseJustPressed[mouseBtn->Button] = true;
        }
        else if (auto* mouseBtnUp = dynamic_cast<MouseButtonReleasedEvent*>(&e)) {
            m_MouseJustReleased[mouseBtnUp->Button] = true;
        }
    }

    bool InputManager::GetKeyDown(KeyCode key) const {
        auto it = m_KeyJustPressed.find(key);
        return it != m_KeyJustPressed.end() && it->second;
    }

    bool InputManager::GetKeyUp(KeyCode key) const {
        auto it = m_KeyJustReleased.find(key);
        return it != m_KeyJustReleased.end() && it->second;
    }

    bool InputManager::GetKey(KeyCode key) const {
        // Пока используем старый Input
        return Input::IsKeyDown(static_cast<KeyCode>(static_cast<uint32_t>(key)));
    }

    bool InputManager::GetMouseButtonDown(MouseButton button) const {
        auto it = m_MouseJustPressed.find(button);
        return it != m_MouseJustPressed.end() && it->second;
    }

    bool InputManager::GetMouseButtonUp(MouseButton button) const {
        auto it = m_MouseJustReleased.find(button);
        return it != m_MouseJustReleased.end() && it->second;
    }

    bool InputManager::GetMouseButton(MouseButton button) const {
        return Input::IsMouseButtonDown(static_cast<int>(button));
    }

    void InputManager::ResetOneShotStates() {
        m_KeyJustPressed.clear();
        m_KeyJustReleased.clear();
        m_MouseJustPressed.clear();
        m_MouseJustReleased.clear();
    }

} // namespace NK