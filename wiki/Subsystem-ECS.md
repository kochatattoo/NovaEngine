# Subsystem: ECS (EnTT)

> 🟡 **v0.2** — активно разрабатывается. Match3 уже мигрирован.

## Классы

- **`NK::ECS::World`** (`Engine/Include/ECS/World.h`) — обёртка над `entt::registry`.
  - `CreateEntity()`, `CreateEntity(name)` — создать entity.
  - `GetEntityByName(name)` — найти по строковому имени.
  - `AddComponent<T>`, `GetComponent<T>`, `HasComponent<T>`, `RemoveComponent<T>`.
  - `ForEach<Components...>(func)` — итерация.

## Компоненты (POD)

- **`TransformComponent`** — `glm::vec3 Position`, `glm::quat Rotation`, `glm::vec3 Scale`.
- **`SpriteComponent`** — `std::shared_ptr<Texture2D> Texture`, `glm::vec4 Color`.
- **`NameComponent`** — `std::string Name` (для name-based lookup).

## Системы (game-specific, в SandBox)

- **`NK::Game::ECS::Match3System`** (`SandBox/Source/Game/ECS/`) — owns grid + 100 tile-entities, дублирует логику `Match3Board`.
- **`Match3TileComponent`** — `Row, Col, Type`.

## Engine-уровневые системы (TODO v0.2.6+)

- `SpriteRenderSystem` — рендер entities с `(Transform, Sprite)`.
- `CameraSystem` — обновление ViewProjection.
- `LifetimeSystem` — удаление просроченных.

## Lua-биндинги

- **`GetBoard()`** — глобальная функция, возвращает `Match3BoardProxy` (прокси-обёртку вокруг `Match3System`).
- API 1:1 с `Match3Board` для обратной совместимости.

→ [полная документация](../docs/02_Подсистемы/06_ECS_EnTT.md)
