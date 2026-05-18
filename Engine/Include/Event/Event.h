#pragma once
#include <string>
#include <functional>

namespace NK {

    // Типы событий – перечисление, чтобы можно было сравнивать
    enum class EventType {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    // Базовый класс события (как абстрактный класс в C#, но без интерфейсов)
    class Event {
    public:
        virtual ~Event() = default;

        // Чисто виртуальные методы, чтобы каждый наследник реализовал
        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual std::string ToString() const { return GetName(); }

        // Флаг, который можно выставить в true, если событие обработано
        bool Handled = false;
    };

    // Шаблонный диспетчер событий. Аналог паттерна Visitor.
    // Позволяет писать: dispatcher.Dispatch<KeyPressedEvent>([](auto& e){ ... });
    class EventDispatcher {
    public:
        EventDispatcher(Event& event) : m_Event(event) {}

        // Метод Dispatch принимает тип события T и функцию-обработчик.
        // Если тип события совпадает, вызывает функцию и возвращает true.
        template<typename T, typename F>
        bool Dispatch(const F& func) {
            if (m_Event.GetEventType() == T::GetStaticType()) {
                m_Event.Handled |= func(static_cast<T&>(m_Event));
                return true;
            }
            return false;
        }
    private:
        Event& m_Event;
    };

    // Макрос для удобства: добавляет статический метод GetStaticType,
    // виртуальные GetEventType и GetName.
#define EVENT_CLASS_TYPE(type) \
    static EventType GetStaticType() { return EventType::type; } \
    virtual EventType GetEventType() const override { return GetStaticType(); } \
    virtual const char* GetName() const override { return #type; }

// ---------- Конкретные события ----------

// Нажатие клавиши (срабатывает при WM_KEYDOWN)
    class KeyPressedEvent : public Event {
    public:
        KeyPressedEvent(int keycode, bool repeat = false)
            : KeyCode(keycode), Repeat(repeat) {
        }

        int KeyCode;     // виртуальный код клавиши (VK_*)
        bool Repeat;     // true, если удерживается (автоповтор)

        EVENT_CLASS_TYPE(KeyPressed)
    };

    // Отпускание клавиши (WM_KEYUP)
    class KeyReleasedEvent : public Event {
    public:
        KeyReleasedEvent(int keycode)
            : KeyCode(keycode) {
        }

        int KeyCode;
        EVENT_CLASS_TYPE(KeyReleased)
    };

    // Движение мыши (WM_MOUSEMOVE)
    class MouseMovedEvent : public Event {
    public:
        MouseMovedEvent(float x, float y)
            : MouseX(x), MouseY(y) {
        }

        float MouseX, MouseY;
        EVENT_CLASS_TYPE(MouseMoved)
    };

    // Прокрутка колёсика (WM_MOUSEWHEEL)
    class MouseScrolledEvent : public Event {
    public:
        MouseScrolledEvent(float xOffset, float yOffset)
            : XOffset(xOffset), YOffset(yOffset) {
        }

        float XOffset, YOffset;   // обычно XOffset = 0, YOffset = ±1
        EVENT_CLASS_TYPE(MouseScrolled)
    };

    // Пока достаточно. Позже можно добавить MouseButtonPressed, WindowResize и т.д.

} // namespace NK
