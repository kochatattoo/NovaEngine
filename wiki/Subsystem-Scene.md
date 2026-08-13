# Subsystem: Scene (GameObject, Component — legacy)

> 🚧 **TODO:** страница-заготовка. **Legacy** — мигрирует на ECS в v0.2+.

## Классы

- **`Scene`** — контейнер `m_Objects` (game world) + `m_UIObjects` (UI).
- **`GameObject`** — узел сцены с `Transform` + набором `Component`-ов.
- **`Component`** — базовый класс; конкретные: `Transform`, `SpriteRenderer`, `TextRenderer`, `ScriptComponent`, `Button`, `Anchor`.
- **`ScriptComponent`** — запускает Lua-скрипт с `OnStart`/`OnUpdate`.

## Жизненный цикл

```
Scene::OnStart()       // OnStart на всех GameObject
Scene::OnUpdate(dt)    // OnUpdate на всех GameObject
Scene::OnRender()      // draw game objects (m_GameCamera) + UI (m_UICamera)
```

## Удаление

Планируется удалить в **v0.5**, когда ECS полностью заменит Scene/GameObject.

→ [полная документация](../docs/02_Подсистемы/05_Сцена_GameObject_компоненты.md)
