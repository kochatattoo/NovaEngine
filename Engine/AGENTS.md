# Engine/ — правила для ИИ-агентов

> **Что тут:** статическая библиотека `Engine.lib` — все подсистемы движка.
> **Точка входа:** `Engine/Engine.cpp::Engine::Run`.
> **Корневой контекст:** [../AGENTS.md](../AGENTS.md)

---

## 1. Что здесь лежит

| Папка | Назначение |
|---|---|
| `Engine.cpp` | Точка входа движка (singleton Engine, Run, Initialize, Shutdown) |
| `pch.cpp` / `pch.h` / `framework.h` | Precompiled headers (отключены, но файлы есть) |
| `Include/` | **Публичные заголовки** — то, что могут включать Sandbox/ и другие клиенты |
| `Source/` | Реализации — зеркально Include/ |

## 2. Подсистемы в `Include/`

| Подсистема | Ключевые классы | Назначение |
|---|---|---|
| `Core/` | `Engine`, `Application`, `EntryPoint`, `Log`, `Timer` | Ядро движка |
| `Window/` | `Window` | WinAPI окно |
| `Input/` | `Input` (legacy), `InputSystem`, `KeyCodes` | Ввод |
| `Renderer/` | `Renderer`, `Shader`, `Texture2D`, `Font`, `SpriteRenderer`, `TextRenderer`, `Camera`, `OrthographicCamera`, `VertexArray/Buffer/IndexBuffer` | 2D-рендеринг |
| `Scene/` | `Scene`, `GameObject`, `Component`, `Transform`, `ScriptComponent` | Классическая иерархия объектов |
| `ECS/` | `World`, `TransformComponent`, `SpriteComponent`, `NameComponent` | **ECS-каркас (v0.2.6: рендер из ECS, v0.2.8: Lua биндинги)** |
| `ECS/Systems/` | `SpriteRenderSystem` | **v0.2.6: рендер entities через OpenGL** |
| `UI/` | `Button`, `Anchor` | UI-компоненты |
| `Resource/` | `ResourceManager`, `ResourcePool` | Кэш текстур/шейдеров |
| `Lua/` | `LuaManager`, `LuaBindings`, `LuaClass/Component/Camera/ECS/Func/InputBindings` | Скриптинг |
| `Event/` | `Event`, `EventDispatcher` | События |
| `glad/` | `gl.h`, `khrplatform.h` | Сгенерированные GL-объявления |

## 3. Правила для подсистем

### 3.1 Общее

- **Публичные API** в `Include/`, реализации в `Source/`. Зеркально.
- **Не дублируй** функционал между подсистемами. Если два класса делают одно — объединяй.
- **Не делай breaking changes** в публичных заголовках без согласования. Sandbox зависит от них.

### 3.2 Renderer (OpenGL)

- **Шейдеры** хранятся в виде строк. Версионирование: `#version 330 core` (минимум).
- **Каждая новая GL-функция** = добавить `glad_gl*` объявление в `Include/glad/gl.h` + явную загрузку в `Source/Renderer/GraphicsContext.cpp::Init`. См. §6.1 корневого AGENTS.md.
- **VAO/VBO/IBO** — единичный static quad в `SpriteRenderer::s_QuadVAO`. Не создавать новые VAO на каждый кадр.
- **DefaultSprite shader** — в `Engine.cpp::Initialize`. `u_Color` uniform обязателен.

### 3.3 Input (InputSystem)

- **Не дублировать** legacy `Input` (статический опрос). `InputSystem` — основной.
- **m_KeysHeld, m_MouseButtonsHeld** — состояние "удерживается".
- **m_KeyJustPressed, m_KeyJustReleased, m_MouseJustPressed, m_MouseJustReleased** — состояние "только что", сбрасывается в `ResetOneShotStates()`.
- **m_MouseScroll** — накопительная дельта за кадр.
- **m_MousePosition** — клиентские координаты из `WM_MOUSEMOVE.lParam`.

### 3.4 ECS (v0.2)

- **POD-структуры** в `Include/ECS/Components/`. Без методов, только данные.
- **`World`** — обёртка над `entt::registry`. Не светить `entt::entity` в публичных API напрямую — используй `GetEntityByName(name)` для name-based lookup.
- **Game-specific компоненты** (например, `Match3TileComponent`) — в `SandBox/Source/Game/ECS/`, НЕ в `Engine/Include/ECS/Components/`.
- **Game-specific системы** (например, `Match3System`) — в `SandBox/Source/Game/ECS/`, НЕ в `Engine/Source/ECS/`.
- **Engine-уровневые системы** (SpriteRenderSystem, CameraSystem) будут в `Engine/Source/ECS/Systems/` (TODO v0.2.6+).
- **Сейчас Scene/GameObject/Component** — legacy. Match3 уже мигрирован на ECS-логику (v0.2.3), но рендер пока в Scene/GameObject.
- **`NameComponent`** (v0.2.1) — стандартный компонент-имя. Добавляется через `World::CreateEntity(name)` или `RenameEntity`. Хранится `m_NamedEntities: unordered_map<string, entity>` в World.

### 3.5 Lua

- **Биндинги** разделены по файлам: `LuaClass/Component/Camera/ECS/Func/InputBindings`. Не смешивать.
- **Порядок регистрации** в `LuaBindings::RegisterAll` важен: последняя `RegisterFunction("name")` перезаписывает. Дубликаты — баг.
- **`Input::GetMousePosition`** (legacy) даёт экранные координаты. **НЕ использовать в Lua** — `LuaInputBindings::GetMousePosition` уже удалён, остались только клиентские варианты (`GetMousePos`, `Window::GetMouseClientPosition`).

## 4. Подводные камни (Engine-специфичные)

### 4.1 Двойной вызов `Scene::OnUpdate`

`Engine::Run` вызывает `m_Scene.OnUpdate(dt)`, потом `m_App->OnUpdate(dt)`. В `Sandbox/Source/Game.cpp::OnUpdate` ещё раз вызывается `m_Scene.OnUpdate(dt)`. Дублирование. Безвредно, TODO v0.2.

### 4.2 glm::vec2 overloads

`SpriteRenderer::SetCustomSize(vec2)` и `SetCustomSize(float, float)` — два overloads в inline-заголовке. **В lambda для sol2 ВСЕГДА disambiguate** через `glm::vec2(...)` или `static_cast<>`. См. §6.2 корневого AGENTS.md.

### 4.3 Camera (перспективная) — legacy

`Renderer/Camera.h` (перспективная) — НЕ ИСПОЛЬЗУЕТСЯ, TODO удаление. Используй `OrthographicCamera` (2D).

### 4.4 SpriteRenderer игнорирует Transform::SetScale

Для игрового мира (`!m_IsUI`) model matrix не использует `transform::GetScale()`. Только `m_CustomSize`. TODO v0.2.

## 5. Что нельзя трогать в Engine/

- `glad/gl.h`, `glad/khrplatform.h` — сгенерированные.
- Любой `.vcxproj.filters` — IDE-метаданные.
- Lua-исходники в `Libraries/lua/src/`.

`Engine.vcxproj` — менять можно, но после правки собрать и убедиться, что не сломалось.

## 6. Тесты

- Тестов нет (TODO v0.2+).
- Планируется Catch2/doctest (см. `docs/06_План_разработки/05_Backlog_идеи.md`).
- Пока — ручная проверка: `F5` в Visual Studio, визуально смотрим что Match3 играется.

## 7. Стиль кода

См. [../AGENTS.md](../AGENTS.md) §5. Дополнительно:

- **Header guards:** `#pragma once` (НЕ `#ifndef`).
- **Порядок include'ов** в .cpp: сначала paired .h, потом C++ std, потом project.
- **Forward declarations** предпочтительны в .h, полные include'ы в .cpp.
- **Конструкторы** — explicit где одна переменная.
- **Doxygen-комментарии** — для публичного API в `Include/`. На русском.
