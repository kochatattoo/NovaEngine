# Subsystem: Input (InputSystem, KeyCodes)

> 🚧 **TODO:** страница-заготовка.

## Два слоя

- **`Input` (legacy)** — `static class`, опрашивает `GetAsyncKeyState`, `GetCursorPos`. Может давать **экранные** координаты — осторожно.
- **`InputSystem`** (рекомендуемый) — singleton, событийная модель, хранит `m_KeysHeld`, `m_MouseButtonsHeld`, `m_KeyJustPressed/Released`. **Клиентские** координаты.

## Главное правило

| API | Координаты | Подходит для игр? |
|---|---|---|
| `Input::GetMousePosition` | **Экранные** | ❌ |
| `InputSystem::GetMousePosition` | **Клиентские** | ✅ |
| `Window::GetMouseClientPosition` | **Клиентские** | ✅ |
| `OrthographicCamera::ScreenToWorldPoint` | ожидает **клиентские** | — |

## Lua API (только новый!)

- `GetKey(KeyCode.X)`, `GetKeyDown`, `GetKeyUp`.
- `GetMouseButton(MouseButton.Left)`, `GetMouseButtonDown/Up`.
- `GetMousePos()` — клиентские.

❌ Не используй legacy: `IsMouseButtonDown(1)`, `GetMousePosition()`.

→ [полная документация](../docs/02_Подсистемы/07_Ввод.md)
