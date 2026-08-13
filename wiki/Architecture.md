# Architecture Overview

> 🚧 **TODO:** страница-заготовка.

## Слои

```
┌─────────────────────────────────┐
│  SandBox (игра Match3)          │  ← Lua-скрипты + C++-игра
├─────────────────────────────────┤
│  Engine.lib                     │  ← все подсистемы
│  ├─ Core        (Engine, App)   │
│  ├─ Window      (WinAPI + WGL)  │
│  ├─ Renderer    (OpenGL 3.3)    │
│  ├─ Scene       (GameObject)    │  ← legacy, мигрирует на ECS
│  ├─ ECS         (EnTT)          │  ← v0.2+
│  ├─ Input       (InputSystem)   │
│  ├─ UI          (Button/Anchor) │
│  ├─ Resource    (cache)         │
│  ├─ Lua         (sol2)          │
│  └─ Event       (bus)           │
├─────────────────────────────────┤
│  External: lua, sol, EnTT, glm  │
└─────────────────────────────────┘
```

## Главный цикл

```
main()
  → Engine::Run()
    → Initialize()          // Window, GL, Lua, Scene
    → m_App->OnStart()      // пользовательский код (SandboxApp::OnStart)
    → loop:
      ├─ poll events        // Win32 message loop
      ├─ m_App->OnUpdate(dt)
      ├─ Scene::OnUpdate(dt)
      ├─ Scene::OnRender()  // рисуем спрайты/текст
      ├─ SwapBuffers
    → Shutdown()
```

## Ключевые принципы

- **Engine — статическая библиотека.** SandBox линкуется с ней.
- **Scene vs ECS.** Scene — классическая иерархия (GameObject → Components). ECS — data-oriented (World → Entities → Components → Systems). В v0.2 ECS начинает заменять Scene.
- **Lua как основной язык для игровой логики.** Движок экспортирует API, игра пишется в `assets/scripts/*.lua`.
- **PCH не используется** (хоть и зарезервировано). Include-what-you-use.

→ [Subsystem: Core](Subsystem-Core.md) — детали.
