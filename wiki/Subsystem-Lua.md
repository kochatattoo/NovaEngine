# Subsystem: Lua (sol2, биндинги)

> 🚧 **TODO:** страница-заготовка.

## Архитектура

`LuaManager` владеет `sol::state`. Биндинги разделены по файлам:

| Файл | Что регистрирует |
|---|---|
| `LuaClassBindings.cpp` | `GameObject`, `Scene` |
| `LuaComponentBindings.cpp` | `Transform`, `SpriteRenderer`, `TextRenderer` |
| `LuaCameraBindings.cpp` | `OrthographicCamera` |
| `LuaFuncBindings.cpp` | `Log`, `GetScene`, `GetTexture`, `GetShader`, `GetMousePosition` |
| `LuaInputBindings.cpp` | `KeyCode`, `MouseButton`, `GetKey/Down/Up`, `GetMouseButton*`, `GetMousePos` |

## Порядок регистрации важен

`LuaInputBindings` идёт **после** `LuaFuncBindings` — последняя `RegisterFunction("name")` перезаписывает. Дубликаты — частая ошибка.

## Lua-скрипты в SandBox

- Точки входа: `OnStart()`, `OnUpdate(dt)`, `OnShutdown()`.
- Путь: `assets/scripts/<name>.lua`.
- Post-build event копирует `assets/` в `$(OutDir)assets/`.

## Подводный камень: overloads

```cpp
// ❌ linker-ошибка (MSVC Release inline)
"SetCustomSize", [](SpriteRenderer& sr, double w, double h) {
    sr.SetCustomSize((float)w, (float)h);
}

// ✅ OK
"SetCustomSize", [](SpriteRenderer& sr, double w, double h) {
    sr.SetCustomSize(glm::vec2((float)w, (float)h));
}
```

→ [полная документация](../docs/02_Подсистемы/10_Lua_интеграция.md)
