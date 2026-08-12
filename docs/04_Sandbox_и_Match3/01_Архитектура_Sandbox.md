# 01 — Архитектура Sandbox-приложения

> **Статус:** 🟢 Актуально для v0.1

Sandbox — это **эталонный пример** того, как писать игру на NovaEngine. Сейчас в нём живёт Match3. Когда захочется писать другую игру — создаёшь новый `Application`-класс и подключаешь свой скрипт.

**Файлы:**
- `SandBox/Source/Game.cpp` — `SandboxApp`
- `SandBox/Source/Game/Match3Game.h/.cpp` — связка с Lua + специфичные биндинги
- `SandBox/Source/Game/Match3Board.h/.cpp` — игровая логика
- `SandBox/assets/scripts/*.lua` — Lua-скрипты

## Класс `SandboxApp`

**Файл:** `SandBox/Source/Game.cpp`

```cpp
#include <Core/EntryPoint.h>
#include <Core/Log.h>
#include <Input/InputSystem.h>
#include <Renderer/Renderer.h>

#include "Game/Match3Game.h"

class SandboxApp : public NK::Application {
public:
    SandboxApp() = default;

    void OnStart() override {
        m_Game = std::make_unique<NK::Match3Game>();
        m_Game->Start();
    }

    void OnUpdate(float deltaTime) override {
        // 1. Обновить все объекты сцены (вызывают Lua-скрипты через ScriptComponent)
        NK::Engine::Get().GetScene().OnUpdate(deltaTime);

        if (m_Game) m_Game->Update(deltaTime);

        // 2. Начало кадра
        NK::Renderer::BeginFrame();

        // 3. Рисуем все игровые объекты через SpriteRenderer
        NK::Engine::Get().GetScene().OnRender();

        // 4. Завершающий кадр
        NK::Renderer::EndFrame();
    }

    void OnShutdown() override {
        NK_INFO("Sandbox shutting down.");
    }

private:
    std::unique_ptr<NK::Match3Game> m_Game;
};

NK::Application* NK::CreateApplication() {
    return new SandboxApp();
}
```

### Что тут важно

1. **`#include <Core/EntryPoint.h>`** — единственное место, где есть `main()`. Нигде больше его не должно быть.
2. **`CreateApplication`** — единственная точка, где движок узнаёт о твоём классе.
3. **`m_Game->Start()`** — загружает `game_match3.lua` и вызывает `OnStart()`.
4. **`OnUpdate(dt)`** — обновляет сцену, потом Lua-скрипт, потом рендерит.

### v0.1.1: убрано

- `m_ColorR/G/B` — мусор от ранней версии (пробел менял цвет). Удалён.
- Тестовый ECS-код в `OnStart` — заглушка. Удалён.
- `OnEvent` override — мёртвый (метод удалён из `Application` в v0.1.1). Удалён.
- `#include "Input/Input.h"` → `#include "Input/InputSystem.h"` (используем только новый API).
- `#include "Event/Event.h"` — не нужен, убран.

## Класс `Match3Game`

**Файл:** `SandBox/Source/Game/Match3Game.h/.cpp`

```cpp
class Match3Game {
public:
    Match3Game();
    ~Match3Game() = default;

    void Start();
    void Update(float deltaTime);

private:
    void SetupLuaBindings();

    std::unique_ptr<Match3Board> m_Board;
    Scene* m_Scene;
    LuaManager& m_Lua;
    std::string m_ScriptPath = "assets/scripts/game_match3.lua";
};
```

### Конструктор

```cpp
Match3Game::Match3Game()
    : m_Scene(&Engine::Get().GetScene()),
      m_Lua(Engine::Get().GetLuaManager())
{
    SetupLuaBindings();
}
```

Получает из движка `Scene` и `LuaManager`, регистрирует специфичные для Match3 биндинги.

### `SetupLuaBindings()`

```cpp
void Match3Game::SetupLuaBindings() {
    m_Lua.BindClass<Match3Board>("Match3Board",
        sol::constructors<Match3Board(int, int, double, double)>(),
        "FillRandom", &Match3Board::FillRandom,
        "GetTile", &Match3Board::GetTile,
        "SetTile", &Match3Board::SetTile,
        "IsValidCell", &Match3Board::IsValidCell,
        "Swap", &Match3Board::Swap,
        "FindMatches", &Match3Board::FindMatches,
        "RemoveTiles", &Match3Board::RemoveTiles,
        "ApplyGravity", &Match3Board::ApplyGravity,
        "FillEmpty", &Match3Board::FillEmpty,
        "HasPossibleMoves", &Match3Board::HasPossibleMoves,
        "Mix", &Match3Board::Mix,
        "GetCellPosition", [](Match3Board& board, int r, int c) -> std::tuple<float, float> {
            auto pos = board.GetCellPosition(r, c);
            return { pos.x, pos.y };
        },
        "GetRows", &Match3Board::GetRows,
        "GetCols", &Match3Board::GetCols,
        "OnTileChanged", &Match3Board::OnTileChanged
    );

    m_Lua.RegisterFunction("CreateSolidColorTexture", [](int r, int g, int b, int a) -> std::shared_ptr<Texture2D> {
        return Texture2D::CreateSolidColor((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a);
    });
    m_Lua.RegisterFunction("GetMousePosition", []() -> std::tuple<int, int> { … });
    m_Lua.RegisterFunction("GetWindowWidth", []() { return Engine::Get().GetWindow()->GetWidth(); });
    m_Lua.RegisterFunction("GetWindowHeight", []() { return Engine::Get().GetWindow()->GetHeight(); });
    m_Lua.RegisterFunction("IsMouseButtonDown", [](int button) -> bool { return Input::IsMouseButtonDown(button); });
    m_Lua.RegisterFunction("GetScene", [this]() -> Scene& { return *m_Scene; });
}
```

### `Start`

```cpp
void Match3Game::Start() {
    m_Lua.RunScript(m_ScriptPath);
    m_Lua.CallFunction("OnStart");
}
```

### `Update`

```cpp
void Match3Game::Update(float deltaTime) {
    m_Lua.CallFunction("OnUpdate", deltaTime);
}
```

## ⚠️ Что в Sandbox плохо

1. **`m_ColorR/G/B`** в `SandboxApp` — мусор, остался от ранней версии (пробел менял цвет). TODO: удалить.
2. **Тестовый ECS-код** в `OnStart` — заглушка, должна уйти в отдельный `Demo` или выпилиться.
3. **`OnEvent`** — мёртвый код (см. [11_События](../02_Подсистемы/11_События.md)).
4. **`Match3Game::SetupLuaBindings`** дублирует `GetMousePosition` и `GetScene`, которые уже есть в `LuaFuncBindings`. TODO: использовать общие биндинги.

## Как добавить новую игру

1. Создать `MyGame.h/.cpp` в `SandBox/Source/Game/` (по образцу `Match3Game`).
2. В `MyGame::SetupLuaBindings` зарегистрировать свои классы/функции.
3. В `MyGame::Start/Update` — загрузить и вызвать Lua.
4. В `SandboxApp::OnStart` создать `m_Game = std::make_unique<MyGame>(); m_Game->Start();`.
5. В `SandboxApp::OnUpdate` вызвать `m_Game->Update(dt)`.
6. Создать `assets/scripts/my_game.lua`.

Если хочется переключаться между играми — вынести выбор в `EngineConfig` (например, `config.StartupScript = "my_game"`).

## См. также

- [02_Игра_Match3](02_Игра_Match3.md) — как работает сама игра.
- [02_Подсистемы/10_Lua_интеграция](../02_Подсистемы/10_Lua_интеграция.md) — общие биндинги.
- [03_Скриптинг_на_Lua/01_Lua_API_справочник](../03_Скриптинг_на_Lua/01_Lua_API_справочник.md) — API.
