# Subsystem: Core (Engine, Application, EntryPoint, Timer, Log)

> 🚧 **TODO:** страница-заготовка.

## Классы

| Класс | Файл | Назначение |
|---|---|---|
| `Engine` | `Engine/Engine.cpp` | Singleton, владеет Window, Scene, LuaManager |
| `Application` | `Engine/Include/Core/Application.h` | Базовый класс для пользовательского приложения |
| `EntryPoint` | `Engine/Include/Core/EntryPoint.h` | `main()` обёртка |
| `Log / Logger` | `Engine/Include/Core/Log.h` | printf-style логгер с уровнями |
| `Timer` | `Engine/Include/Core/Timer.h` | `deltaTime` на кадр |

## Жизненный цикл

```cpp
class SandboxApp : public Application {
    void OnStart() override { ... }       // один раз
    void OnUpdate(float dt) override { ... }  // каждый кадр
    void OnShutdown() override { ... }    // один раз при выходе
};

Application* CreateApplication() {       // EntryPoint ищет эту функцию
    return new SandboxApp();
}
```

→ [полная документация](../docs/02_Подсистемы/01_Core_Engine_Application_EntryPoint.md)
