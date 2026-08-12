# 06 — ECS на EnTT (World, TransformComponent, SpriteComponent)

> **Статус:** 🟡 Каркас есть, в Sandbox пока не интегрирован. **Активный фронт работ.**

**Файлы:**
- `Engine/Include/ECS/World.h`, `Engine/Source/ECS/World.cpp`
- `Engine/Include/ECS/Components/Transform.h`
- `Engine/Include/ECS/Components/Sprite.h`

ECS (Entity-Component-System) — data-oriented параллель к Scene/GameObject. Пока **не используется** в Sandbox: создаётся тестовый `World`, добавляется одна entity, проверяется через `ForEach` — и на этом всё. Цель — **заменить** `Scene/GameObject` на ECS-аналог в v0.2 — v0.3.

## Зачем нужен ECS

- **Cache locality:** данные компонентов лежат плотно в массивах (EnTT использует sparse-set).
- **Производительность:** системы работают линейно по памяти, без `dynamic_cast`.
- **Декомпозиция:** данные отдельно от логики.
- **Гибкость:** легко добавлять новые компоненты.

## Класс `ECS::World`

**Файл:** `Engine/Include/ECS/World.h`

```cpp
namespace NK::ECS {

class World {
public:
    World() = default;
    ~World() = default;

    entt::entity CreateEntity();
    void DestroyEntity(entt::entity entity);

    template<typename T, typename... Args>
    T& AddComponent(entt::entity entity, Args&&... args);

    template<typename T> const T& GetComponent(entt::entity entity) const;
    template<typename T> T& GetComponent(entt::entity entity);

    template<typename T> bool HasComponent(entt::entity entity) const;
    template<typename T> void RemoveComponent(entt::entity entity);

    template<typename... Components, typename Func>
    void ForEach(Func&& func);

    entt::registry& GetRegistry();
    const entt::registry& GetRegistry() const;
};

}
```

Обёртка над `entt::registry`. Скрывает `entt::entity` (32-битный ID) за типизированными методами.

### Использование

```cpp
NK::ECS::World ecsWorld;
auto entity = ecsWorld.CreateEntity();
ecsWorld.AddComponent<NK::ECS::TransformComponent>(entity, glm::vec3(1.0f, 2.0f, 3.0f));
ecsWorld.AddComponent<NK::ECS::SpriteComponent>(entity);

auto& transform = ecsWorld.GetComponent<NK::ECS::TransformComponent>(entity);
NK_INFO("Entity %u pos = (%.1f, %.1f, %.1f)",
    static_cast<uint32_t>(entity),
    transform.Position.x, transform.Position.y, transform.Position.z);

ecsWorld.ForEach<NK::ECS::TransformComponent>([](entt::entity e, auto& t) {
    NK_INFO("Entity %u at (%f, %f, %f)",
        static_cast<uint32_t>(e), t.Position.x, t.Position.y, t.Position.z);
});
```

Это и есть текущий код в `Game.cpp::OnStart`.

## Компоненты

### `ECS::TransformComponent`

```cpp
struct TransformComponent {
    glm::vec3 Position = glm::vec3(0.0f);
    glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // identity
    glm::vec3 Scale    = glm::vec3(1.0f);
};
```

Использует `glm::quat` для поворота (в отличие от Scene/Transform, который хранит float-радианы вокруг Z).

### `ECS::SpriteComponent`

```cpp
struct SpriteComponent {
    std::shared_ptr<Texture2D> Texture;
    glm::vec4 Color = glm::vec4(1.0f);
};
```

Минимальный — пока только текстура и цвет. Нет шейдера, нет PPU, нет alignment.

## Что нужно для полноценной интеграции

Сейчас `World` — это просто обёртка над `entt::registry`. Для замены `Scene/GameObject` нужны:

1. **Системы (Systems).**
   - `TransformSystem` — вычисление world matrix.
   - `SpriteRenderSystem` — итерация по `(Transform, Sprite)`, батч + draw.
   - `TextRenderSystem` — итерация по `(Transform, Text)`.
   - `LifetimeSystem` — удаление просроченных entity.
   - `InputSystem` — обновление состояний.

2. **Сцены как `World`.**
   - `Scene` хранит `unique_ptr<World>`.
   - `CreateEntity` / `DestroyEntity` — обёртки.
   - `OnUpdate` запускает системы в правильном порядке.

3. **Связь со Scene/GameObject.**
   - Возможность постепенной миграции: в одном кадре работают и Scene, и ECS.
   - Компоненты могут быть общими (например, `Transform` ↔ `TransformComponent` через адаптер).

4. **Lua-биндинги для ECS.**
   - `world:CreateEntity()`.
   - `entity:AddComponent_Transform(...)`.
   - `world:ForEach(...)` (если возможно через sol2).

5. **Удалить/спрятать `World` из Sandbox** (сейчас тестовый код в `Game.cpp`).

## План миграции

| Этап | Что |
|---|---|
| v0.2 | Вынести `World` в публичный API. Сделать `Scene` обёрткой над `World`. Перевести `Match3Board` на ECS-сущности (вместо Scene::GameObject). |
| v0.3 | Перевести рендеринг спрайтов на `SpriteRenderSystem`. Добавить `Camera` как entity. |
| v0.4 | Перевести UI на ECS (Anchor, Button как компоненты). |
| v0.5 | Удалить `Scene/GameObject/Component` (или оставить как legacy-обёртку). |

## Известные ограничения

- ❌ Нет систем (только данные + `ForEach`).
- ❌ Нет рендеринга через ECS.
- ❌ Нет Lua-биндингов для ECS.
- ❌ Тестовый код в `Game.cpp` создаёт World "вручную" в `OnStart` — это не production-путь.

## TODO

- [ ] Добавить `Camera` как entity (компонент `CameraComponent`).
- [ ] `SpriteRenderSystem` с batching.
- [ ] Lua-биндинги `World`, `entity`, ECS-компонентов.
- [ ] Сериализация `World` (для будущей загрузки/сохранения).
- [ ] Решить: оставить Scene/GameObject как legacy-обёртку или удалить.

См. также: [05_Сцена_GameObject_компоненты](05_Сцена_GameObject_компоненты.md) — что заменяем.
