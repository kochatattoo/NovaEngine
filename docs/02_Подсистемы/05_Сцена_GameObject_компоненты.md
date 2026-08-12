# 05 — Сцена, GameObject и компоненты

> **Статус:** 🟢 Стабильно, основная модель объектов

**Файлы:**
- `Engine/Include/Scene/Scene.h`, `Engine/Source/Scene/Scene.cpp`
- `Engine/Include/Scene/GameObject.h`, `Engine/Source/Scene/GameObject.cpp`
- `Engine/Include/Scene/Component.h`
- `Engine/Include/Scene/Transform.h`, `Engine/Source/Scene/Transform.cpp`
- `Engine/Include/Scene/ScriptComponent.h`, `Engine/Source/Scene/ScriptComponent.cpp`

Классическая Unity-подобная иерархия. На данный момент — **основной** способ организации игровых объектов (ECS пока только в каркасе, см. [06_ECS_EnTT](06_ECS_EnTT.md)).

## Класс `Scene`

**Файл:** `Engine/Include/Scene/Scene.h`

```cpp
class Scene {
public:
    GameObject* CreateGameObject(const std::string& name = "GameObject");
    GameObject* CreateUIObject(const std::string& name = "UIObject");
    void AddUIObject(GameObject* obj);     // ⚠️ недоделан

    void OnRender();
    void OnStart();
    void OnUpdate(float deltaTime);

    OrthographicCamera& GetGameCamera();
    OrthographicCamera& GetUICamera();

    void RegisterAnchor(Anchor* anchor);
    void RecalculateAnchors(uint32_t width, uint32_t height);

    const std::vector<std::unique_ptr<GameObject>>& GetObjects() const;
    const std::vector<std::unique_ptr<GameObject>>& GetUIObjects() const;
};
```

### Два списка и две камеры

```cpp
std::vector<std::unique_ptr<GameObject>> m_Objects;    // игровой мир
std::vector<std::unique_ptr<GameObject>> m_UIObjects;  // UI (отдельный список)
OrthographicCamera m_GameCamera{ -5.0f, 5.0f, -5.0f, 5.0f };
OrthographicCamera m_UICamera {  0.0f, 1280.0f, 720.0f, 0.0f };
```

Разделение нужно потому, что UI рисуется **в экранных координатах** (top-left origin, Y вниз) с включённым блендингом и без depth test.

### `OnStart`

Вызывается из `Engine::Run` **до** `m_App->OnStart()`. Делает:

1. Получает размеры окна.
2. `m_GameCamera.OnWindowResized(w, h)` — пересчёт проекции с сохранением aspect ratio.
3. `m_UICamera.SetProjection(0, w, h, 0)`.
4. `OnStart()` у всех объектов обоих списков.
5. `m_Started = true`.

### `OnUpdate(dt)`

Просто вызывает `OnUpdate(dt)` у всех объектов обоих списков. Никакой логики.

### `OnRender`

```cpp
void Scene::OnRender() {
    // Игровой мир
    const glm::mat4& gameViewProj = m_GameCamera.GetViewProjectionMatrix();
    for (auto& obj : m_Objects) {
        auto* sr = obj->GetComponent<SpriteRenderer>();
        if (sr) sr->Render(gameViewProj);
    }

    // UI (сортировка по ZOrder)
    const glm::mat4& uiViewProj = m_UICamera.GetViewProjectionMatrix();
    std::sort(m_UIObjects.begin(), m_UIObjects.end(),
        [](const std::unique_ptr<GameObject>& a, const std::unique_ptr<GameObject>& b) {
            return a->GetZOrder() < b->GetZOrder();
        });

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);

    for (auto& obj : m_UIObjects) {
        auto* sr = obj->GetComponent<SpriteRenderer>();
        if (sr) sr->Render(uiViewProj);
        auto* tr = obj->GetComponent<TextRenderer>();
        if (tr) tr->Render(uiViewProj);
    }

    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
```

### `RegisterAnchor` / `RecalculateAnchors`

UI-объекты с `Anchor` компонентом вызывают `RegisterAnchor(this)` в `OnStart`. При ресайзе окна `RecalculateAnchors` идёт по всем якорям и пересчитывает позицию (см. [08_UI](08_UI.md)).

## Класс `GameObject`

**Файл:** `Engine/Include/Scene/GameObject.h`

```cpp
class GameObject {
public:
    GameObject(const std::string& name = "GameObject");
    ~GameObject() = default;

    const std::string& GetName() const;

    template<typename T, typename... Args>
    T* AddComponent(Args&&... args);

    template<typename T>
    T* GetComponent();

    void OnStart();
    void OnUpdate(float deltaTime);

    bool IsStarted() const;
    void SetZOrder(int z);
    int GetZOrder() const;
};
```

### Конструктор

```cpp
GameObject::GameObject(const std::string& name) : m_Name(name) {
    m_Components.push_back(std::make_unique<Transform>(this));
    NK_CORE_TRACE("GameObject '%s' created with default Transform", name.c_str());
}
```

Каждый GameObject **автоматически** получает `Transform` (см. ниже).

### `AddComponent<T>(args...)`

```cpp
template<typename T, typename... Args>
T* AddComponent(Args&&... args) {
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
    auto component = std::make_unique<T>(this, std::forward<Args>(args)...);
    T* rawPtr = component.get();
    m_Components.push_back(std::move(component));
    if (m_Started) rawPtr->OnStart();
    return rawPtr;
}
```

Создаёт компонент, передавая `this` как владельца. Если GameObject уже `Started` — сразу вызывает `OnStart` нового компонента.

### `GetComponent<T>()`

```cpp
template<typename T>
T* GetComponent() {
    for (auto& c : m_Components) {
        T* casted = dynamic_cast<T*>(c.get());
        if (casted) return casted;
    }
    return nullptr;
}
```

`dynamic_cast` — линейный поиск. На малом числе компонентов (1–10) приемлемо.

> ⚠️ TODO v0.2: заменить на map<type_index, Component*> для O(1).

### `OnStart` / `OnUpdate`

Итерируют `m_Components` и вызывают соответствующий метод.

## Класс `Component` (базовый)

**Файл:** `Engine/Include/Scene/Component.h`

```cpp
class Component {
public:
    Component(GameObject* owner) : m_Owner(owner) {}
    virtual ~Component() = default;

    virtual void OnStart() {}
    virtual void OnUpdate(float deltaTime) {}

    GameObject& GetOwner() const { return *m_Owner; }

protected:
    GameObject* m_Owner;
};
```

## Класс `Transform`

**Файл:** `Engine/Include/Scene/Transform.h`

```cpp
class Transform : public Component {
public:
    Transform(GameObject* owner);

    void SetPosition(const glm::vec3& pos);
    const glm::vec3& GetPosition() const;
    void SetRotation(float angleDegrees);
    float GetRotationDegrees() const;
    void SetScale(const glm::vec2& scale);
    const glm::vec2& GetScale() const;

    glm::mat4 GetModelMatrix() const;
};
```

`GetModelMatrix` = `translate(position) * rotate(rotation) * scale(scale)`.

## Класс `ScriptComponent`

**Файл:** `Engine/Include/Scene/ScriptComponent.h`

```cpp
class ScriptComponent : public Component {
public:
    ScriptComponent(GameObject* owner, const std::string& scriptPath);

    void OnStart() override;
    void OnUpdate(float deltaTime) override;
};
```

⚠️ **ScriptComponent сейчас не используется в Sandbox.** Вместо этого Lua-логика пишется в `game_match3.lua` и вызывается из `Match3Game::Update` через `LuaManager::CallFunction("OnUpdate", dt)`. `ScriptComponent` существует как API для будущего использования (например, разные скрипты на разных GameObject).

## Полная диаграмма

```
Scene
├── m_Objects (vector<unique_ptr<GameObject>>)
│   ├── GameObject "Tile_0_0"
│   │   ├── Transform
│   │   └── SpriteRenderer
│   ├── GameObject "Player"
│   │   ├── Transform
│   │   └── SpriteRenderer
│   └── ...
├── m_UIObjects (vector<unique_ptr<GameObject>>)
│   ├── GameObject "WelcomeText"
│   │   ├── Transform
│   │   ├── Anchor
│   │   └── TextRenderer
│   └── GameObject "TestButton"
│       ├── Transform
│       ├── Anchor
│       ├── SpriteRenderer
│       ├── TextRenderer
│       └── Button
├── m_GameCamera (OrthographicCamera, world)
├── m_UICamera  (OrthographicCamera, screen-space)
└── m_Anchors (vector<Anchor*>)
```

## Известные ограничения

- ❌ Нет иерархии Transform (parent-child). Объекты — плоский список.
- ❌ Нет сериализации / десериализации (сохранение/загрузка сцены).
- ❌ Нет динамического удаления компонентов.
- ❌ `AddUIObject` недоделан (нужно либо удалить, либо реализовать).
- ❌ `GetComponent` — линейный поиск.

## TODO v0.2

- [ ] `RemoveComponent<T>()`.
- [ ] Иерархия Transform (parent-child).
- [ ] `Scene` как сериализуемая структура.
- [ ] Scene Manager (несколько сцен, переходы).

См. также: [06_ECS_EnTT](06_ECS_EnTT.md) — параллельная модель данных (миграция).
