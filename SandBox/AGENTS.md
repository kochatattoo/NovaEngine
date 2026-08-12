# SandBox/ — правила для ИИ-агентов

> **Что тут:** исполняемый проект `SandBox.exe` — референсная игра Match3.
> **Точка входа:** `Source/Game.cpp::CreateApplication` (создаёт `SandboxApp`).
> **Корневой контекст:** [../AGENTS.md](../AGENTS.md)

---

## 1. Что здесь лежит

| Папка/файл | Назначение |
|---|---|
| `Source/Game.cpp` | `SandboxApp : public NK::Application` — точка входа игры |
| `Source/Game/Match3Board.h/.cpp` | POD-логика поля (legacy, v0.1). В v0.2 заменён на `Match3System`. |
| `Source/Game/Match3Game.h/.cpp` | Связка C++ ↔ Lua (биндинги, Start/Update). v0.2: owns World + Match3System. |
| `Source/Game/ECS/Match3System.h/.cpp` | v0.2: ECS-система — owns grid + 100 tile-entities. Заменяет `Match3Board`. |
| `Source/Game/ECS/Match3TileComponent.h` | v0.2: POD-компонент плитки (Row, Col, Type). |
| `assets/scripts/` | Lua-скрипты (game.lua, game_match3.lua, ui_test.lua) |
| `assets/textures/` | PNG-текстуры (test.png) |
| `assets/ui/` | Кнопки, иконки (button.png) |
| `assets/fonts/` | TTF-шрифты (arial.ttf, Samson.ttf) |
| `SandBox.vcxproj` | MSBuild-проект |

## 2. Post-build событие

`SandBox.vcxproj` копирует `assets/` в `$(OutDir)assets\` после сборки:

```bat
cmd /c xcopy /y /d "$(ProjectDir)assets" "$(OutDir)assets\"
```

**Не удалять** — без этого Lua-скрипты не найдутся в runtime.

## 3. Как добавить новую игру

1. Создать `Source/Game/MyGame.h/.cpp` (по образцу `Match3Game`).
2. В `MyGame::MyGame()` — получить `Engine::Get().GetScene()` и `GetLuaManager()`, вызвать `SetupLuaBindings()`.
3. В `MyGame::SetupLuaBindings()` — зарегистрировать специфичные для игры классы/функции.
4. В `Source/Game.cpp::SandboxApp::OnStart()` — заменить `m_Game = std::make_unique<NK::MyGame>(); m_Game->Start();`.
5. Создать `assets/scripts/my_game.lua` с `OnStart()` / `OnUpdate(dt)`.
6. В `MyGame.h` — `m_ScriptPath = "assets/scripts/my_game.lua";`.

## 4. Lua в SandBox

### 4.1 Что регистрируется автоматически

`Engine::SetupLuaBindings()` → `LuaBindings::RegisterAll()`:

- Классы: `GameObject`, `Scene`, `Font`, `vec2`.
- Компоненты: `Transform`, `SpriteRenderer`, `TextRenderer`, `Button`, `Anchor`.
- Камеры: `OrthographicCamera`.
- Функции: `Log`, `EngineVersion`, `GetEngine`, `GetScene`, `GetTexture`, `GetShader`, `LoadFont`, `GetMousePosition` (через `Window`!), `GetWindowWidth/Height`.
- Input: `KeyCode`, `MouseButton`, `GetKey`, `GetKeyDown/Up`, `IsKeyDown`, `GetMouseButton`, `GetMouseButtonDown/Up`, `GetMousePos`.
- Material/SolidColorTexture (через `Match3Game`).

### 4.2 Специфичные для Match3

В `Match3Game::SetupLuaBindings`:

- Класс `Match3Board` со всеми методами.
- Функция `CreateSolidColorTexture(r, g, b, a)` → 1×1 текстура.

## 5. Подводные камни

### 5.1 SetScale на Transform игнорируется

`SpriteRenderer::Render` для игрового мира не использует `transform::GetScale()`. Используй `SetCustomSize` для размера.

### 5.2 GetMousePosition — экранные (баг)

`Input::GetMousePosition` (через legacy `Input.h`) даёт экранные координаты. В Lua **не использовать** — `LuaInputBindings::GetMousePosition` уже удалён, используй `GetMousePos()` (клиентские).

### 5.3 Размер плитки должен быть вычислен явно

`CreateSolidColorTexture(...)` создаёт 1×1 текстуру. С `m_PixelsPerUnit=100` (default) плитка будет 0.01 мировых единиц = 1 экранный пиксель. **В `OnTileChanged` обязательно:**

```lua
sr:SetCustomSize(CELL_SIZE / PPU, CELL_SIZE / PPU)
sr:SetPixelsPerUnit(PPU)
```

### 5.4 Lua bindings дубликаты

`LuaInputBindings` идёт **после** `LuaFuncBindings` и перезаписывает дубликаты. Не дублируй регистрации.

### 5.5 Match3Board — safety

`Match3Board::Mix` имеет safety limit 50 попыток + warning. `FillEmpty` не создаёт начальный матч (v0.1.1).

### 5.6 Match3 v0.2 — Match3System вместо Match3Board

**v0.2:** `Match3System` (ECS) полностью заменил `Match3Board` (POD) в runtime. `Match3Board` оставлен в коде для reference, **будет удалён в v0.2.7**.

**Изменения в Lua:** `board = Match3Board.new(...)` → `board = GetBoard()`. Всё остальное (`board:Swap`, `board:FindMatches`) работает как раньше (Match3System забиндан под именем `Match3Board` для обратной совместимости).

**Двойное хранение:** в v0.2 Match3System спавнит 100 ECS-entities (с Name/Transform/Sprite/Match3Tile), но НЕ рендерит их. Lua в `OnTileChanged` callback по-прежнему создаёт Scene::GameObject'ы для визуализации. Это будет исправлено в v0.2.6 (SpriteRenderSystem).

**Порядок Start():** `Match3Game::Start()` сначала `RunScript + OnStart` (Lua ставит OnTileChanged), потом `m_System->Start()` (спавнит entities + FillRandom). Если поменять местами — 100 событий потеряются (OnTileChanged будет nullptr в момент FillRandom).

## 6. Что нельзя трогать

| Файл/папка | Почему |
|---|---|
| `SandBox.vcxproj.filters` | IDE-метаданные |
| `assets/scripts/` Lua-файлы | Удалить = сломать референсную игру |
| `assets/fonts/`, `assets/textures/` | Используются в runtime |

`Source/Game.cpp`, `Match3Board.cpp`, `Match3Game.cpp` — менять можно, после правки **обязательно** собрать и проверить что Match3 играется.

## 7. Стиль

См. [../AGENTS.md](../AGENTS.md) §5. Дополнительно:

- `Match3Board` — pure logic, **без зависимостей от `GameObject`/rendering**. Только данные + `OnTileChanged` callback.
- `Match3Game` — мост между C++ и Lua. Не содержит игровой логики.
- `game_match3.lua` — UI/visualization/input. Тонкий слой над `Match3Board`.
