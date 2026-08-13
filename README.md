# NovaEngine

> **Учебный 2D-игровой движок на C++17 + Lua + ECS (EnTT)**
> Текущая версия: **v0.2** (ECS-миграция Match3 в процессе)
> Автор: [@kochatattoo](https://github.com/kochatattoo)

[![Status: v0.2](https://img.shields.io/badge/status-v0.2--WIP-yellow)]()
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)]()
[![Platform: Windows](https://img.shields.io/badge/platform-Windows-lightgrey)]()
[![License: MIT](https://img.shields.io/badge/license-MIT-green)]()

NovaEngine — это pet-project игровой движок, который я пишу для глубокого погружения в C++, OpenGL, ECS и архитектуру игр в целом. На нём крутится простая игра Match3 как «canary» — на ней проверяются все архитектурные решения перед тем, как двинуть их в более сложные проекты.

---

## ✨ Что внутри

- **C++17, статическая библиотека + исполняемый проект** — `Engine.lib` + `SandBox.exe`.
- **OpenGL 3.3 Core**, ручная загрузка через **glad** (без GLFW/SDL — только WinAPI + WGL).
- **Lua 5.4** (исходники вкомпилированы в `Engine.lib`) + **sol2** для биндингов.
- **EnTT** (Entity-Component-System) — современный data-oriented подход.
- **gluten-free** — никаких чужих движков под капотом, кроме математики (`glm`) и утилит (`stb_image`, `stb_truetype`).
- **Скриптуемая логика игр** на Lua, движок даёт API для сцены, камеры, ввода, рендера, UI.

## 🎮 Демо-игра: Match3

В папке `SandBox/` — рабочая игра «три в ряд» 10×10, шесть цветов, поиск матчей, гравитация, каскады. Логика в `SandBox/Source/Game/`, скриптовая визуализация и обработка ввода — `SandBox/assets/scripts/game_match3.lua`.

## 🛠️ Состояние по подсистемам (v0.2)

| Подсистема | Статус | Комментарий |
|---|---|---|
| Окно + GL-контекст | ✅ | WinAPI + WGL, glad |
| Рендерер (спрайты, текст) | ✅ | Quads, batching в разработке |
| Сцена + GameObject + компоненты | ✅ | Классическая иерархия |
| ECS (EnTT) | 🟡 v0.2 | World + name-based lookup, Match3 на ECS, рендер из ECS — в плане |
| Lua-интеграция | ✅ | sol2, разделение по файлам биндингов |
| Ввод (InputSystem) | ✅ | Клиентские/экранные координаты, key states, scroll |
| UI (Button, Anchor) | ✅ | Screen anchors, не ECS |
| Звук | ❌ | Не начато |
| Сеть | ❌ | Не начато |

Подробная разбивка — в [`docs/06_План_разработки/`](docs/06_План_разработки/).

## 🚀 Quick Start

### Требования
- **Windows 10/11** (только — пока нет кроссплатформенности).
- **Visual Studio 2022 Community** (или Build Tools 2022) с workload «Desktop development with C++».
- **vcpkg** (для `glm`).
- **Git** для клонирования.

### Установка
```powershell
# 1. Клонируем
git clone https://github.com/kochatattoo/NovaEngine.git
cd NovaEngine

# 2. Ставим зависимость (один раз)
vcpkg install glm:x64-windows

# 3. Интеграция vcpkg в солюшен (уже настроено через vcpkg manifest)
# Если нужно: vcpkg integrate install
```

### Сборка и запуск
```powershell
# Из Developer PowerShell for VS 2022:
msbuild NovaEngine.sln /p:Configuration=Debug /p:Platform=x64
.\x64\Debug\SandBox.exe
```

Или открыть `NovaEngine.sln` в Visual Studio, выбрать `SandBox` как startup project, нажать **F5**.

> ⚠️ **Первый запуск:** Post-build event копирует `SandBox/assets/` в `x64\Debug\assets\`. Если Lua-скрипты не найдены — будет ошибка `Lua function 'OnUpdate' not found`. Проверьте, что `assets/scripts/game_match3.lua` лежит рядом с `SandBox.exe`.

## 📁 Структура репозитория

```
NovaEngine/
├── Engine/                # Статическая библиотека (все подсистемы)
│   ├── Include/           # Публичные заголовки
│   └── Source/            # Реализации
├── SandBox/               # Исполняемый проект — игра Match3
│   ├── Source/            # C++ код игры
│   ├── assets/            # Lua-скрипты, текстуры, шрифты
│   └── SandBox.vcxproj
├── Libraries/             # Внешние зависимости (lua, sol, EnTT, glm)
├── docs/                  # Системная документация (см. ниже)
├── wiki/                  # GitHub Wiki исходники
├── vcpkg/                 # Локальная установка пакетов
├── .agents/               # Инструкции для AI-агентов
├── AGENTS.md              # Правила для AI-агентов (главные)
└── NovaEngine.sln
```

## 📚 Документация

- **[`docs/`](docs/README.md)** — полная системная документация (иерархия, жизненный цикл, подсистемы, Lua API, Match3, план разработки).
- **[`wiki/`](wiki/Home.md)** — GitHub Wiki исходники (короткая справка для новых читателей).
- **[`.agents/AGENTS.md`](.agents/AGENTS.md)** — правила и конвенции проекта.

### Куда смотреть

- 🏗️ **Хочу понять архитектуру** → [`docs/01_Архитектура/`](docs/01_Архитектура/)
- 🧩 **Конкретная подсистема** → [`docs/02_Подсистемы/`](docs/02_Подсистемы/) (Core, Renderer, Scene, ECS, Input, UI, Lua, …)
- 📜 **Как писать скрипты на Lua** → [`docs/03_Скриптинг_на_Lua/`](docs/03_Скриптинг_на_Lua/)
- 🎮 **Как устроен Match3** → [`docs/04_Sandbox_и_Match3/`](docs/04_Sandbox_и_Match3/)
- 🔨 **Как собрать проект** → [`docs/05_Сборка_и_запуск/`](docs/05_Сборка_и_запуск/)
- 🗺️ **Что планируется** → [`docs/06_План_разработки/`](docs/06_План_разработки/)

## 🗺️ Roadmap

Текущая фокусная задача — **v0.2: полная миграция Match3 на ECS**.

| Версия | Цель | Статус |
|---|---|---|
| v0.1.x | Рабочий движок + Match3 на Scene/GameObject | ✅ |
| **v0.2** | **ECS-миграция Match3** (логика на EnTT) | 🟡 В процессе |
| v0.2.6 | `SpriteRenderSystem` — рендер прямо из ECS | ⏳ |
| v0.2.7 | Удалить `Match3Board` (POD), оставить только `Match3System` | ⏳ |
| v0.3 | UI на ECS (Anchor, Button как компоненты) | ⏳ |
| v0.4 | Сериализация World (save/load) | ⏳ |
| v0.5 | Удалить `Scene/GameObject/Component` (legacy) | ⏳ |
| v0.6+ | Звук, камера-как-entity, пост-процессинг, многопоточность, Linux/macOS | 🔮 |

## 🏗️ Разработка

Это **pet-project**, написанный одним человеком. Я учусь:
- Писать идиоматичный современный C++.
- Проектировать игровые подсистемы с нуля.
- Использовать EnTT для data-oriented дизайна.
- Работать с OpenGL на низком уровне (без движков-обёрток).
- Строить скриптуемые runtime'ы на Lua.

**Принимаю предложения и фидбек** — открывайте issues или pull requests.

## 🤝 Вклад

Пока это личный проект, но если у вас есть полезные патчи (особенно в ECS, рендере, Linux-порте) — велкам. Проект следует правилам в [`AGENTS.md`](AGENTS.md) (нейминг, стиль, layout).

## 📄 License

MIT — см. [`LICENSE`](LICENSE).

## 📞 Контакт

- **Автор:** Vlad Kochetov ([@kochatattoo](https://github.com/kochatattoo))
- **Telegram:** [@kochatattoo](https://t.me/kochatattoo)
- **Тату-блог:** [kchtt.ru](https://kchtt.ru)

---

> *Документация синхронизирована с `docs/` в OneDrive (Obsidian). Если что-то не совпадает — откройте issue.*
