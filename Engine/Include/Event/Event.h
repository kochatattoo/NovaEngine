#pragma once
#include <string>
#include <functional>
#include <Input/KeyCode/KeyCodes.h>

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
    class EventDispatcher {
    public:
        EventDispatcher(Event& event) : m_Event(event) {}

        // 1. Перегрузка ТОЛЬКО для функций, возвращающих строго bool
        template<typename T, typename F>
        typename std::enable_if_t<std::is_same_v<bool, std::invoke_result_t<F, T&>>, EventDispatcher&>
            Dispatch(const F& func) {
            if (!m_Event.Handled && m_Event.GetEventType() == T::GetStaticType()) {
                m_Event.Handled |= func(static_cast<T&>(m_Event));
            }
            return *this;
        }

        // 2. Перегрузка ТОЛЬКО для функций, возвращающих void
        template<typename T, typename F>
        typename std::enable_if_t<std::is_same_v<void, std::invoke_result_t<F, T&>>, EventDispatcher&>
            Dispatch(const F& func) {
            if (!m_Event.Handled && m_Event.GetEventType() == T::GetStaticType()) {
                func(static_cast<T&>(m_Event));
            }
            return *this;
        }

        // 3. Удобный оператор () как альтернатива вызову .Dispatch
        template<typename T, typename F>
        EventDispatcher& operator()(const F& func) {
            return Dispatch<T>(func);
        }

    private:
        Event& m_Event;
    };

/*  =========== Exemple for use classic method =================

     EventDispatcher(e)
         .Dispatch<KeyPressedEvent>([this](auto& key) {
             m_KeyJustPressed[static_cast<KeyCode>(key.KeyCode)] = true;
         })
         .Dispatch<KeyReleasedEvent>([this](auto& keyUp) {
             m_KeyJustReleased[static_cast<KeyCode>(keyUp.KeyCode)] = true;
         })
    =============================================================
*/

    // Макрос автоматически создает лямбду с захватом this и правильным типом аргумента
#define EVENT_BIND(EventClass, FuncBody) [this](EventClass& e) FuncBody

/* =========== Exemple for use macros ===========================

    EventDispatcher dispatch(e);

    dispatch<KeyPressedEvent>          (EVENT_BIND(KeyPressedEvent,          { m_KeyJustPressed[static_cast<KeyCode>(e.KeyCode)] = true; }));
    dispatch<KeyReleasedEvent>         (EVENT_BIND(KeyReleasedEvent,         { m_KeyJustReleased[static_cast<KeyCode>(e.KeyCode)] = true; }));
    dispatch<MouseMovedEvent>          (EVENT_BIND(MouseMovedEvent,          { m_MousePosition = glm::vec2(e.MouseX, e.MouseY); }));
    dispatch<MouseButtonPressedEvent>  (EVENT_BIND(MouseButtonPressedEvent,  { m_MouseJustPressed[e.Button] = true; }));
    dispatch<MouseButtonReleasedEvent> (EVENT_BIND(MouseButtonReleasedEvent, { m_MouseJustReleased[e.Button] = true; }));

    ============================================================
*/

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

    class MouseButtonPressedEvent : public Event {
    public:
        MouseButtonPressedEvent(MouseButton button) : m_Button(button) {}
        MouseButton m_Button;
        EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class MouseButtonReleasedEvent : public Event {
    public:
        MouseButtonReleasedEvent(MouseButton button) : m_Button(button) {}
        MouseButton m_Button;
        EVENT_CLASS_TYPE(MouseButtonReleased)
    };

    // Пока достаточно. Позже можно добавить  WindowResize и т.д.

} // namespace NK
