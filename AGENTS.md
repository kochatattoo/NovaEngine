# NovaEngine — правила для ИИ-агентов

> **Проект:** учебный игровой движок на C++17, OpenGL 3.3, Lua, EnTT. Только Windows.
> **Текущая версия кода:** v0.2. ECS-миграция Match3 в процессе (логика мигрирована, рендер — следующий шаг).

Корневой файл инструкций. Читается агентом при работе с любым файлом проекта.

---

## 1. Проект

- **Назначение:** движок общего назначения + референсная игра Match3.
- **Платформа:** Windows (WinAPI + WGL). Кроссплатформенность — TODO.
- **Стек:** C++17, OpenGL 3.3 Core, Lua 5.4, EnTT ECS, glm, sol2, stb_image, stb_truetype, glad (ручная загрузка), vcpkg (только glm).
- **Сборка:** Visual Studio 2022, MSBuild/vcxproj. CMake — TODO.
- **Корень проекта:** `D:\ENGINE\NovaEngine\`
- **Документация:** `D:\ENGINE\NovaEngine\docs\` (системный справочник) + OneDrive (история).

## 2. Команды

```powershell
# Visual Studio 2022 (рекомендуется):
Ctrl+Shift+B         # Build Solution
F5                   # Run с дебагом
Ctrl+F5              # Run без дебага

# Из PowerShell (альтернатива):
msbuild NovaEngine.sln /p:Configuration=Debug /p:Platform=x64
.\x64\Debug\SandBox.exe
```

Тестов — нет (TODO v0.2+). `vcpkg install glm:x64-windows` нужен перед первой сборкой.

## 3. Структура

```
NovaEngine/
├── Engine/                  # статическая библиотека .lib — все подсистемы
│   ├── Include/             # публичные заголовки
│   │   ├── Core/            # Engine, Application, EntryPoint, Log, Timer
│   │   ├── Window/          # WinAPI окно
│   │   ├── Input/           # Input (legacy), InputSystem, KeyCodes
│   │   ├── Renderer/        # SpriteRenderer, TextRenderer, Shader, Texture2D, Font, ...
│   │   ├── Scene/           # GameObject, Component, Transform, ScriptComponent
│   │   ├── ECS/             # World, TransformComponent, SpriteComponent
│   │   ├── UI/              # Button, Anchor
│   │   ├── Resource/        # ResourceManager, ResourcePool
│   │   ├── Lua/             # LuaManager, биндинги
│   │   └── Event/           # Event, EventDispatcher
│   └── Source/              # реализации (зеркально Include/)
├── SandBox/                 # исполняемый .exe — игра Match3
│   ├── Source/Game/         # Match3Board, Match3Game
│   ├── assets/
│   │   ├── scripts/         # Lua-скрипты
│   │   ├── textures/         # PNG/JPG
│   │   ├── fonts/            # TTF
│   │   └── ui/               # кнопки и т.п.
│   └── SandBox.vcxproj
├── Libraries/               # внешние (НЕ РЕДАКТИРОВАТЬ без явной причины)
│   ├── glm/                 # через vcpkg
│   ├── lua/src/             # исходники Lua 5.4, компилируются в Engine.lib
│   ├── sol/                 # C++ биндинги Lua
│   └── EnTT/                # ECS
├── docs/                    # системная документация (см. docs/AGENTS.md)
├── vcpkg/, vcpkg_installed/, x64/  # НЕ РЕДАКТИРОВАТЬ
├── .agents/                 # служебные инструкции (MD-IMPROVER и пр.)
└── NovaEngine.sln
```

Подробности по подсистемам — в `Engine/AGENTS.md`, `SandBox/AGENTS.md`, `docs/AGENTS.md`.

## 4. Что нельзя трогать

| Файл/папка | Почему |
|---|---|
| `Libraries/` | Внешние зависимости. Правки ломают проект. |
| `vcpkg_installed/`, `x64/` | Артефакты сборки. Пересоздаются. |
| `Engine/Source/glad/gl.c`, `Engine/Include/glad/gl.h` | Сгенерированы glad-loader'ом. Ручная правка бессмысленна. |
| `Engine/Include/glad/khrplatform.h` | То же. |
| `*.vcxproj.filters` | IDE-метаданные, генерируются автоматически. |
| `Libraries/lua/src/*.c` | Исходники Lua 5.4, менять только при апгрейде версии. |

`Engine.vcxproj` / `SandBox.vcxproj` — менять можно, но аккуратно: сначала собрать, потом смотреть, не сломались ли include-paths.

## 5. Правила кода

### 5.1 Нейминг

| Сущность | Стиль | Пример |
|---|---|---|
| Класс, struct, enum, метод, свойство, публичное поле | `PascalCase` | `Engine`, `Match3Board`, `IsKeyDown` |
| Приватное поле | `m_camelCase` | `m_Running`, `m_MousePosition` |
| Локальная переменная, параметр | `camelCase` | `targetPosition` |
| `static const` поле | `s_PascalCase` или `kPascalCase` | `s_ClearColor` |
| Константа/enum-значение | `PascalCase` | `MaxTargetCount` |
| Namespace | `NK` (от NovaKernel), подмодули `NK::ECS` | |
| Логгер: client / core | `NK_INFO` / `NK_CORE_INFO` | |
| Файл | соответствует классу | `Engine.h` → `class Engine` |

### 5.2 C++

- C++17, `#pragma once`, RAII.
- `std::unique_ptr` для владельцев, `std::shared_ptr` для общего владения (renderer, resources).
- `nullptr` вместо `NULL`/`0`.
- `auto` только когда тип очевиден из правой части.
- `const` где возможно.
- Header-only только для шаблонов.
- Для новых GL-функций — добавить `glad_gl*` в `glad/gl.h` И явный `GetAnyGLFuncAddress` в `GraphicsContext::Init`.

### 5.3 Lua

- Новые скрипты — в `assets/scripts/<name>.lua`.
- Точки входа: `OnStart()`, `OnUpdate(dt)`, `OnShutdown()`.
- Используй **новый API**: `GetMouseButton(MouseButton.Left)`, `GetMousePos()`, `EngineVersion()`.
- **НЕ используй** legacy `IsMouseButtonDown(1)`, `GetMousePosition()` (баг с экранными координатами, см. §6.3).

### 5.4 Документация

- Все новые/изменённые файлы кода → обнови соответствующую документацию в `docs/`.
- Формат: Markdown (CommonMark + GFM), UTF-8.
- Имена файлов: `NN_Название.md` (NN — двузначный номер).
- В конце каждого спринта — отчёт в `docs/Reports/YYYY-MM-DD.md`.

## 6. Подводные камни

### 6.1 GLAD загружается вручную

`Engine/Source/Renderer/GraphicsContext.cpp` явно получает указатели через `GetAnyGLFuncAddress(name)`. **Новая GL-функция = добавить в `glad/gl.h` И в список загрузки.** Без этого будет linker-ошибка или тихий `nullptr` → crash.

### 6.2 Overload resolution в inline-заголовках

`SpriteRenderer` имеет два overloads `SetCustomSize`: `SetCustomSize(const glm::vec2&)` и `SetCustomSize(float, float)`. В lambda для sol2 **ВСЕГДА disambiguate**:

```cpp
// ❌ linker-ошибка
"SetCustomSize", [](SpriteRenderer& sr, double w, double h) {
    sr.SetCustomSize((float)w, (float)h);
}

// ✅ OK
"SetCustomSize", [](SpriteRenderer& sr, double w, double h) {
    sr.SetCustomSize(glm::vec2(static_cast<float>(w), static_cast<float>(h)));
}
```

### 6.3 Mouse coordinates: экранные vs клиентские

| API | Координаты | Через |
|---|---|---|
| `Input::GetMousePosition(int, int)` | **Экранные** (от монитора) | `GetCursorPos` |
| `InputSystem::GetMousePosition()` | **Клиентские** (от окна) | `WM_MOUSEMOVE.lParam` |
| `Window::GetMouseClientPosition(int, int)` | **Клиентские** (от окна) | `GetCursorPos` + `ScreenToClient` |
| `OrthographicCamera::ScreenToWorldPoint` | ожидает **клиентские** | |

**В Lua использовать только:**
- `GetMousePos()` (через `InputSystem`) — клиентские ✅
- `GetMousePosition()` через `Window::GetMouseClientPosition` — клиентские ✅

### 6.4 Transform::SetScale игнорируется в игровом мире

`SpriteRenderer::Render` для `!m_IsUI` строит model matrix **только** из `m_CustomSize / 2`, без `transform::GetScale()`. Поэтому `obj:GetTransform():SetScale(1.2, 1.2)` для игровых объектов **не даёт визуального эффекта**. Для изменения размера используй `SpriteRenderer::SetCustomSize`. **v0.2.6+:** планируется `SpriteRenderSystem` — рендер напрямую из `TransformComponent::Scale`.

### 6.4a ECS World — name-based lookup (v0.2.1)

`NK::ECS::World` поддерживает строковые имена entity через `NameComponent` + `std::unordered_map`:
- `CreateEntity(name)` — создать именованную entity
- `GetEntityByName(name)` → `entt::entity` (или `entt::null`)
- `RenameEntity(entity, newName)`, `GetEntityName(entity)`, `Clear()`

Используется в `Match3System` для плиток (`"tile_<row>_<col>"`). Для ECS-рендера имя типа `"player_main"` будет использоваться для редактора и Lua.

### 6.4b EnTT include path

Include path в vcxproj = `Libraries/EnTT/`. Поэтому:
- ✅ `#include <entt/entt.hpp>` (путь = `Libraries/EnTT/entt/entt.hpp`)
- ❌ `#include <EnTT/entt/entt.hpp>` (лишний `EnTT/`)
- Namespace `entt` — top-level, не `NK::ECS::entt`. Используй `entt::entity`.

### 6.5 Double `Scene::OnUpdate`

`Engine::Run` вызывает `m_Scene.OnUpdate(dt)` И `m_App->OnUpdate(dt)`. В `Game.cpp::OnUpdate` снова вызывается `m_Scene.OnUpdate(dt)`. **Двойной вызов за кадр.** Безвредно (идемпотентно), TODO v0.2+ — убрать один.

### 6.6 `Application::OnEvent` удалён (v0.1.1)

Был мёртвым. Вместо него — `InputSystem::Get().GetKeyDown(...)` в `OnUpdate`.

### 6.7 Lua bindings перезаписываются

В sol2 последняя `RegisterFunction("name", ...)` перезаписывает предыдущую. `LuaInputBindings::RegisterAll` вызывается **после** `LuaFuncBindings::RegisterAll`, поэтому перезаписывает. **Дубликаты** — частая ошибка.

### 6.8 DefaultSprite использует `u_Color`

`Engine.cpp::Initialize` шейдер содержит `uniform vec4 u_Color;`. По умолчанию `(1, 1, 1, 1)` (без изменений). `SpriteRenderer::SetColor(r, g, b, a)` реально работает только с v0.1.2+.

## 7. Git

- **Не коммитить автоматически.** Только по явному запросу пользователя.
- Теги: `v0.1.0`, `v0.1.1`, `v0.1.2`, `v0.1.3`, `v0.2.0` (по завершении спринта).
- Формат коммита: `v0.X.Y: краткое описание изменений`.
- Сообщение коммита — на русском.

## 8. Подсистемы

| Папка | Содержимое | Свой AGENTS |
|---|---|---|
| `Engine/` | Все подсистемы движка | [Engine/AGENTS.md](Engine/AGENTS.md) |
| `SandBox/` | Match3 + game-specific | [SandBox/AGENTS.md](SandBox/AGENTS.md) |
| `docs/` | Системная документация | [docs/AGENTS.md](docs/AGENTS.md) |
| `Libraries/` | Внешние (не трогать) | — |

## 9. Дополнительные инструкции

- [`.agents/MD-IMPROVER.md`](.agents/MD-IMPROVER.md) — процедура аудита и улучшения AGENTS.md.
- [`.agents/AGENTS.md`](.agents/AGENTS.md) — шаблон AGENTS.md (для справки по формату и стилю).

## 10. Стоп-сигналы

Остановить работу и спросить пользователя, если:

- Правка `Libraries/`, `vcpkg_installed/`, `x64/`.
- Изменение GLAD-файлов (`glad/gl.h`, `glad/gl.c`, `khrplatform.h`).
- Изменение namespace `NK` (может сломать весь код).
- Изменение `Engine.vcxproj` / `SandBox.vcxproj` без проверки компиляции.
- Любое изменение, затрагивающее > 5 файлов, без явного согласования.
- Удаление или переименование публичного API (классы в `Engine/Include/`).
- Изменение `Sandbox/assets/scripts/*.lua` без обновления `docs/03_Скриптинг_на_Lua/`.
- Найден баг, ломающий компиляцию, — сообщить, не чинить молча.
