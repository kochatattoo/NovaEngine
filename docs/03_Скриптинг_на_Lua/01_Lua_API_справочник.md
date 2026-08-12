# 01 — Справочник Lua API

> **Статус:** 🟢 Актуально для v0.1

Полный API, доступный из `.lua` файлов в `assets/scripts/`. Все классы/функции регистрируются в `Engine::SetupLuaBindings()` → `LuaBindings::RegisterAll(LuaManager&)`.

## Глобальные функции

### Логирование

| Функция | Описание |
|---|---|
| `Log(msg)` | Вывести `msg` в ClientLogger. |

### Сцена и движок

| Функция | Описание |
|---|---|
| `GetEngine()` | Возвращает `Engine&`. |
| `GetScene()` | Возвращает `Scene&`. |

### Ресурсы

| Функция | Описание |
|---|---|
| `GetTexture(path)` | `Texture2D&` (shared) из `ResourceManager`. |
| `GetShader(name, vSrc, fSrc)` | `Shader&` — компилирует + кэширует по хэшу src. |
| `LoadFont(path)` | `Font&` или `nil` при ошибке. |
| `CreateSolidColorTexture(r, g, b, a)` | `Texture2D&` 1×1 заданного цвета. |

### Окно и ввод (legacy)

| Функция | Описание |
|---|---|
| `GetMousePosition()` | `(x, y)` (lua tuple). |
| `GetWindowWidth()` | int. |
| `GetWindowHeight()` | int. |
| `IsKeyDown(key)` | bool, key — int (VK_*). |
| `IsMouseButtonDown(button)` | bool, button — int (VK_LBUTTON = 1). |
| `SetClearColor(r, g, b, a)` | Цвет фона. |

### Ввод (новый, через `InputSystem`)

| Функция | Описание |
|---|---|
| `GetKey(KeyCode.X)` | bool — удерживается. |
| `GetKeyDown(KeyCode.X)` | bool — только что нажата. |
| `GetKeyUp(KeyCode.X)` | bool — только что отпущена. |
| `GetMouseButton(MouseButton.X)` | bool — удерживается. |
| `GetMouseButtonDown(MouseButton.X)` | bool — только что нажата. |
| `GetMouseButtonUp(MouseButton.X)` | bool — только что отпущена. |
| `GetMousePos()` | `(x, y)` (lua tuple). |

## Enum'ы

### `KeyCode`

```lua
KeyCode.A … KeyCode.Z
KeyCode.Num0 … KeyCode.Num9
KeyCode.F1 … KeyCode.F12
KeyCode.Left, Right, Up, Down
KeyCode.Space, Escape, Enter, Tab, Backspace
KeyCode.Insert, Delete, Home, End, PageUp, PageDown
KeyCode.CapsLock, NumLock, ScrollLock, PrintScreen, Pause
KeyCode.LeftShift, LeftControl, LeftAlt
KeyCode.RightShift, RightControl, RightAlt
KeyCode.Numpad0 … KeyCode.Numpad9
KeyCode.NumpadAdd, Subtract, Multiply, Divide, Decimal
```

### `MouseButton`

```lua
MouseButton.Left
MouseButton.Right
MouseButton.Middle
```

## Классы

### `GameObject`

```lua
local obj = scene:CreateGameObject("Name")
local uiObj = scene:CreateUIObject("UIName")

-- Добавление компонентов
obj:AddComponent_Transform()
obj:AddComponent_SpriteRenderer()
obj:AddComponent_TextRenderer()
obj:AddComponent_Button()
obj:AddComponent_Anchor()
obj:AddComponent_Script("path/to/script.lua")

-- Получение компонентов
local t = obj:GetTransform()
local sr = obj:GetSpriteRenderer()
local tr = obj:GetTextRenderer()
local btn = obj:GetButton()

-- Прочее
obj:SetZOrder(2)
obj:GetZOrder()        -- int
obj:GetName()          -- "Name"
obj:OnStart()
obj:OnUpdate(dt)
```

### `Scene`

```lua
local scene = GetScene()

scene:CreateGameObject("Name")     -- → GameObject
scene:CreateUIObject("UIName")     -- → GameObject (UI)
scene:OnStart()
scene:OnUpdate(dt)
scene:OnRender()
local cam = scene:GetGameCamera()  -- → OrthographicCamera
local uiCam = scene:GetUICamera()  -- → OrthographicCamera
```

### `Transform` (компонент, :base Component)

```lua
local t = obj:AddComponent_Transform()

t:SetPosition(x, y, z)         -- 3 числа
t:GetPosition()                -- (x, y, z) lua tuple
t:SetRotation(angleDegrees)
t:GetRotationDegrees()         -- float
t:SetScale(x, y)               -- 2 числа
t:GetScale()                   -- (x, y) lua tuple
```

### `SpriteRenderer` (компонент, :base Component)

```lua
local sr = obj:AddComponent_SpriteRenderer()

sr:SetTexture(tex)             -- Texture2D (shared_ptr)
sr:SetShader(shader)           -- Shader (shared_ptr)
sr:SetAlignment(hAlign, vAlign) -- 0..1, 0..1
sr:SetIsUI(true)
sr:SetColor(r, g, b, a)        -- 0..1
sr:SetUseColor(true)
```

### `TextRenderer` (компонент, :base Component)

```lua
local tr = obj:AddComponent_TextRenderer()

tr:SetFont(font)               -- Font
tr:SetText("Hello")
tr:SetFontSize(size)           -- float
tr:SetColor(r, g, b, a)        -- 0..255
tr:SetAlignment(hAlign, vAlign) -- 0..1, 0..1
```

### `Button` (компонент, :base Component)

```lua
local btn = obj:AddComponent_Button()

btn:SetSize(x, y)
btn:SetOnClick(function() … end)
btn:SetOnPointerDown(function() … end)
btn:SetOnPointerUp(function() … end)
btn:SetOnPointerEnter(function() … end)
btn:SetOnPointerExit(function() … end)
```

### `Anchor` (компонент, :base Component)

```lua
local anchor = obj:AddComponent_Anchor()

anchor:SetPreset(0)            -- AnchorPreset (int 0..9)
-- 0=TopLeft, 1=TopCenter, 2=TopRight,
-- 3=MiddleLeft, 4=MiddleCenter, 5=MiddleRight,
-- 6=BottomLeft, 7=BottomCenter, 8=BottomRight,
-- 9=StretchAll
anchor:SetScreenAnchor(sx, sy) -- 0..1
anchor:SetObjectAnchor(ox, oy) -- 0..1
anchor:SetSize(w, h)
```

### `OrthographicCamera`

```lua
local cam = scene:GetGameCamera()

cam:GetLeft()                  -- float
cam:GetRight()
cam:GetBottom()
cam:GetTop()
cam:ScreenToWorldPoint(x, y, w, h)   -- (wx, wy)
cam:ScreenToUIPoint(x, y, w, h)      -- (ux, uy)
```

### `Font`

```lua
local font = LoadFont("assets/fonts/arial.ttf")

-- Низкоуровневый: создать текстуру с заданным текстом/цветом
local tex = font:CreateTextTexture("Hello", size, r, g, b, a)
```

## Match3-специфичные биндинги (в `Match3Game.cpp`)

### `Match3Board`

```lua
local board = Match3Board.new(rows, cols, cellSize, pixelsPerUnit)

board:FillRandom()
board:GetTile(row, col)        -- int (-1 если пусто)
board:SetTile(row, col, type)
board:IsValidCell(row, col)    -- bool
board:HasPossibleMoves()       -- bool
board:Swap(r1, c1, r2, c2)     -- bool
local matches = board:FindMatches()  -- { {r,c}, {r,c}, … } (lua-список пар)
board:RemoveTiles(matches)     -- удалить
board:ApplyGravity()           -- гравитация
board:FillEmpty()              -- заполнить пустоты
board:Mix()                    -- перемешать

board:GetRows()                -- int
board:GetCols()                -- int
board:GetCellPosition(row, col) -- (x, y) lua tuple
board:GetCellSize()            -- float
board:GetPixelsPerUnit()       -- float

-- Callback
board.OnTileChanged = function(row, col, newType) … end
```

## Конвенции вызова

- **Методы** — через `:` (передаётся `self`).
- **Статические/глобальные** — через `.` или без префикса.
- **Свойства** — через `.` (например, `MouseButton.Left`, `board.OnTileChanged`).
- **Конструкторы** — через `ClassName.new(args...)` (sol2 usertype).
- **tuple возвраты** — Lua получает несколько значений, например: `x, y, z = obj:GetPosition()`.

## Полный пример

```lua
Log("Hello world!")

local scene = GetScene()

-- Игровой объект
local player = scene:CreateGameObject("Player")
local t = player:AddComponent_Transform()
t:SetPosition(0, 0, 0)
t:SetScale(2.0, 2.0)

local sr = player:AddComponent_SpriteRenderer()
sr:SetShader(GetShader("Default", vSrc, fSrc))
sr:SetTexture(GetTexture("assets/textures/test.png"))

-- UI-объект
local font = LoadFont("assets/fonts/arial.ttf")
local textObj = scene:CreateUIObject("Title")
local tt = textObj:AddComponent_Transform()
local anchor = textObj:AddComponent_Anchor()
anchor:SetPreset(1)             -- TopCenter
anchor:SetSize(400, 80)

local tr = textObj:AddComponent_TextRenderer()
tr:SetFont(font)
tr:SetText("Hello, UI!")
tr:SetFontSize(48)
tr:SetColor(255, 215, 0, 255)   -- золотой
tr:SetAlignment(0.5, 0.5)

-- Кнопка
local btn = scene:CreateUIObject("Btn")
local bt = btn:AddComponent_Transform()
local ba = btn:AddComponent_Anchor()
ba:SetScreenAnchor(0.5, 0.5)
ba:SetObjectAnchor(0.5, 0.5)
ba:SetSize(200, 50)

local bgr = btn:AddComponent_SpriteRenderer()
bgr:SetShader(GetShader("Default", vSrc, fSrc))
bgr:SetTexture(GetTexture("assets/ui/button.png"))
bgr:SetIsUI(true)
bgr:SetAlignment(0.5, 0.5)

local btr = btn:AddComponent_TextRenderer()
btr:SetFont(font)
btr:SetText("Click me")
btr:SetFontSize(32)
btr:SetAlignment(0.5, 0.5)

local button = btn:AddComponent_Button()
button:SetSize(200, 50)
button:SetOnClick(function() Log("Clicked!") end)

-- Update каждый кадр
function OnUpdate(dt)
    if GetKeyDown(KeyCode.Space) then
        Log("Space pressed")
    end
    local mx, my = GetMousePos()
    local wx, wy = scene:GetGameCamera():ScreenToWorldPoint(mx, my, GetWindowWidth(), GetWindowHeight())
end
```

См. также: [02_Lua_примеры](02_Lua_примеры.md) — реальные скрипты из Sandbox.
