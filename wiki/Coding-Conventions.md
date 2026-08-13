# Coding Conventions

> 🚧 **TODO:** страница-заготовка. Полные правила — в [`AGENTS.md`](../AGENTS.md).

## Нейминг

| Сущность | Стиль | Пример |
|---|---|---|
| Класс, struct, метод | `PascalCase` | `Engine`, `Match3System` |
| Приватное поле | `m_camelCase` | `m_Running` |
| Локальная переменная, параметр | `camelCase` | `targetPosition` |
| `static const` поле | `s_PascalCase` | `s_ClearColor` |
| Константа/enum-значение | `PascalCase` | `MaxTargetCount` |
| Namespace | `NK` (от NovaKernel), подмодули `NK::ECS` | |
| Файл | соответствует классу | `Engine.h` → `class Engine` |

## C++

- **C++17**, `#pragma once`, RAII.
- `std::unique_ptr` для владельцев, `std::shared_ptr` для общего владения.
- `nullptr` вместо `NULL`/`0`.
- `auto` только когда тип очевиден.
- `const` где возможно.
- Header-only только для шаблонов.

## Lua

- Точки входа: `OnStart()`, `OnUpdate(dt)`, `OnShutdown()`.
- Новый API ввода: `GetMousePos()`, `GetMouseButton(MouseButton.Left)`, `GetKey(KeyCode.A)`.
- ❌ Не использовать legacy: `IsMouseButtonDown(1)`, `GetMousePosition()` (экранные, баг).

## Git

- Не коммитить автоматически — только по запросу.
- Формат: `v0.X.Y: краткое описание`.
- Сообщения — на русском.
- Теги: `v0.1.0`, `v0.1.1`, … `v0.2.0` (по завершении спринта).

## Документация

- Каждое изменение в коде → обновить `docs/`.
- Имена файлов: `NN_Название.md` (NN — двузначный номер).
- В конце спринта — отчёт `docs/Reports/YYYY-MM-DD.md`.
