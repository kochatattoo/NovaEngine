# 06 — ECS на EnTT (World, компоненты, системы)

> **Статус:** 🟡 **v0.2** — World API расширен (name-based lookup), Match3 мигрирован на ECS. **Активный фронт работ.**

**Файлы:**
- `Engine/Include/ECS/World.h`, `Engine/Source/ECS/World.cpp`
- `Engine/Include/ECS/Components/Transform.h`
- `Engine/Include/ECS/Components/Sprite.h`
- `Engine/Include/ECS/Components/NameComponent.h`
- `SandBox/Source/Game/ECS/Match3TileComponent.h`
- `SandBox/Source/Game/ECS/Match3System.h/.cpp`

ECS (Entity-Component-System) — data-oriented параллель к Scene/GameObject. В v0.2 Match3 полностью переведён на ECS-логику (Match3System + 100 tile-entities), рендер пока остаётся в Scene/GameObject — это **миграция логики, не рендера**.

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
    // Создание / удаление
    entt::entity CreateEntity();
    entt::entity CreateEntity(const std::string& name);  // v0.2: с именем
    void DestroyEntity(entt::entity entity);
    void Clear();                                         // v0.2: всё стереть

    // Компоненты
    template<typename T, typename... Args>
    T& AddComponent(entt::entity entity, Args&&... args);

    template<typename T> const T& GetComponent(entt::entity entity) const;
    template<typename T> T& GetComponent(entt::entity entity);
    template<typename T> bool HasComponent(entt::entity entity) const;
    template<typename T> void RemoveComponent(entt::entity entity);

    // Итерация
    template<typename... Components, typename Func>
    void ForEach(Func&& func);

    // Прямой доступ к registry
    entt::registry& GetRegistry();
    const entt::registry& GetRegistry() const;

    // === Именованные entities (v0.2) ===
    entt::entity GetEntityByName(const std::string& name) const; // entt::null если нет
    void RenameEntity(entt::entity entity, const std::string& newName);
    std::string GetEntityName(entt::entity entity) const;
};

}
```

Обёртка над `entt::registry`. Скрывает `entt::entity` (32-битный ID) за типизированными методами.

### Именованные entities (v0.2)

Хранит `std::unordered_map<std::string, entt::entity>`. Позволяет искать entity по строковому имени — полезно для:
- Lua-скриптов: `world:GetEntityByName("player_main")` без сохранения raw `entt::entity`.
- Редактора: показать entity в иерархии.
- Систем, которые ищут конкретные entity (например, `Match3System` ищет плитку `"tile_3_5"`).

Использует компонент `NameComponent` (см. ниже) — если у entity нет этого компонента, имя будет `""`.

## Компоненты (v0.2)

### `ECS::NameComponent` (новое)

```cpp
struct NameComponent {
    std::string Name;
};
```

Просто строка-имя. Автоматически добавляется через `World::CreateEntity(name)` и `RenameEntity`.

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

### `Game::ECS::Match3TileComponent` (SandBox-specific)

```cpp
namespace NK::Game::ECS {
    struct Match3TileComponent {
        int Row;     // 0..m_Rows-1
        int Col;     // 0..m_Cols-1
        int Type;    // -1 = пусто, 0..5 = цвет
        static constexpr const char* Prefix = "tile_";
    };
}
```

Game-specific компонент — намеренно НЕ в Engine, остаётся в SandBox. По конвенции `tile_<row>_<col>` используется для имен entity плиток.

## Системы (v0.2)

### `Game::ECS::Match3System`

**Файл:** `SandBox/Source/Game/ECS/Match3System.h/.cpp`

Owns:
- Сетку `std::vector<std::vector<int>>` (те же данные, что были в `Match3Board`).
- Ссылку на `NK::ECS::World&`.
- 100 tile-entities (по одной на клетку).

Спавнит entities в `Start()`: каждая плитка получает компоненты:
- `NameComponent` (`"tile_<row>_<col>"`)
- `TransformComponent` (позиция = центр клетки)
- `SpriteComponent` (Color по типу, Texture = nullptr — рендер будет в v0.2.6)
- `Match3TileComponent` (Row, Col, Type)

Публичный API — точная копия `Match3Board` (для обратной совместимости с Lua):
- `FillRandom`, `GetTile`, `SetTile`, `IsValidCell`
- `Swap`, `FindMatches`, `RemoveTiles`, `ApplyGravity`, `FillEmpty`, `HasPossibleMoves`, `Mix`
- `GetCellPosition`, `GetRows`, `GetCols`, `GetCellSize`, `GetPixelsPerUnit`
- `OnTileChanged` callback

При каждом изменении плитки (`SetTile`, `Swap`, `RemoveTiles`, `ApplyGravity`, `FillEmpty`) — обновляется grid + ECS-entity (`Match3TileComponent::Type`, `SpriteComponent::Color`) + дёргается `OnTileChanged` callback.

## Миграция Match3 (v0.2)

**Было (v0.1):**
```cpp
class Match3Game {
    std::unique_ptr<Match3Board> m_Board;  // POD-логика
    Scene* m_Scene;                         // GameObject'ы
    LuaManager& m_Lua;
};

// Lua: board = Match3Board.new(ROWS, COLS, CELL_SIZE, PPU)
```

**Стало (v0.2):**
```cpp
class Match3Game {
    std::unique_ptr<NK::ECS::World> m_World;
    std::unique_ptr<NK::Game::ECS::Match3System> m_System;  // owns grid + 100 entities
    LuaManager& m_Lua;
};

// Lua: board = GetBoard()  -- возвращает указатель на m_System
```

Внутри `Match3System` живёт grid + 100 ECS-entities. Визуально плитки НЕ рисуются через ECS (пока) — Lua-скрипт продолжает создавать Scene/GameObject'ы в `OnTileChanged` callback. Это позволяет поэтапно: сначала логика на ECS, потом рендер.

### Lua API (не изменился)

`Match3Game::SetupLuaBindings()` регистрирует `Match3System` под именем **`Match3Board`** (для обратной совместимости) + глобальную функцию `GetBoard()`:

```cpp
m_Lua.BindClass<NK::Game::ECS::Match3System>("Match3Board",
    sol::no_constructor,
    "FillRandom", &Match3System::FillRandom,
    "Swap",       &Match3System::Swap,
    // ...
);

m_Lua.RegisterFunction("GetBoard", [this]() { return m_System.get(); });
```

Lua-скрипт:
```lua
board = GetBoard()  -- вместо Match3Board.new(...)
board:FillRandom()
```

Всё остальное (`board:Swap(...)`, `board:FindMatches()` и т.д.) работает без изменений.

## Что нужно для полноценной интеграции

Сейчас ECS хранит данные, но рендер идёт через Scene/GameObject. Для полной миграции:

1. **Системы (Systems).**
   - ✅ `Match3System` — игровая логика (v0.2).
   - ✅ `SpriteRenderSystem` — итерация по `(Transform, Sprite)`, draw через OpenGL (v0.2.6).
   - ⏳ `TextRenderSystem` — для UI-текста.
   - ⏳ `LifetimeSystem` — удаление просроченных entity.

2. **Рендер из ECS.**
   - Сцена создаёт `World` + `Camera` (entity с `CameraComponent`).
   - `SpriteRenderSystem` читает `(Transform, Sprite)` и рисует через `SpriteRenderer` (один на камеру, не на entity).
   - Удалить `Scene::OnRender` (или оставить как legacy для UI).

3. **Lua-биндинги для ECS.**
   - ✅ `GetBoard()` — глобальная функция возвращает `Match3System*` (v0.2).
   - ⏳ `world:CreateEntity(name)`, `entity:AddComponent_Transform()` — будут в v0.2.6.
   - ⏳ `world:ForEach(...)` (если возможно через sol2).

4. **Удалить `Match3Board`** (POD-класс) — `Match3System` полностью его заменяет. Оставлен пока для reference.

## План миграции

| Этап | Что | Статус |
|---|---|---|
| v0.2.1 | World API: CreateEntity(name), GetEntityByName, RenameEntity, Clear, NameComponent | ✅ |
| v0.2.2 | Match3TileComponent (POD) | ✅ |
| v0.2.3 | Match3System — owns grid + 100 ECS-entities | ✅ |
| v0.2.4 | Match3Game owns World + System (вместо Match3Board) | ✅ |
| v0.2.5 | Lua bindings: Match3System под именем Match3Board, GetBoard() | ✅ |
| v0.2.6 | SpriteRenderSystem (рендер из ECS) | ✅ |
| v0.2.7 | Удалить Match3Board (POD) и Scene::CreateGameObject для плиток | ✅ |
| v0.3   | UI на ECS (Anchor, Button как компоненты) | ⏳ |
| v0.4   | Сериализация World | ⏳ |
| v0.5   | Удалить Scene/GameObject/Component (legacy) | ⏳ |

## Известные ограничения

- ❌ Нет рендеринга через ECS (плитки в ECS существуют, но не рисуются).
- ❌ Lua-биндинги для `World`/`entity` отсутствуют (только `GetBoard()`).
- ⚠️ `Match3Board` (POD) оставлен в коде — будет удалён в v0.2.7.
- ⚠️ Lua-скрипт по-прежнему создаёт Scene::GameObject'ы для визуализации (двойное хранение).

## TODO

- [x] NameComponent (v0.2.1)
- [x] World API: GetEntityByName, RenameEntity, Clear (v0.2.1)
- [x] Match3TileComponent (v0.2.2)
- [x] Match3System (v0.2.3)
- [x] Match3Game → World + System (v0.2.4)
- [x] Lua bindings через GetBoard() (v0.2.5)
- [x] SpriteRenderSystem (v0.2.6) — рендер entities напрямую, без GameObject
- [x] Удалить Match3Board (v0.2.7) — Match3System полностью заменил
- [ ] Lua: world:CreateEntity(name) (v0.2.8)
- [ ] Camera как entity (v0.3)

См. также: [05_Сцена_GameObject_компоненты](05_Сцена_GameObject_компоненты.md) — что заменяем.
