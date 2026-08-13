# NovaEngine — Code Style Guide

> **Версия:** v0.3.1
> **Назначение:** единые правила написания кода для всего проекта. Философия, конвенции, антипаттерны.
> **Где читать:** этот файл — при работе с **любым** C++/C# кодом проекта.
> **Корневой контекст:** [AGENTS.md](AGENTS.md) — общие правила (структура, команды, build).

---

## 1. Философия

### 1.1 Принципы

| Принцип | Коротко | В NovaEngine |
|---|---|---|
| **SOLID** | Single responsibility, Open-closed, Liskov, Interface segregation, Dependency inversion | Каждый класс — одна ответственность. Зависимости через интерфейсы или DI (если возможно). |
| **DRY** | Don't Repeat Yourself | Нет копипасты. Общая логика — в helper или систему. |
| **KISS** | Keep It Simple, Stupid | Простое решение лучше умного. Если можно сделать 5 строк вместо 50 — делай 5. |
| **YAGNI** | You Aren't Gonna Need It | Не пиши код "на будущее". Если не нужно сейчас — не делай. |
| **Boy Scout Rule** | Оставь код чище, чем нашёл | Каждый коммит должен немного улучшать существующий код. |
| **Fail Fast** | Падай рано и громко | Assertions в Debug, чёткие ошибки. Не молчи о проблемах. |

### 1.2 Приоритеты (когда они конфликтуют)

```
1. Корректность        (сначала — работает)
2. Простота            (KISS)
3. Читаемость          (для людей)
4. Согласованность     (со стилем проекта)
5. Производительность  (когда нужно)
6. Элегантность        (последнее)
```

> ⚠️ Производительность **не** оптимизируется преждевременно. Сначала — измерить, потом — оптимизировать.

---

## 2. Нейминг

### 2.1 Общее

| Сущность | Стиль | Пример |
|---|---|---|
| **Класс, struct, enum, метод, свойство, публичное поле** | `PascalCase` | `Engine`, `Match3System`, `IsKeyDown`, `MaxSpeed` |
| **Приватное поле** | `m_camelCase` (с `m_` префиксом) | `m_Running`, `m_MousePosition` |
| **Protected поле** | `m_camelCase` | `m_Count` |
| **Static поле** | `s_PascalCase` (с `s_` префиксом) | `s_QuadVAO`, `s_Initialized` |
| **Static const / constexpr** | `kPascalCase` или `s_PascalCase` | `kMaxSize` или `s_ClearColor` |
| **Const переменная** | `kPascalCase` или `camelCase` (для локальных) | `const float kPi = 3.14f`; `const int maxCount = 10;` |
| **Локальная переменная, параметр** | `camelCase` | `targetPosition`, `deltaTime` |
| **Enum-значение** | `PascalCase` (без префикса enum-класса) | `enum class LogLevel { Trace, Info, Warn, Error };` |
| **Namespace** | `NK` (от NovaKernel), подмодули `NK::ECS` | `namespace NK::ECS { ... }` |
| **Макрос** | `NK_*` для core, `NK_CORE_*` для core-логгера | `NK_INFO`, `NK_CORE_INFO` |
| **Файл** | соответствует классу | `Engine.h` → `class Engine` |
| **Header guard** | `#pragma once` (не `#ifndef`) | `#pragma once` |

### 2.2 Глаголы для методов

| Тип | Глаголы |
|---|---|
| **Геттер** | `GetX()`, `IsX()`, `HasX()`, `CanX()` |
| **Сеттер** | `SetX(value)` |
| **Действие** | `Start()`, `Stop()`, `Update(dt)`, `Render()` |
| **Создание** | `CreateX()`, `MakeX()`, `BuildX()` |
| **Удаление** | `DestroyX()`, `RemoveX()` |
| **Проверка** | `IsValid()`, `Contains()`, `Matches()` |

### 2.3 Антипаттерны нейминга

❌ **`Hungarian notation`** (без префикса типа): `strName`, `iCount`, `pPointer` — запрещено.
❌ **`CamelCase для классов`**: `match3Game` — запрещено.
❌ **`snake_case`**: `match3_game` — запрещено.
❌ **Слишком общие имена**: `Manager`, `Helper`, `Util`, `Data` — давай конкретное имя.
❌ **Сокращения**: `Mgr`, `Cfg`, `Btn` — только общеизвестные (`id`, `url`, `ui`).

---

## 3. Файлы и директории

### 3.1 Структура файла

```
// 1. License/copyright header (если есть)
// 2. #pragma once
// 3. Includes (в порядке: paired, std, third-party, project)
// 4. Forward declarations
// 5. Namespace opening
// 6. Constants / enums
// 7. Classes / structs (по одному)
// 8. Free functions
// 9. Namespace closing
```

### 3.2 Порядок include'ов

```cpp
// 1. Paired header
#include "Renderer/Texture2D.h"

// 2. Стандартная библиотека
#include <memory>
#include <string>
#include <vector>

// 3. Third-party
#include <glm/glm.hpp>
#include <sol/sol.hpp>

// 4. Engine (отсортированы по глубине)
#include "Core/Log.h"
#include "ECS/World.h"
```

### 3.3 Размер файла

- **Максимум ~300 строк** на файл. Если больше — разделить.
- **Один класс на файл** (header + .cpp).
- **Header-only только для шаблонов** или маленьких inline-функций.

---

## 4. Классы и struct'ы

### 4.1 Порядок секций (модификаторы доступа)

```cpp
class MyClass : public Base {
public:
    // 1. Публичные типы (typedef, enum, nested class)
    // 2. Конструкторы / деструктор
    // 3. Операторы (если есть)
    // 4. Публичные методы (группировать по функциональности)
    // 5. Публичные свойства (если есть)

protected:
    // 6. Protected методы (для наследников)
    // 7. Protected свойства

private:
    // 8. Private методы (хелперы)
    // 9. Private поля (ВСЕ поля — внизу, в private)
};
```

### 4.2 Порядок методов внутри секции

1. **Конструкторы / деструктор** — вверху.
2. **Главное API** (то, для чего класс создан).
3. **Геттеры/сеттеры** свойств.
4. **Хелперы** — внизу.
5. **Перегрузки операторов** — в самом низу или отдельно.

### 4.3 Пример правильного класса

```cpp
// File: MyClass.h
#pragma once
#include <string>
#include <memory>

class MyClass : public Base {
public:
    // Типы
    using Ptr = std::shared_ptr<MyClass>;

    // Конструкторы
    explicit MyClass(int initialValue);
    ~MyClass() override;

    // Запрет копирования (если нужно)
    MyClass(const MyClass&) = delete;
    MyClass& operator=(const MyClass&) = delete;

    // Главное API
    void Start();
    void Update(float deltaTime);

    // Геттеры
    int GetValue() const { return m_Value; }
    bool IsActive() const { return m_Active; }

    // Сеттеры
    void SetValue(int v) { m_Value = v; }
    void SetActive(bool active) { m_Active = active; }

private:
    // Хелперы
    void Recalculate();
    void LogState() const;

    // Поля
    int m_Value = 0;
    bool m_Active = false;
    std::string m_Name;
};
```

### 4.4 Struct vs Class

- **`struct`** — POD-данные (только публичные поля, без методов с логикой). ECS-компоненты.
- **`class`** — всё остальное (инкапсуляция, методы, наследование).

```cpp
// ✅ Хорошо: struct как POD
struct TransformComponent {
    glm::vec3 Position = glm::vec3(0.0f);
    glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 Scale = glm::vec3(1.0f);
};

// ❌ Плохо: struct с методами
struct Vector2 {
    float x, y;
    float Length() const { return sqrt(x * x + y * y); }  // Должен быть class
};
```

---

## 5. Функции

### 5.1 Размер

- **Максимум ~40 строк** на функцию. Если больше — разделить.
- **Максимум ~5 параметров**. Если больше — передавай структуру.

### 5.2 Параметры

```cpp
// ✅ Хорошо
void SetPosition(float x, float y, float z);
void SetPosition(const glm::vec3& pos);

// ❌ Плохо: много параметров
void SetSprite(float x, float y, float w, float h, float r, float g, float b, float a,
               const std::string& texture, bool isUI, float ppu);
```

### 5.3 Передача по значению / ссылке / указателю

| Ситуация | Способ |
|---|---|
| **POD (int, float, bool)** | По значению |
| **Маленькие struct'ы (glm::vec2/3/4)** | По значению |
| **Большие struct'ы (glm::mat4)** | По `const&` |
| **Объекты с виртуальными методами** | По `const&` (для чтения) или `&` (для записи) |
| **Опциональные объекты** | По указателю (может быть nullptr) |
| **Out-параметры** | По `&` (только для модификации) |

### 5.4 Const-корректность

```cpp
// ✅ Метод, который не меняет состояние, помечен const
int GetValue() const { return m_Value; }
bool IsEmpty() const { return m_Size == 0; }

// ✅ Параметр, который не меняется, передаётся как const&
void Process(const std::string& input);

// ✅ Локальная переменная, которая не меняется
const float kPi = 3.14159f;
```

### 5.5 Имена параметров

```cpp
// ✅ Параметры с говорящими именами
void MoveTo(float x, float y, float z);

// ✅ Много параметров с префиксом (если нужно)
void SetSprite(float spriteX, float spriteY, float spriteW, float spriteH);
```

---

## 6. Переменные

### 6.1 Где объявлять

- **Внутри класса** — в `private:` (в самом низу).
- **Локальные** — максимально близко к месту использования.
- **Не в середине функции** — это C-style, в C++ так не пишут.

### 6.2 Инициализация

```cpp
// ✅ Всегда инициализируй при объявлении
int m_Count = 0;
std::string m_Name;
bool m_Active = false;

// ✅ Или в конструкторе
MyClass() : m_Count(0), m_Name("default"), m_Active(false) {}
```

### 6.3 Глобальные переменные

❌ **Глобальные переменные запрещены** (кроме `Logger`, `Engine::s_Instance`).

Используй singleton, namespace-static, или передавай через DI.

### 6.4 Const correctness

Все, что можно сделать `const`, должно быть `const`.

```cpp
// ✅ Параметры
void Render(const World& world, const OrthographicCamera& camera);

// ✅ Методы
int GetRows() const { return m_Rows; }

// ✅ Локальные
const auto* window = Engine::Get().GetWindow();
```

---

## 7. Форматирование

### 7.1 Отступы

- **4 пробела** (не табы). Visual Studio по умолчанию.
- **1 statement per line**.
- **1 declaration per line** (не `int a, b, c;`).

### 7.2 Скобки

**Allman style** (скобка на новой строке):

```cpp
// ✅ Allman
void Foo()
{
    if (condition)
    {
        DoSomething();
    }
    else
    {
        DoOther();
    }
}

// ❌ K&R style (1TBS) — НЕ используем
void Foo() {
    if (condition) {
        DoSomething();
    } else {
        DoOther();
    }
}
```

### 7.3 Длина строки

- **Максимум 120 символов** (некоторые используют 100, но 120 — предел).
- Если строка длиннее — переноси.

```cpp
// ✅ Перенос длинной строки
void SomeFunction(
    int firstParameter,
    const std::string& secondParameter,
    const glm::vec3& thirdParameter
);
```

### 7.4 Пустые строки

- **1 пустая строка** между методами.
- **2 пустые строки** между секциями (между `public:` и `protected:`).
- **Без пустых строк** внутри метода (если нужна — лучше разделить метод).

### 7.5 Switch

```cpp
switch (value)
{
    case 0:
        DoZero();
        break;
    case 1:
        DoOne();
        break;
    default:
        DoDefault();
        break;
}
```

### 7.6 Namespace closing comment

```cpp
namespace NK {
    // ...
} // namespace NK
```

---

## 8. Комментарии

### 8.1 Философия

- **WHY, not WHAT** — код показывает ЧТО, комментарий — ПОЧЕМУ.
- **Не комментируй очевидное** — `i++; // increment i` — запрещено.
- **Обновляй комментарии** при изменении кода (Boy Scout Rule).

### 8.2 Doxygen (для публичного API)

```cpp
/// Краткое описание (одна строка).
///
/// Подробное описание (если нужно). Может быть несколько строк.
///
/// @param x   Описание параметра x
/// @param y   Описание параметра y
/// @return    Описание возвращаемого значения
/// @see       Связанные функции
///
/// @code
/// int result = MyFunction(1, 2);
/// @endcode
int MyFunction(int x, int y);
```

### 8.3 TODO / FIXME / HACK

```cpp
// TODO: описание — сделать в будущем
// FIXME: описание — известный баг, надо починить
// HACK: описание — костыль, не трогать без понимания
// NOTE: описание — важное замечание
```

### 8.4 Комментарии в коде

```cpp
// ✅ Хороший комментарий: объясняет WHY
// Используем const_cast потому что EnTT view требует non-const registry
entt::registry& registry = const_cast<entt::registry&>(world.GetRegistry());

// ❌ Плохой комментарий: объясняет WHAT (очевидно из кода)
// Increment counter
m_Count++;
```

---

## 9. Error Handling

### 9.1 Принципы

- **Fail fast** — падай рано, в release — gracefully.
- **Не молчи** об ошибках. Если что-то пошло не так — log, exception, или return error code.
- **Не используй exit()** в библиотечном коде.

### 9.2 Exceptions vs Error codes

- **Exceptions** — для unexpected errors (nullptr, file not found).
- **Error codes / optional / expected** — для expected failures (validation, parse error).

### 9.3 Nullptr

```cpp
// ✅ Проверяй перед использованием
if (auto* tex = sprite.Texture)
{
    tex->Bind(0);
}

// ✅ Если nullptr — early return
void Foo(Texture* tex)
{
    if (!tex) return;
    tex->Bind();
}
```

### 9.4 Assertions

```cpp
// Debug-only assertion
NK_CORE_ASSERT(m_Size > 0, "Size must be positive");

// Runtime check (в release тоже)
if (m_Size <= 0)
{
    NK_CORE_ERROR("Invalid size: {0}", m_Size);
    return;
}
```

### 9.5 Логирование

```cpp
// ✅ Информационное
NK_CORE_INFO("Window resized to {0}x{1}", width, height);

// ✅ Предупреждение (что-то не так, но не критично)
NK_CORE_WARN("Texture not found, using fallback");

// ✅ Ошибка (что-то сломалось)
NK_CORE_ERROR("Failed to load shader: {0}", path);

// ✅ Трассировка (для отладки)
NK_CORE_TRACE("Entity created: {0}", entity);
```

---

## 10. Тестирование

> ⚠️ В NovaEngine пока нет unit-тестов. Это TODO v0.2+.

Когда появятся тесты:
- **Один тест на функцию** (или на аспект).
- **AAA pattern**: Arrange, Act, Assert.
- **Имена тестов**: `TestName_WhenCondition_ExpectResult` (Given-When-Then).
- **Не тестируй private** — тестируй public API.
- **Mock для зависимостей** (DI упрощает моки).

---

## 11. Архитектурные принципы (SOLID в NovaEngine)

### 11.1 Single Responsibility (SRP)

```cpp
// ❌ Плохо: Match3Game делает ВСЁ
class Match3Game {
    void LoadLua()    { /* ... */ }
    void SetupECS()   { /* ... */ }
    void Render()     { /* ... */ }
    void HandleInput() { /* ... */ }
};

// ✅ Хорошо: разделено на части
class Match3Game {
    std::unique_ptr<Match3System> m_System;       // игровая логика
    std::unique_ptr<World>         m_World;       // ECS
    std::shared_ptr<Shader>        m_SpriteShader; // рендер
};
```

### 11.2 Open/Closed (OCP)

```cpp
// ✅ Система расширяется через новые компоненты, не модифицируя World
// Хотим добавить LifetimeSystem? Просто пишем новую систему.
// World не меняется.
```

### 11.3 Liskov Substitution (LSP)

```cpp
// ✅ Match3BoardProxy подставляется вместо ожидаемого board в Lua
// Match3System реализует те же методы, что и старый Match3Board
```

### 11.4 Interface Segregation (ISP)

```cpp
// ❌ Один большой интерфейс
class IGameObject {
    virtual void Update() = 0;
    virtual void Render() = 0;
    virtual void OnCollision() = 0;
    virtual void OnClick() = 0;
    virtual void Serialize() = 0;
};

// ✅ Разделены на маленькие
class IUpdatable { virtual void Update() = 0; };
class IRenderable { virtual void Render() = 0; };
class IClickable { virtual void OnClick() = 0; };
```

### 11.5 Dependency Inversion (DIP)

```cpp
// ❌ Match3Game напрямую зависит от конкретной камеры
auto& camera = Engine::Get().GetScene().GetGameCamera();

// ✅ Зависимость от абстракции (когда возможно)
Match3Game(World& world, Camera& camera);
```

---

## 12. Антипаттерны

### 12.1 ❌ God Object

```cpp
// ❌ Один класс делает ВСЁ
class Engine {
    void Run() { /* ... */ }
    void Render() { /* ... */ }
    void Physics() { /* ... */ }
    void Audio() { /* ... */ }
    void Network() { /* ... */ }
    void Save() { /* ... */ }
};
```

### 12.2 ❌ Spaghetti Code

```cpp
// ❌ Код без структуры, всё в одном методе
void OnUpdate(float dt) {
    // 200 строк логики, без функций
}
```

### 12.3 ❌ Magic Numbers

```cpp
// ❌
if (m_Health < 25) { /* ... */ }

// ✅
constexpr int kCriticalHealthThreshold = 25;
if (m_Health < kCriticalHealthThreshold) { /* ... */ }
```

### 12.4 ❌ Глобальное состояние

```cpp
// ❌
int g_CurrentLevel = 1;
void SaveLevel() { g_CurrentLevel++; }

// ✅ Передавай через контекст
class LevelManager {
    int m_CurrentLevel = 1;
public:
    void SaveLevel() { m_CurrentLevel++; }
};
```

### 12.5 ❌ Длинные цепочки

```cpp
// ❌
auto result = engine.GetScene().GetCamera().GetFrustum().GetPlanes()[0].GetNormal().Normalize();

// ✅ Разбей
auto& scene = engine.GetScene();
auto& camera = scene.GetCamera();
auto& planes = camera.GetFrustum().GetPlanes();
auto normal = planes[0].GetNormal();
normal.Normalize();
```

### 12.6 ❌ Дублирование кода

```cpp
// ❌ Один и тот же код в двух местах
void ClassA::Render() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // ...
}
void ClassB::Render() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // ...
}

// ✅ Общий хелпер
void Renderer::EnableAlphaBlend() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
```

### 12.7 ❌ Premature Optimization

```cpp
// ❌ Оптимизация до того, как измерили
inline int FastMod(int x, int m) { return x - (x / m) * m; }  // зачем?
```

### 12.8 ❌ Глубокая вложенность

```cpp
// ❌
if (a) {
    if (b) {
        if (c) {
            if (d) {
                DoSomething();
            }
        }
    }
}

// ✅ Early return
if (!a || !b || !c || !d) return;
DoSomething();
```

---

## 13. Специфика NovaEngine

### 13.1 C++ standard

- **C++17** (не C++14, не C++20).
- `#pragma once` (не include guards).
- `nullptr` (не NULL, не 0).

### 13.2 Зависимости

| Lib | Для чего | Нюансы |
|---|---|---|
| **EnTT** | ECS | `entt::entity` (uint32). `#include <entt/entt.hpp>` (не `<EnTT/...>`) |
| **glm** | математика | `glm::vec2/3/4`, `glm::mat4`, `glm::quat` |
| **sol2** | Lua биндинги | В lambda ВСЕГДА disambiguate overloads (см. ниже) |
| **stb_image, stb_truetype** | загрузка ресурсов | inline, просто инклюдятся |
| **glad** | OpenGL loader | Ручная загрузка — добавлять в `glad/gl.h` + `GraphicsContext::Init` |

### 13.3 Overload resolution в inline-заголовках (v0.1.3 баг)

`SpriteRenderer` имеет 2 overloads `SetCustomSize`. В lambda для sol2 **ВСЕГДА disambiguate**:

```cpp
// ❌ linker-ошибка
"SetCustomSize", [](SpriteRenderer& sr, double w, double h) {
    sr.SetCustomSize((float)w, (float)h);
}

// ✅ OK
"SetCustomSize", [](SpriteRenderer& sr, double w, double h) {
    sr.SetCustomSize(glm::vec2(static_cast<float>(w), static_cast<float>(h)));
}
```

### 13.4 Координаты мыши

- **`Input::GetMousePosition`** — экранные (от монитора). **НЕ использовать в Lua.**
- **`InputSystem::GetMousePosition`** — клиентские (от окна). Использовать.
- **`Window::GetMouseClientPosition`** — клиентские. Использовать.

### 13.5 Lua скрипты

- Точки входа: `OnStart()`, `OnUpdate(dt)`, `OnShutdown()`.
- Имена файлов: `snake_case.lua` или `lowercase_underscore.lua`.
- Используй новый API: `GetMouseButton(MouseButton.Left)`, `GetMousePos()`.

---

## 14. Code Review чеклист

Перед коммитом:

- [ ] Код компилируется без warnings (`/W4`).
- [ ] Нет magic numbers — все константы названы.
- [ ] Нет копипасты — общая логика вынесена.
- [ ] Нет глобальных переменных.
- [ ] Нет `using namespace std;` (и других `using namespace` в headers).
- [ ] Const-корректность соблюдена.
- [ ] Все `new` имеют `delete` (или используются smart pointers).
- [ ] Нет raw `printf` / `cout` (только через `NK_*` логгер).
- [ ] Нет `TODO` без описания.
- [ ] Файлы ≤ 300 строк.
- [ ] Функции ≤ 40 строк.
- [ ] Параметров ≤ 5.
- [ ] Нет warnings от clang-tidy (если настроен).
- [ ] Код отформатирован (4 пробела, Allman braces).

---

## 15. Миграция / Legacy

В проекте есть legacy код (Scene/GameObject/Component). Правила работы с ним:

- **НЕ расширять legacy** — добавлять новые подсистемы через ECS.
- **Legacy можно использовать** в существующем коде, пока не мигрирован.
- **TODO:** отмечать в коде, если файл надо мигрировать: `// TODO: migrate to ECS in v0.5+`.

Когда мигрируем:
1. Создать новые ECS компоненты / системы.
2. Переключить Match3 (или другую игру) на новые системы.
3. Удалить legacy файлы через mavis-trash.
4. Обновить документацию.

---

## 16. Changelog стиля

| Версия | Что изменилось |
|---|---|
| v0.3.1 | Создан документ. Включены SOLID/DRY/KISS/YAGNI, naming, formatting, antipatterns. |

---

> **См. также:**
> - [AGENTS.md](AGENTS.md) — общие правила проекта.
> - [docs/02_Подсистемы/](docs/02_Подсистемы/) — документация подсистем.
> - [wiki/Coding-Conventions.md](wiki/Coding-Conventions.md) — короткая версия для контрибьюторов.
