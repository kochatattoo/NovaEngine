# Match3 Game

> 🟡 v0.2 — логика на ECS, рендер пока через Scene/GameObject.

## Что это

Простая игра «три в ряд» 10×10, шесть цветов. Игрок свапает две соседние плитки мышью. Если после свапа есть 3+ одинаковых плитки в ряд (горизонтально или вертикально) — они удаляются, сверху падают новые, каскад продолжается.

## Файлы

| Файл | Что |
|---|---|
| `SandBox/Source/Game/ECS/Match3System.h/.cpp` | Игровая логика + 100 ECS-entities |
| `SandBox/Source/Game/ECS/Match3TileComponent.h` | POD-компонент плитки (Row, Col, Type) |
| `SandBox/Source/Game/ECS/Match3BoardProxy.h` | Lua-прокси (для биндинга `std::function`) |
| `SandBox/Source/Game/Match3Game.h/.cpp` | Связка C++ ↔ Lua, owns `World` + `Match3System` |
| `SandBox/assets/scripts/game_match3.lua` | Визуализация + ввод мыши |

## API

```lua
board = GetBoard()
board:FillRandom()                       -- заполнить поле
board:Swap(r1, c1, r2, c2)               -- обмен (возврат: bool)
matches = board:FindMatches()            -- { {r,c}, {r,c}, ... }
board:RemoveTiles(matches)               -- удалить
board:ApplyGravity()                     -- падение вниз
board:FillEmpty()                        -- заполнить пустоты
board:HasPossibleMoves()                 -- есть ходы?
board:Mix()                              -- перемешать
board:GetCellPosition(r, c)              -- {x, y} в мировых координатах
```

## Тик (вручную)

```lua
board:Swap(r1, c1, r2, c2)
local matches = board:FindMatches()
if #matches == 0 then
    board:Swap(r1, c1, r2, c2)   -- откат
else
    while #matches > 0 do
        board:RemoveTiles(matches)
        board:ApplyGravity()
        board:FillEmpty()
        matches = board:FindMatches()
    end
    if not board:HasPossibleMoves() then
        board:Mix()
    end
end
```

→ [полная документация](../docs/04_Sandbox_и_Match3/02_Игра_Match3.md)
