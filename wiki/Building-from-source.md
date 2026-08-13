# Building from source

> 🚧 **TODO:** страница-заготовка.

## Требования

- **Windows 10/11** — только Windows, кроссплатформенность не реализована.
- **Visual Studio 2022 Community** (или Build Tools 2022) с workload «Desktop development with C++» + компонент «Windows 10/11 SDK».
- **vcpkg** (для `glm`).
- **Git**.

## Шаги

```powershell
# 1. Клонируем
git clone https://github.com/kochatattoo/NovaEngine.git
cd NovaEngine

# 2. Зависимости
vcpkg install glm:x64-windows

# 3. Сборка
msbuild NovaEngine.sln /p:Configuration=Debug /p:Platform=x64

# 4. Запуск
.\x64\Debug\SandBox.exe
```

## Структура выходных файлов

```
x64/
└── Debug/
    ├── SandBox.exe
    └── assets/        ← post-build copy из SandBox/assets/
        ├── scripts/
        ├── textures/
        ├── fonts/
        └── ui/
```

## Конфигурации

- `Debug | x64` — разработка (используется по умолчанию).
- `Release | x64` — оптимизированная.

## Подводные камни

- **`sol/sol.hpp` not found** — vcpkg не интегрирован. Запустите `vcpkg integrate install`.
- **`EnTT/entt/entt.hpp` not found** — vcxproj уже настроен, должно работать. Если нет — проверьте `$(SolutionDir)Libraries\EnTT` в include paths.
- **Lua-скрипты не найдены** в runtime — post-build event не сработал. Скопируйте `assets/` вручную.

→ [Getting Started](Getting-Started.md) — быстрый путь.
