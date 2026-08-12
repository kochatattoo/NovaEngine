# 02 — Игра Match3 (механика, логика, визуализация)

> **Статус:** 🟢 Полностью играбельна

**Файлы:**
- `SandBox/Source/Game/Match3Board.h/.cpp` — игровая логика
- `SandBox/Source/Game/Match3Game.h/.cpp` — связка с Lua
- `SandBox/assets/scripts/game_match3.lua` — визуализация и ввод

## Общая идея

Классический Match3: поле 10×10, 6 типов плиток. Игрок свайпает две соседние плитки мышью. Если после свайпа образуется линия из 3+ одинаковых плиток по горизонтали или вертикали — линия удаляется, плитки сверху падают вниз (гравитация), пустоты заполняются новыми случайными. Если после этого образуются новые линии — каскад продолжается. Если ходов больше нет — поле перемешивается.

## Класс `Match3Board`

**Файл:** `SandBox/Source/Game/Match3Board.h`

```cpp
class Match3Board {
public:
    using TileType = int;
    using Grid = std::vector<std::vector<TileType>>;

    Match3Board(int rows, int cols, float cellSize = 64.0f, float pixelsPerUnit = 100.0f);

    void FillRandom();
    void SetTile(int row, int col, TileType type);
    TileType GetTile(int row, int col) const;
    bool IsValidCell(int row, int col) const;
    bool HasPossibleMoves();

    bool Swap(int r1, int c1, int r2, int c2);
    std::vector<std::pair<int, int>> FindMatches() const;
    void RemoveTiles(const std::vector<std::pair<int, int>>& cells);
    void ApplyGravity();
    void FillEmpty();
    void Mix();

    int GetRows() const;
    int GetCols() const;
    float GetCellSize() const;
    glm::vec2 GetCellPosition(int row, int col) const;
    float GetPixelsPerUnit() const;

    // Callback
    std::function<void(int row, int col, TileType newType)> OnTileChanged;

private:
    int m_Rows, m_Cols;
    float m_CellSize;
    float m_PixelsPerUnit;
    Grid m_Grid;
};
```

### `FillRandom()`

```cpp
void Match3Board::FillRandom() {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 5);

    for (int r = 0; r < m_Rows; ++r) {
        for (int c = 0; c < m_Cols; ++c) {
            TileType type;
            bool valid = false;
            while (!valid) {
                type = dist(rng);
                // Проверяем, не образуется ли тройка
                if (c >= 2 && m_Grid[r][c-1] == type && m_Grid[r][c-2] == type) continue;
                if (r >= 2 && m_Grid[r-1][c] == type && m_Grid[r-2][c] == type) continue;
                valid = true;
            }
            m_Grid[r][c] = type;
            if (OnTileChanged) OnTileChanged(r, c, type);
        }
    }
}
```

Генерирует поле, **избегая** начальных троек (иначе игрок сразу получит очки).

### `Swap(r1, c1, r2, c2)`

```cpp
bool Match3Board::Swap(int r1, int c1, int r2, int c2) {
    if (!IsValidCell(r1, c1) || !IsValidCell(r2, c2)) return false;
    std::swap(m_Grid[r1][c1], m_Grid[r2][c2]);
    if (OnTileChanged) {
        OnTileChanged(r1, c1, m_Grid[r1][c1]);
        OnTileChanged(r2, c2, m_Grid[r2][c2]);
    }
    return true;
}
```

Меняет две клетки и **оповещает** через `OnTileChanged`. **Не проверяет**, даёт ли это матч — это делает вызывающий код.

### `FindMatches()`

```cpp
std::vector<std::pair<int, int>> Match3Board::FindMatches() const {
    std::vector<std::pair<int, int>> matches;
    std::vector<std::vector<bool>> marked(m_Rows, std::vector<bool>(m_Cols, false));

    // 1. Горизонтальные цепочки (≥3)
    for (int r = 0; r < m_Rows; ++r) {
        for (int c = 0; c < m_Cols - 2; ++c) {
            int type = m_Grid[r][c];
            if (type == -1) continue;

            if (m_Grid[r][c+1] == type && m_Grid[r][c+2] == type) {
                marked[r][c]   = true;
                marked[r][c+1] = true;
                marked[r][c+2] = true;
                int nextC = c + 3;
                while (nextC < m_Cols && m_Grid[r][nextC] == type) {
                    marked[r][nextC] = true;
                    nextC++;
                }
            }
        }
    }

    // 2. Вертикальные цепочки (≥3)
    // (аналогично)

    // 3. Собираем помеченные
    for (int r = 0; r < m_Rows; ++r)
        for (int c = 0; c < m_Cols; ++c)
            if (marked[r][c])
                matches.emplace_back(r, c);

    std::sort(matches.begin(), matches.end());
    return matches;
}
```

Возвращает **уникальный** список всех клеток, входящих в линии ≥3. Линии могут пересекаться — все клетки попадают.

### `RemoveTiles(cells)`

```cpp
void Match3Board::RemoveTiles(const std::vector<std::pair<int, int>>& cells) {
    for (auto [r, c] : cells) {
        m_Grid[r][c] = -1;
        if (OnTileChanged) OnTileChanged(r, c, -1);
    }
}
```

Ставит -1 ("пусто") во все клетки. `-1` — это сигнал "нет плитки".

### `ApplyGravity()`

```cpp
void Match3Board::ApplyGravity() {
    for (int c = 0; c < m_Cols; ++c) {
        int writeRow = m_Rows - 1;
        for (int r = m_Rows - 1; r >= 0; --r) {
            if (m_Grid[r][c] != -1) {
                if (r != writeRow) {
                    m_Grid[writeRow][c] = m_Grid[r][c];
                    m_Grid[r][c] = -1;
                    if (OnTileChanged) {
                        OnTileChanged(writeRow, c, m_Grid[writeRow][c]);
                        OnTileChanged(r, c, -1);
                    }
                }
                --writeRow;
            }
        }
    }
}
```

Плитки "падают" вниз по каждому столбцу. Реализовано как two-pointer: `writeRow` указывает на последнюю пустую клетку снизу.

### `FillEmpty()`

```cpp
void Match3Board::FillEmpty() {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 5);

    for (int r = 0; r < m_Rows; ++r) {
        for (int c = 0; c < m_Cols; ++c) {
            if (m_Grid[r][c] != -1) continue;

            // Перебираем типы, пока не найдём тот, который не создаёт начальный матч
            // (то же поведение, что в FillRandom, но для одной клетки)
            TileType type;
            int attempts = 0;
            const int maxAttempts = 16;
            do {
                type = dist(rng);
                bool wouldMatch = false;
                if (c >= 2 && m_Grid[r][c-1] == type && m_Grid[r][c-2] == type)
                    wouldMatch = true;
                if (!wouldMatch && r >= 2 && m_Grid[r-1][c] == type && m_Grid[r-2][c] == type)
                    wouldMatch = true;
                if (!wouldMatch) break;
                ++attempts;
            } while (attempts < maxAttempts);

            m_Grid[r][c] = type;
            if (OnTileChanged) OnTileChanged(r, c, m_Grid[r][c]);
        }
    }
}
```

Заполняет пустоты случайными плитками (0..5). **v0.1.1:** теперь избегает создания начального матча (как `FillRandom`).

### `Mix()`

```cpp
void Match3Board::Mix() {
    const int maxAttempts = 50;
    int attempts = 0;
    do {
        FillRandom();
        ++attempts;
    } while (!HasPossibleMoves() && attempts < maxAttempts);

    if (!HasPossibleMoves()) {
        NK_WARN("Match3Board::Mix: failed to find a state with possible moves after %d attempts", maxAttempts);
    }
}
```

Перемешивает, пока не будет хотя бы одного хода. **v0.1.1:** добавлен safety limit (50 попыток) — иначе возможен бесконечный цикл на почти полностью заполненном поле. Если не получилось — warning в лог.

### `HasPossibleMoves()`

```cpp
bool Match3Board::HasPossibleMoves() {
    for (int r = 0; r < m_Rows; ++r) {
        for (int c = 0; c < m_Cols; ++c) {
            if (m_Grid[r][c] == -1) continue;
            // Пробуем swap с правым
            if (c + 1 < m_Cols && m_Grid[r][c+1] != -1) {
                std::swap(m_Grid[r][c], m_Grid[r][c+1]);
                auto matches = FindMatches();
                std::swap(m_Grid[r][c], m_Grid[r][c+1]);
                if (!matches.empty()) return true;
            }
            // Пробуем swap с нижним
            if (r + 1 < m_Rows && m_Grid[r+1][c] != -1) {
                std::swap(m_Grid[r][c], m_Grid[r+1][c]);
                auto matches = FindMatches();
                std::swap(m_Grid[r][c], m_Grid[r+1][c]);
                if (!matches.empty()) return true;
            }
        }
    }
    return false;
}
```

**O(N² · M)** где M = стоимость FindMatches. На поле 10×10 = 100 клеток × 2 swap'а × FindMatches = быстро.

### `GetCellPosition(row, col)`

```cpp
glm::vec2 Match3Board::GetCellPosition(int row, int col) const {
    float x = (col + 0.5f) * m_CellSize / m_PixelsPerUnit;
    float y = (row + 0.5f) * m_CellSize / m_PixelsPerUnit;
    return glm::vec2(x, y);
}
```

Возвращает **центр** клетки в мировых координатах. `(0,0)` = (0.5 * cellSize / ppu, 0.5 * cellSize / ppu) (центр верхнего левого угла сетки).

## Визуализация в Lua

`game_match3.lua` создаёт `tileObjects[row .. "," .. col] = GameObject("Tile_...")` на каждый матч `OnTileChanged`. Каждый объект имеет:
- `Transform` (позиция)
- `SpriteRenderer` (текстура = цветная 1×1 текстура)

Координаты рассчитываются с `BOARD_OFFSET_X/Y` — смещением, чтобы поле было по центру камеры.

### Координатная система

В `Match3Board` (0,0) — **верхний-левый угол** (как принято в 2D-гридах).
В OpenGL (0,0) — **нижний-левый** (Y растёт вверх).
В `OrthographicCamera` — по умолчанию `(-5,-5) → (5,5)`, центр (0,0), Y вверх.

В `game_match3.lua` используется `visualRow = (ROWS - 1) - row` — инвертирует Y, чтобы логический "верх" отображался в верхнюю часть экрана.

## Ввод

```lua
-- OnUpdate(dt)
if IsMouseButtonDown(1) then
    -- ЛКМ удерживается — выделить плитку
    pressedObj = tileObjects[row..","..col]
    pressedObj:GetTransform():SetScale(1.2, 1.2)   -- визуальный feedback
else
    -- ЛКМ отпущена — попробовать swap
    if pressedObj then
        local dr = math.abs(pressedRow - row)
        local dc = math.abs(pressedCol - col)
        if (dr == 1 and dc == 0) or (dr == 0 and dc == 1) then
            board:Swap(pressedRow, pressedCol, row, col)
            local matches = board:FindMatches()
            if #matches == 0 then
                board:Swap(pressedRow, pressedCol, row, col)   -- откат
            else
                -- обработать матчи, каскады, ...
            end
        end
    end
end
```

⚠️ `IsMouseButtonDown(1)` — это legacy-биндинг (1 = VK_LBUTTON). Можно использовать `GetMouseButton(MouseButton.Left)`.

## Полный игровой цикл

```
Игрок нажимает ЛКМ на плитке → pressedObj запомнен
Игрок тянет на соседнюю клетку → на отпускание:
    1. board:Swap(...)
    2. matches = board:FindMatches()
    3. Если matches пуст → откатить Swap
    4. Иначе:
        a. board:RemoveTiles(matches)
        b. board:ApplyGravity()
        c. board:FillEmpty()
        d. Цикл каскадов (пока FindMatches не пуст)
        e. Если !HasPossibleMoves → board:Mix()
```

## Известные ограничения

- ❌ Нет анимаций (исчезновение/падение — мгновенные).
- ❌ Нет подсчёта очков в UI (`score` объявлен, но не отображается).
- ❌ Нет специальных плиток (`SpecialBomb=100`, `SpecialRainbow=101` зарезервированы, но не используются).
- ❌ Нет ограничения ходов / времени.
- ❌ Нет undo.
- ❌ Нет уровней / целей.

## Что улучшено в v0.1.1

- ✅ **`FillEmpty`** — теперь не создаёт начальный матч (аналогично `FillRandom`).
- ✅ **`Mix`** — safety limit 50 попыток + warning в лог при неудаче.
- ✅ **`Match3Game::SetupLuaBindings`** — убрано дублирование (`GetMousePosition`, `GetWindowWidth/Height`, `IsMouseButtonDown`, `GetScene` уже в `LuaFuncBindings`).
- ✅ **`Game.cpp` (SandboxApp)** — убран мёртвый код (`m_ColorR/G/B`, тестовый ECS, `OnEvent` override).

## TODO v0.2

- [ ] Анимация падения (tween позиции).
- [ ] Анимация исчезновения (tween альфа-канала).
- [ ] Специальные плитки: бомба (4-в-ряд → бомба), радуга (5-в-ряд).
- [ ] UI со счётом и индикатором ходов.
- [ ] Звуки совпадений.

См. также: [01_Архитектура_Sandbox](01_Архитектура_Sandbox.md), [03_Скриптинг_на_Lua/02_Lua_примеры](../03_Скриптинг_на_Lua/02_Lua_примеры.md).
