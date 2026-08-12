# 05 — Backlog идей и опциональные фичи

> **Горизонт:** неопределён
> **Цель:** собрать все "хотелки", которые не вошли в v0.2–v0.5

## Геймплей / Match3-специфичное

- [ ] **Специальные плитки** (`SpecialBomb`, `SpecialRainbow` — зарезервированы в `enum class TileType`).
  - [ ] 4-в-ряд → создаёт `SpecialBomb` на месте 4-й.
  - [ ] 5-в-ряд → `SpecialRainbow`.
  - [ ] Bomb: прилегающие 3×3 клетки уничтожаются.
  - [ ] Rainbow: заменяет все плитки выбранного цвета на rainbow.
- [ ] **Цели уровня**: "набери 1000 очков за 30 ходов", "собери 30 синих", "очисти все бомбы".
- [ ] **Система уровней**: progression, unlock, save/load прогресса.
- [ ] **Бустеры**: строка-бомба, столбец-бомба, swap-любых-двух, time-freeze.
- [ ] **Анимация встряски** (когда ходов нет).
- [ ] **Звуки**: match-sound, fail-sound, mix-sound, background-music.
- [ ] **Particle-эффекты** на совпадении (искры, конфетти).
- [ ] **Экран паузы / Game Over UI**.
- [ ] **Таблица рекордов** (highscores).
- [ ] **Tutorial** (первые 3 хода подсвечиваются).
- [ ] **Undo** (отмена последнего хода).

## Engine — низкоуровневые улучшения

### Производительность

- [ ] **Sprite batching** (см. [03_Среднесрочный_план](03_Среднесрочный_план.md)).
- [ ] **Instanced rendering** для одинаковых спрайтов.
- [ ] **Texture atlases** (объединить несколько мелких текстур в одну).
- [ ] **Job system / Multi-threading** (для asset loading, ECS систем).
- [ ] **Memory pool** для компонентов (избегать аллокаций на каждом `AddComponent`).
- [ ] **SIMD** для матричных операций (вместо glm — вручную SSE/AVX).

### Рендеринг

- [ ] **Render targets / FBO** (для post-effects, render-to-texture).
- [ ] **Post-processing**: blur, bloom, color grading, vignette.
- [ ] **2D-lighting** (normal maps + light entities).
- [ ] **Particle system** (CPU и GPU варианты).
- [ ] **Skeleton animation** (skinned mesh 2D).
- [ ] **Tilemap renderer** (специализированный для больших 2D-карт).
- [ ] **9-slice sprite** (для UI-кнопок с тянущимися краями).
- [ ] **Polygon sprites** (с произвольной формой, не только квадрат).
- [ ] **Слои рендеринга** (явные layers + камера видит только свой layer).

### Ресурсы

- [ ] **Asset Manager** с метаданными (формат, размер, зависимости).
- [ ] **Async asset loading** с прелоадер-сценой.
- [ ] **Hot reload** для Lua-скриптов и текстур.
- [ ] **Soft reference + LRU cache** (автовыгрузка неиспользуемых).
- [ ] **Compression** текстур (DXT/S3TC) — но не на OpenGL 3.3 без расширений.
- [ ] **mipmaps** для текстур (если будут 3D или большие 2D миры).
- [ ] **Video playback** (через ffmpeg или libvlc).
- [ ] **SVG-парсинг** (для масштабируемых UI).

### Сцена / объекты

- [ ] **Иерархия Transform** (parent-child).
- [ ] **Prefab** (шаблоны объектов).
- [ ] **Сериализация** сцен (JSON, YAML, бинарный).
- [ ] **Scene Manager** (стек сцен, переходы).
- [ ] **Component removal** API.
- [ ] **Component swap** (заменить один компонент другим).
- [ ] **Tag-компоненты** (пустые маркеры для фильтрации).

### Input

- [ ] **Action Maps** (абстракция над сырым вводом: "Jump" = Space + Gamepad A).
- [ ] **Input rebinding** UI.
- [ ] **Gamepad** (XInput, SDL GameController).
- [ ] **Touch** (для мобильных).
- [ ] **Gesture** (tap, swipe, pinch).

### UI

- [ ] **TextBox** (ввод текста с клавиатуры).
- [ ] **DropDown / ComboBox**.
- [ ] **TabControl** / **Accordion**.
- [ ] **Tooltip**.
- [ ] **Drag-and-drop** внутри UI.
- [ ] **Modal dialog**.
- [ ] **Themes** (light/dark).

### Lua

- [ ] **Coroutines** (`sol::coroutine`).
- [ ] **Sandbox** (безопасные библиотеки, лимит инструкций).
- [ ] **Hot reload** скриптов.
- [ ] **Debugging bridge** (Lua → Visual Studio breakpoints).
- [ ] **Profiler** для Lua (узкие места в скриптах).
- [ ] **Типизированные структуры** (LuaBridge-like).
- [ ] **Module system** (`require` с `package.path`).

### Звук

- [ ] **miniaudio** integration.
- [ ] **3D sound** (позиционный звук, doppler).
- [ ] **Mixer** (группы, эффекты).
- [ ] **Music streaming** (большие файлы с префетчем).
- [ ] **Voice/speech** (Text-to-Speech).

### Физика

- [ ] **Box2D** integration (или собственная 2D-физика).
- [ ] **Rigidbody2D, Collider2D** компоненты.
- [ ] **Collision events**.
- [ ] **Triggers** (зоны без физики).
- [ ] **Raycasting**.
- [ ] **Joints** (revolute, prismatic, distance).

### Сеть (если решим делать)

- [ ] **ENet / GameNetworkingSockets** integration.
- [ ] **Client/Server** abstraction.
- [ ] **Replication** entity/components.
- [ ] **RPC** (Remote Procedure Call).
- [ ] **Lag compensation**.
- [ ] **Matchmaking** (опционально).

### Editor

- [ ] **ImGui Inspector** (для отладки).
- [ ] **Visual Scene Editor** (drag-n-drop объектов в сцене).
- [ ] **Asset browser** (текстуры, шрифты, скрипты).
- [ ] **Console** (лог + Lua REPL).
- [ ] **Profiler UI** (FPS, draw calls, память).

### Кроссплатформенность

- [ ] **Linux** (X11 + GLX).
- [ ] **macOS** (Cocoa + NSOpenGL).
- [ ] **Web** (Emscripten + WebGL 2.0).
- [ ] **Android** (NDK + EGL).
- [ ] **iOS** (Metal или OpenGL ES).

### Сборка / DevOps

- [ ] **CMake** project (в дополнение к vcxproj).
- [ ] **vcpkg** для всех зависимостей (сейчас только glm).
- [ ] **Conan** (альтернатива vcpkg).
- [ ] **CI/CD** (GitHub Actions / Azure Pipelines).
- [ ] **Unit-тесты** (Catch2 / doctest / Google Test).
- [ ] **Coverage** (lcov + gcov).
- [ ] **Sanitizers** (ASan, UBSan, TSan) в debug-сборке.
- [ ] **Static analysis** (clang-tidy, CppCheck).
- [ ] **Установщик** (Inno Setup, WiX).
- [ ] **Steam / itch.io** publishing.

### Архитектура

- [ ] **Полная миграция на ECS** (см. [03_Среднесрочный_план](03_Среднесрочный_план.md)).
- [ ] **Удалить `Scene/GameObject/Component`** (или оставить как legacy).
- [ ] **Удалить `Input` (legacy)**, оставить только `InputSystem`.
- [ ] **Удалить `Camera` (перспективная)**.
- [ ] **Удалить `AddUIObject`**.
- [ ] **Удалить `OrthographicCamera` ScreenToWorld/UI** — вынести в утилитарную функцию.
- [ ] **Вынести `Window::WndProc` в `WindowsWindow`**, сделать `Window` абстрактным.
- [ ] **Вынести `WindowsOpenGLContext` в отдельный .cpp**, оставить в `GraphicsContext` только интерфейс.
- [ ] **PCH (precompiled headers)** — включить для ускорения компиляции.
- [ ] **CMake** — уход от MSBuild-only.

## Игры, которые можно сделать на NovaEngine

| Игра | Сложность | Что прокачает |
|---|---|---|
| **Puzzle Bobble** | Низкая | Сфера-физика, таймеры, scoring |
| **Platformer** | Средняя | Физика, ввод, камера-follow |
| **Top-down RPG** | Средняя | Карты, NPC, диалоги, инвентарь |
| **Tower Defense** | Средняя | Waves, AI, простая физика |
| **Visual Novel** | Низкая | UI, текст, диалоги, сохранения |
| **Beat-em-up** | Высокая | Анимации, коллизии, AI |
| **Bullet Hell** | Средняя | Много объектов (нагрузка на batching) |
| **Арканоид** | Низкая | Физика, разрушаемые блоки |

## Книги / ресурсы для изучения

- **Game Engine Architecture** (Jason Gregory) — must-read.
- **Real-Time Rendering 4** (Tomas Akenine-Möller).
- **The Cherno** (YouTube) — серия "Game Engine in C++".
- **Code:** Hazel (TheCherno), Kotor (Jessie), bsf (Serinity).
- **EnTT** docs — для глубокого понимания ECS.

См. также: [03_Среднесрочный_план](03_Среднесрочный_план.md), [04_Долгосрочный_план](04_Долгосрочный_план.md).
