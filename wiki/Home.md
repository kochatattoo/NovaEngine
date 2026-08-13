# NovaEngine Wiki

> **Версия:** v0.2 (WIP)
> **GitHub:** [kochatattoo/NovaEngine](https://github.com/kochatattoo/NovaEngine)

Добро пожаловать в NovaEngine — учебный 2D-игровой движок на C++17, OpenGL 3.3 и Lua, с ECS на EnTT.

## 🚀 Быстрый старт

- [Getting Started](Getting-Started.md) — как собрать и запустить
- [Building from source](Building-from-source.md) — детали сборки

## 🏗️ Архитектура

- [Architecture Overview](Architecture.md) — общая картина: слои, модули, главный цикл
- [Subsystem: Core](Subsystem-Core.md) — Engine, Application, EntryPoint, Timer, Log
- [Subsystem: Renderer](Subsystem-Renderer.md) — OpenGL, SpriteRenderer, TextRenderer, Shader, Camera
- [Subsystem: Scene](Subsystem-Scene.md) — GameObject, Component, Transform (legacy)
- [Subsystem: ECS](Subsystem-ECS.md) — EnTT, World, Components, Systems (v0.2+)
- [Subsystem: Input](Subsystem-Input.md) — InputSystem, KeyCodes, клиентские/экранные координаты
- [Subsystem: UI](Subsystem-UI.md) — Button, Anchor
- [Subsystem: Lua](Subsystem-Lua.md) — sol2, биндинги, регистрация

## 📜 Скриптинг

- [Scripting API](Scripting-API.md) — Lua API: функции, классы, глобалы

## 🎮 Демо-игра

- [Match3 Game](Match3-Game.md) — как устроена игра-«canary»

## 🤝 Contributing

- [Coding Conventions](Coding-Conventions.md) — нейминг, стиль, layout
- [Roadmap](Roadmap.md) — что планируется, как помочь

---

> **См. также:** [полная системная документация в `docs/`](../docs/README.md) — гораздо подробнее.
