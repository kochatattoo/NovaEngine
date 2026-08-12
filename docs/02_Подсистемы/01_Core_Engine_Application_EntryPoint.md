# 01 — Core: Engine, Application, EntryPoint, Timer

> **Статус:** 🟢 Стабильно

Подсистема **Core** — скелет движка: запуск, главный цикл, контракт с игрой, лог, таймер. Не зависит от OpenGL, Lua, сцены.

## Класс `NK::Engine`

**Файлы:** `Engine/Include/Core/Engine.h`, `Engine/Engine.cpp`

Singleton. Создаётся в `main()`, живёт до конца программы.

### Поля

| Поле | Тип | Назначение |
|---|---|---|
| `s_Instance` | `static Engine*` | Singleton-указатель |
| `m_Window` | `std::unique_ptr<Window>` | Владеет окном |
| `m_App` | `Application*` | Не владеет (удаляется в `main`) |
| `m_Config` | `EngineConfig` | Title, Width, Height |
| `m_Running` | `bool` | Флаг главного цикла |
| `m_ResourceManager` | `ResourceManager` | Кэш текстур/шейдеров |
| `m_LuaManager` | `LuaManager` | sol::state + биндинги |
| `m_Scene` | `Scene` | Контейнер GameObject + камеры |

### Публичные методы

```cpp
Engine(const EngineConfig& config);
~Engine();

void Run(Application* app);    // главный цикл
void Shutdown();              // выставить m_Running=false

class Window* GetWindow();
static Engine& Get();         // singleton-доступ
ResourceManager& GetResourceManager();
LuaManager& GetLuaManager();
Scene& GetScene();
```

### `Run` — главный цикл

1. `Initialize()` — окно, контекст, Renderer, Lua-биндинги, default shader.
2. `m_Scene.OnStart()` — сцена инициализирует камеры по размерам окна, OnStart всех GameObject.
3. `m_App->OnStart()` — игра стартует.
4. Цикл пока `m_Running`:
   - `m_Window->OnUpdate()` — PeekMessage
   - `InputSystem::Get().Update()` — разобрать очередь
   - `Renderer::BeginFrame()` — очистить
   - `m_Scene.OnUpdate(dt)` + `m_App->OnUpdate(dt)`
   - `Renderer::EndFrame()` — пусто
   - `ctx->SwapBuffers()` — swap
5. `m_App->OnShutdown()`, `Shutdown()`.

## Класс `NK::Application`

**Файл:** `Engine/Include/Core/Application.h`

Базовый класс для игры. Аналог `MonoBehaviour` в Unity.

```cpp
class Application {
public:
    Application() = default;
    virtual ~Application() = default;

    // Вызывается один раз при старте
    virtual void OnStart() {}
    // Вызывается каждый кадр, deltaTime в секундах
    virtual void OnUpdate(float deltaTime) {}
    // Вызывается перед завершением
    virtual void OnShutdown() {}
};

extern Application* CreateApplication();
```

### v0.1.1: `OnEvent` удалён

В v0.1 метод `virtual void OnEvent(Event& event)` **удалён** из базового класса. Причины:

- В `Engine::Run` есть `m_App->OnUpdate(dt)`, но **не было** рассылки событий приложению — метод был «мёртвым».
- Сейчас события обрабатываются через `InputSystem::Get().GetKeyDown(...)` / `GetMouseButtonDown(...)` и т.п.

Прямой проброс событий в `Application` запланирован на **v0.2** (см. [06_План_разработки/03_Среднесрочный_план.md](../06_План_разработки/03_Среднесрочный_план.md)).

Если нужна обработка ввода — подпишитесь в `OnStart` и опрашивайте `InputSystem::Get()` каждый кадр.

## Скрытый `main` — `EntryPoint.h`

**Файл:** `Engine/Include/Core/EntryPoint.h`

Содержит `int main(...)`, **только** при `#ifdef _WIN32`. Подключается в **одном** `.cpp` (в `Sandbox/Source/Game.cpp`).

```cpp
#ifdef _WIN32
extern NK::Application* NK::CreateApplication();

int main(int argc, char** argv) {
    NK::EngineConfig config;
    config.Title  = "NovaEngine Sandbox";
    config.Width  = 1280;
    config.Height = 720;

    NK::Engine engine(config);
    auto app = NK::CreateApplication();
    engine.Run(app);
    delete app;
    return 0;
}
#endif
```

> **Важно:** если определить свою `main` И подключить `EntryPoint.h` — будет конфликт.

## Класс `NK::Timer`

**Файл:** `Engine/Include/Core/Timer.h`

```cpp
class Timer {
public:
    Timer();
    float Tick();   // секунды с прошлого Tick
};
```

## Конфигурация `EngineConfig`

```cpp
struct EngineConfig {
    std::string Title  = "NovaEngine";
    uint32_t    Width  = 1280;
    uint32_t    Height = 720;
};
```

См. также: [02_Логирование](02_Логирование.md), [01_Архитектура/02_Жизненный_цикл](../01_Архитектура/02_Жизненный_цикл.md).
