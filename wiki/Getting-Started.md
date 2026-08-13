# Getting Started

> 🚧 **TODO:** страница-заготовка. Заполнить после стабилизации v0.2.

Краткий путь от `git clone` до запущенного Match3.

## Требования

- Windows 10/11
- Visual Studio 2022 Community (или Build Tools 2022)
- vcpkg (для `glm`)
- Git

## Установка

```powershell
git clone https://github.com/kochatattoo/NovaEngine.git
cd NovaEngine
vcpkg install glm:x64-windows
```

## Сборка и запуск

```powershell
msbuild NovaEngine.sln /p:Configuration=Debug /p:Platform=x64
.\x64\Debug\SandBox.exe
```

Или: открыть `NovaEngine.sln` в Visual Studio, F5.

## Что должно произойти

Окно 1280×720, в нём поле 10×10 из цветных плиток. Мышью можно свапать соседние плитки — при совпадении 3+ они исчезают, сверху падают новые.

## Если что-то не работает

- **`Lua function 'OnUpdate' not found`** — post-build event не скопировал `assets/`. Скопируйте вручную: `xcopy /y /e /r /i SandBox\assets x64\Debug\assets\`.
- **Чёрное окно** — проверьте, что драйвер GPU поддерживает OpenGL 3.3.
- **Linker error** на `sol/sol.hpp` или `entt/entt.hpp` — проверьте, что vcpkg интегрирован, или include paths в vcxproj.

→ [Building from source](Building-from-source.md) — детали.
