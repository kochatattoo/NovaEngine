# 10 — Lua-интеграция (LuaManager, биндинги)

> **Статус:** 🟢 Стабильно, биндинги покрывают основные сценарии

**Файлы:**
- `Engine/Include/Lua/LuaManager.h`, `Engine/Source/Lua/LuaManager.cpp`
- `Engine/Include/Lua/LuaBindings.h`, `Engine/Source/Lua/LuaBindings.cpp`
- `Engine/Include/Lua/LuaClassBindings.h`, `Engine/Source/Lua/LuaClassBindings.cpp`
- `Engine/Include/Lua/LuaComponentBindings.h`, `Engine/Source/Lua/LuaComponentBindings.cpp`
- `Engine/Include/Lua/LuaCameraBindings.h`, `Engine/Source/Lua/LuaCameraBindings.cpp`
- `Engine/Include/Lua/LuaFuncBindings.h`, `Engine/Source/Lua/LuaFuncBindings.cpp`
- `Engine/Include/Lua/LuaInputBindings.h`, `Engine/Source/Lua/LuaInputBindings.cpp`

Основа скриптинга в NovaEngine. Использует [sol2](https://github.com/ThePhD/sol2) — header-only C++ обёртку над Lua C API.

## Класс `LuaManager`

**Файл:** `Engine/Include/Lua/LuaManager.h`

```cpp
class LuaManager {
public:
    LuaManager();
    ~LuaManager();

    bool RunScript(const std::string& filepath);

    sol::state& GetState();

    // Вызов Lua-функции из C++ с varargs
    template<typename... Args>
    auto CallFunction(const std::string& funcName, Args&&... args) -> decltype(auto);

    // Регистрация C++ класса как Lua usertype
    template<typename T, typename... Args>
    void BindClass(const std::string& name, Args&&... args);

    // Регистрация глобальной C++ функции
    template<typename Func>
    void RegisterFunction(const std::string& name, Func&& func);

    // Регистрация enum как Lua-таблицы
    template<typename Enum>
    void RegisterEnum(const std::string& name, std::initializer_list<std::pair<const char*, Enum>> values);

private:
    sol::state m_State;
};
```

### Жизненный цикл

`LuaManager m_LuaManager;` — поле `Engine`. Создаётся в конструкторе `Engine`, живёт до конца программы.

### `RunScript(filepath)`

Загружает и выполняет `.lua`-файл. Возвращает `true`, если успешно.

```cpp
m_LuaManager.RunScript("assets/scripts/game_match3.lua");
```

Путь — **относительно CWD процесса**. Sandbox копирует `assets\` в `$(OutDir)` через post-build event, так что CWD = `x64/Debug/` → путь `assets/scripts/game.lua` работает.

### `CallFunction(funcName, args...)`

Вызывает Lua-функцию. Если функции нет — логирует ошибку и возвращает broken result.

```cpp
m_LuaManager.CallFunction("OnStart");                           // без аргументов
m_LuaManager.CallFunction("OnUpdate", deltaTime);               // один float
```

### `BindClass<T>(name, args...)`

Создаёт Lua usertype для `T`. `args...` — пары (имя, метод/свойство).

```cpp
m_Lua.BindClass<Transform>("Transform",
    "SetPosition", [](Transform& t, double x, double y, double z) {
        t.SetPosition(glm::vec3((float)x, (float)y, (float)z));
    },
    "GetPosition", [](Transform& t) -> std::tuple<float, float, float> {
        auto& p = t.GetPosition();
        return { p.x, p.y, p.z };
    },
    sol::base_classes, sol::bases<Component>()
);
```

Лямбды-обёртки нужны, потому что:
- Lua не различает `float`/`double` — sol2 хочет `double`.
- `glm::vec3` нужно конвертировать в 3 числа для Lua.

### `RegisterFunction(name, func)`

Регистрирует глобальную функцию.

```cpp
m_Lua.RegisterFunction("Log", [](const std::string& msg) {
    NK_INFO("%s", msg.c_str());
});
```

### `RegisterEnum(name, values)`

```cpp
m_Lua.RegisterEnum<MouseButton>("MouseButton", {
    {"Left", MouseButton::Left},
    {"Right", MouseButton::Right},
    {"Middle", MouseButton::Middle}
});
```

В Lua: `MouseButton.Left`, `MouseButton.Right`, и т.д.

## Файлы биндингов

### `LuaClassBindings` — GameObject, Scene, Font, glm::vec2

```cpp
// GameObject
lua.BindClass<GameObject>("GameObject",
    "AddComponent_Transform", ...,
    "AddComponent_SpriteRenderer", ...,
    "AddComponent_TextRenderer", ...,
    "AddComponent_Button", ...,
    "AddComponent_Script", ...,
    "AddComponent_Anchor", ...,
    "GetTransform", ...,
    "GetSpriteRenderer", ...,
    "GetTextRenderer", ...,
    "GetButton", ...,
    "SetZOrder", &GameObject::SetZOrder,
    "GetZOrder", &GameObject::GetZOrder,
    "GetName", &GameObject::GetName,
    "OnStart", &GameObject::OnStart,
    "OnUpdate", &GameObject::OnUpdate
);

// Scene
lua.BindClass<Scene>("Scene",
    "CreateGameObject", &Scene::CreateGameObject,
    "CreateUIObject", &Scene::CreateUIObject,
    "OnStart", &Scene::OnStart,
    "OnUpdate", &Scene::OnUpdate,
    "OnRender", &Scene::OnRender,
    "GetGameCamera", &Scene::GetGameCamera,
    "GetUICamera", &Scene::GetUICamera
);

// Font
lua.BindClass<Font>("Font",
    "CreateTextTexture", &Font::CreateTextTexture
);

// glm::vec2
lua.BindClass<glm::vec2>("vec2",
    sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
    "x", &glm::vec2::x,
    "y", &glm::vec2::y
);
```

### `LuaComponentBindings` — Transform, SpriteRenderer, TextRenderer, Button, Anchor

```cpp
lua.BindClass<Transform>("Transform",
    "SetPosition", [](Transform& t, double x, double y, double z) { … },
    "GetPosition", [](Transform& t) -> std::tuple<float, float, float> { … },
    "SetRotation", &Transform::SetRotation,
    "GetRotationDegrees", &Transform::GetRotationDegrees,
    "SetScale", [](Transform& t, float x, float y) { … },
    "GetScale", [](Transform& t) -> std::tuple<float, float> { … },
    sol::base_classes, sol::bases<Component>()
);

lua.BindClass<SpriteRenderer>("SpriteRenderer",
    "SetTexture", &SpriteRenderer::SetTexture,
    "SetShader", &SpriteRenderer::SetShader,
    "SetAlignment", [](SpriteRenderer& sr, double h, double v) { … },
    "SetIsUI", &SpriteRenderer::SetIsUI,
    "SetColor", [](SpriteRenderer& sr, double r, double g, double b, double a) { … },
    "SetUseColor", &SpriteRenderer::SetUseColor,
    sol::base_classes, sol::bases<Component>()
);

// TextRenderer, Button, Anchor — аналогично
```

### `LuaCameraBindings` — OrthographicCamera

```cpp
lua.BindClass<OrthographicCamera>("OrthographicCamera",
    "GetLeft", &OrthographicCamera::GetLeft,
    "GetRight", &OrthographicCamera::GetRight,
    "GetBottom", &OrthographicCamera::GetBottom,
    "GetTop", &OrthographicCamera::GetTop,
    "ScreenToWorldPoint", &OrthographicCamera::ScreenToWorldPoint,
    "ScreenToUIPoint", &OrthographicCamera::ScreenToUIPoint
);
```

### `LuaFuncBindings` — глобальные функции

```cpp
lua.RegisterFunction("Log", [](const std::string& msg) {
    NK_INFO("%s", msg.c_str());
});
// v0.1.2: версия движка для отладки и условной логики в Lua
lua.RegisterFunction("EngineVersion", []() -> std::string {
    return "0.1.2";
});
lua.RegisterFunction("IsKeyDown", [](int key) -> bool {
    return Input::IsKeyDown(key);
});
lua.RegisterFunction("SetClearColor", [](float r, float g, float b, float a) {
    Renderer::SetClearColor(r, g, b, a);
});
lua.RegisterFunction("GetEngine", []() -> Engine& { return Engine::Get(); });
lua.RegisterFunction("GetScene", []() -> Scene& { return Engine::Get().GetScene(); });
lua.RegisterFunction("GetTexture", [](const std::string& path) {
    return Engine::Get().GetResourceManager().GetTexture(path);
});
lua.RegisterFunction("GetShader", [](const std::string& name, const std::string& vSrc, const std::string& fSrc) {
    return Engine::Get().GetResourceManager().GetShader(name, vSrc, fSrc);
});
lua.RegisterFunction("LoadFont", [](const std::string& path) -> std::shared_ptr<Font> {
    try { return std::make_shared<Font>(path); } catch (...) { return nullptr; }
});
lua.RegisterFunction("GetMousePosition", []() -> std::tuple<int, int> { … });
lua.RegisterFunction("GetWindowWidth", []() { return Engine::Get().GetWindow()->GetWidth(); });
lua.RegisterFunction("GetWindowHeight", []() { return Engine::Get().GetWindow()->GetHeight(); });
lua.RegisterFunction("IsMouseButtonDown", [](int button) -> bool { … });
lua.RegisterFunction("CreateSolidColorTexture", [](int r, int g, int b, int a) -> std::shared_ptr<Texture2D> { … });
```

### `LuaInputBindings` — KeyCode/MouseButton + GetKey/Down/Up

```cpp
lua.RegisterEnum<KeyCode>("KeyCode", {
    {"A", KeyCode::A}, {"B", KeyCode::B}, …,
    {"Space", KeyCode::Space}, {"Escape", KeyCode::Escape}, …
});

lua.RegisterEnum<MouseButton>("MouseButton", {
    {"Left", MouseButton::Left}, {"Right", MouseButton::Right}, {"Middle", MouseButton::Middle}
});

lua.RegisterFunction("GetKey", [](KeyCode k) -> bool { return InputSystem::Get().GetKey(k); });
// v0.1.2: Unity-style алиас
lua.RegisterFunction("IsKeyDown", [](KeyCode k) -> bool { return InputSystem::Get().GetKey(k); });
lua.RegisterFunction("GetKeyDown", [](KeyCode k) -> bool { return InputSystem::Get().GetKeyDown(k); });
lua.RegisterFunction("GetKeyUp", [](KeyCode k) -> bool { return InputSystem::Get().GetKeyUp(k); });
lua.RegisterFunction("GetMouseButton", [](MouseButton b) -> bool { … });
lua.RegisterFunction("GetMouseButtonDown", [](MouseButton b) -> bool { … });
lua.RegisterFunction("GetMouseButtonUp", [](MouseButton b) -> bool { … });
lua.RegisterFunction("GetMousePos", []() -> std::tuple<int, int> { … });
// TODO v0.2: GetMouseScroll()

### `LuaBindings::RegisterAll`

```cpp
void LuaBindings::RegisterAll(LuaManager& lua) {
    LuaClassBindings::RegisterAll(lua);
    LuaComponentBindings::RegisterAll(lua);
    LuaCameraBindings::RegisterAll(lua);
    LuaFuncBindings::RegisterAll(lua);
    LuaInputBindings::RegisterAll(lua);
}
```

Вызывается в `Engine::SetupLuaBindings()` (→ `Engine::Initialize`).

## Match3-специфичные биндинги

В `Match3Game::SetupLuaBindings` (в Sandbox) добавляются:

```cpp
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
    "GetCellPosition", [](Match3Board& board, int r, int c) -> std::tuple<float, float> { … },
    "GetRows", &Match3Board::GetRows,
    "GetCols", &Match3Board::GetCols,
    "OnTileChanged", &Match3Board::OnTileChanged
);
```

## Известные ограничения

- ❌ Один Lua-state на приложение. Нет поддержки нескольких state'ов.
- ❌ Нет защиты от heavy Lua-скриптов (бесконечный цикл в OnUpdate зависнет).
- ❌ `Match3Board` экспортируется напрямую из `Match3Game.cpp` — смешивает C++ и Lua.
- ❌ Нет песочницы (sandbox) — Lua имеет полный доступ к C++ через биндинги.
- ❌ `RegisterEnum` создаёт **копии** значений (lua-таблица с integer keys).

## TODO v0.2

- [ ] Coroutines в Lua (`sol::coroutine`).
- [ ] `sol::protected_function` для всех user-функций (безопасные вызовы).
- [ ] Hot reload скриптов (через `sol::load` + `sol::protected_function::set(nullptr)` + reload).
- [ ] Sandboxing: разрешить только безопасные библиотеки (`sol::lib::base`, `sol::lib::math`, `sol::lib::string`, запретить `io`, `os`).
- [ ] ECS-биндинги (см. [06_ECS_EnTT](06_ECS_EnTT.md)).

См. также: [03_Скриптинг_на_Lua/01_Lua_API_справочник](../03_Скриптинг_на_Lua/01_Lua_API_справочник.md) — полный API.
