# Scripting API (Lua)

> 🚧 **TODO:** страница-заготовка.

## Глобальные функции

| Функция | Описание |
|---|---|
| `Log(msg)` | Вывести в лог (`NK_INFO`) |
| `GetScene()` | Получить текущую сцену |
| `GetTexture(name)` | Загрузить/получить из кэша текстуру |
| `GetShader(name, vs, fs)` | Получить/создать шейдер |
| `GetWindowWidth() / GetWindowHeight()` | Размер окна |
| `EngineVersion()` | Версия движка |
| `GetBoard()` (Match3) | Получить `Match3Board` (proxy → `Match3System`) |
| `CreateSolidColorTexture(r,g,b,a)` | 1×1 текстура |

## Ввод

| Функция | Описание |
|---|---|
| `GetKey(KeyCode.X)` | Удерживается? |
| `GetKeyDown(KeyCode.X)` | Только что нажата? |
| `GetMouseButton(MouseButton.Left)` | Удерживается? |
| `GetMouseButtonDown(MouseButton.Left)` | Только что нажата? |
| `GetMousePos()` | **Клиентские** координаты |

❌ Не используй `IsMouseButtonDown(1)`, `GetMousePosition()` (экранные, баг).

## Классы

- `GameObject` — `CreateGameObject(name)`, `GetTransform()`, `GetSpriteRenderer()`, `AddComponent_Transform()`, `AddComponent_SpriteRenderer()`.
- `Scene` — `CreateGameObject`, `CreateUIObject`, `GetGameCamera`, `GetUICamera`.
- `Match3Board` (Match3) — `FillRandom`, `Swap(r1,c1,r2,c2)`, `FindMatches`, `RemoveTiles`, `ApplyGravity`, `FillEmpty`, `HasPossibleMoves`, `Mix`, `OnTileChanged`.

## Пример (Match3 OnStart)

```lua
function OnStart()
    local scene = GetScene()
    local board = GetBoard()

    board.OnTileChanged = function(row, col, newType)
        local key = row .. "," .. col
        if newType == -1 then return end
        local obj = scene:CreateGameObject("Tile_" .. key)
        local sr = obj:AddComponent_SpriteRenderer()
        sr:SetTexture(CreateSolidColorTexture(255, 0, 0, 255))
        sr:SetCustomSize(0.64, 0.64)
    end

    board:FillRandom()
end
```

→ [полный Lua API](../docs/03_Скриптинг_на_Lua/01_Lua_API_справочник.md)
