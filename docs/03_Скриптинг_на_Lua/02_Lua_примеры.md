# 02 — Примеры Lua-скриптов

> **Статус:** 🟢 Реальные скрипты из Sandbox

Все примеры взяты (или основаны) на реальных файлах в `SandBox/assets/scripts/`. Пути — `assets/scripts/<имя>.lua` (относительно CWD).

## `game.lua` — игрок + UI текст + UI кнопка

```lua
Log("Script loaded: game.lua")

-- Дефолтный шейдер для спрайтов
local vertexSrc = [=[
#version 330 core
layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;
uniform mat4 u_ViewProjection;
uniform mat4 u_Model;
out vec2 v_TexCoord;
void main() {
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 0.0, 1.0);
    v_TexCoord = a_TexCoord;
}
]=]

local fragmentSrc = [=[
#version 330 core
in vec2 v_TexCoord;
out vec4 FragColor;
uniform sampler2D u_Texture;
void main() {
    FragColor = texture(u_Texture, v_TexCoord);
}
]=]

function OnStart()
    Log("Game script started")
    local scene = GetScene()

    -- === Игровой объект ===
    local player = scene:CreateGameObject("Player")
    local transform = player:AddComponent_Transform()
    transform:SetPosition(0, 0, 0)
    transform:SetScale(2.0, 2.0)

    local renderer = player:AddComponent_SpriteRenderer()
    renderer:SetShader(GetShader("SpriteShader", vertexSrc, fragmentSrc))
    renderer:SetTexture(GetTexture("assets/textures/test.png"))

    -- === UI: текст ===
    local font = LoadFont("assets/fonts/arial.ttf")
    if font then
        local textObj = scene:CreateUIObject("WelcomeText")
        local tt = textObj:AddComponent_Transform()
        local anchor = textObj:AddComponent_Anchor()
        anchor:SetPreset(1)               -- TopCenter
        anchor:SetSize(400, 80)

        local tr = textObj:AddComponent_TextRenderer()
        tr:SetFont(font)
        tr:SetFontSize(48)
        tr:SetText("Hello, UI!")
        tr:SetColor(255, 215, 0, 255)
        tr:SetAlignment(0.5, 0.5)
    end

    -- === UI: кнопка ===
    local btnObj = scene:CreateUIObject("TestButton")
    btnObj:SetZOrder(2)
    local bt = btnObj:AddComponent_Transform()

    local ba = btnObj:AddComponent_Anchor()
    ba:SetScreenAnchor(0.5, 0.5)
    ba:SetObjectAnchor(0.5, 0.5)
    ba:SetPreset(8)                     -- MiddleCenter
    ba:SetSize(200, 50)

    local bgr = btnObj:AddComponent_SpriteRenderer()
    bgr:SetShader(GetShader("SpriteShader", vertexSrc, fragmentSrc))
    bgr:SetTexture(GetTexture("assets/ui/button.png"))
    bgr:SetIsUI(true)
    bgr:SetAlignment(0.5, 0.5)

    local btr = btnObj:AddComponent_TextRenderer()
    if font then
        btr:SetFont(font)
        btr:SetText("Click me")
        btr:SetColor(255, 255, 255, 255)
        btr:SetFontSize(32)
        btr:SetAlignment(0.5, 0.5)
    end

    local button = btnObj:AddComponent_Button()
    button:SetSize(200, 50)
    button:SetOnClick(function() Log("Button clicked!") end)
end

function OnUpdate(dt)
    if GetKey(KeyCode.Space) then
        Log("Space held")
    end
end
```

## `game_match3.lua` — основной скрипт Match3

```lua
Log("Script loaded: game_match3.lua")

-- Шейдеры (идентичны)
local vertexSrc = [=[ … ]=]
local fragmentSrc = [=[ … ]=]

local ROWS = 10
local COLS = 10
local CELL_SIZE = 64
local PPU = 100

-- Текстуры для 6 типов плиток
local tileTextures = {
    CreateSolidColorTexture(255, 0, 0, 255),     -- 0 красный
    CreateSolidColorTexture(0, 255, 0, 255),     -- 1 зелёный
    CreateSolidColorTexture(0, 0, 255, 255),     -- 2 синий
    CreateSolidColorTexture(255, 255, 0, 255),   -- 3 жёлтый
    CreateSolidColorTexture(255, 0, 255, 255),   -- 4 пурпурный
    CreateSolidColorTexture(0, 255, 255, 255)    -- 5 голубой
}
local transparentTexture = CreateSolidColorTexture(0, 0, 0, 0)

local board
local tileObjects = {}
local pressedObj, pressedRow, pressedCol
local gameCamera
local BOARD_OFFSET_X, BOARD_OFFSET_Y

function OnStart()
    local scene = GetScene()
    board = Match3Board.new(ROWS, COLS, CELL_SIZE, PPU)
    gameCamera = scene:GetGameCamera()

    -- Вычислить смещение, чтобы доска была по центру
    local cw = gameCamera:GetRight() - gameCamera:GetLeft()
    local ch = gameCamera:GetTop() - gameCamera:GetBottom()
    local bw = (COLS * CELL_SIZE) / PPU
    local bh = (ROWS * CELL_SIZE) / PPU
    BOARD_OFFSET_X = (cw - bw) * 0.5 + gameCamera:GetLeft()
    BOARD_OFFSET_Y = (ch - bh) * 0.5 + gameCamera:GetBottom()

    -- Callback: при изменении плитки
    board.OnTileChanged = function(row, col, newType)
        local key = row .. "," .. col
        if newType == -1 then
            if tileObjects[key] then
                local sr = tileObjects[key]:GetSpriteRenderer()
                if sr then sr:SetTexture(transparentTexture) end
            end
        else
            local obj
            if not tileObjects[key] then
                obj = scene:CreateGameObject("Tile_" .. key)
                local t = obj:AddComponent_Transform()
                local sr = obj:AddComponent_SpriteRenderer()
                sr:SetShader(GetShader("SpriteShader_final", vertexSrc, fragmentSrc))
                sr:SetIsUI(false)
                tileObjects[key] = obj
            else
                obj = tileObjects[key]
            end

            -- Координаты (визуально: row=0 внизу, row=ROWS-1 вверху)
            local visualRow = (ROWS - 1) - row
            local x, y = board:GetCellPosition(visualRow, col)
            obj:GetTransform():SetPosition(x + BOARD_OFFSET_X, y + BOARD_OFFSET_Y, 0)

            local sr = obj:GetSpriteRenderer()
            local tex = tileTextures[newType+1]
            if tex then sr:SetTexture(tex) end
        end
    end

    Log("--- START BOARD GENERATION ---")
    board:FillRandom()
    if not board:HasPossibleMoves() then
        Log("No possible moves at start, mixing...")
        board:Mix()
    end
    Log("--- END BOARD GENERATION ---")
end

function OnUpdate(dt)
    if not board then return end

    local mx, my = GetMousePosition()
    local ww, wh = GetWindowWidth(), GetWindowHeight()
    local worldPos = gameCamera:ScreenToWorldPoint(mx, my, ww, wh)
    local cellSizeWorld = CELL_SIZE / PPU
    local col = math.floor((worldPos.x - BOARD_OFFSET_X) / cellSizeWorld)
    local row = math.floor((worldPos.y - BOARD_OFFSET_Y) / cellSizeWorld)

    if IsMouseButtonDown(1) then   -- ЛКМ удерживается
        if not pressedObj and board:IsValidCell(row, col) then
            pressedRow, pressedCol = row, col
            local key = row .. "," .. col
            local obj = tileObjects[key]
            if obj then
                pressedObj = obj
                pressedObj:GetTransform():SetScale(1.2, 1.2)   -- визуальный feedback
            end
        end
    else
        -- ЛКМ отпущена
        if pressedObj then
            pressedObj:GetTransform():SetScale(1.0, 1.0)

            if board:IsValidCell(row, col) then
                local dr = math.abs(pressedRow - row)
                local dc = math.abs(pressedCol - col)
                if (dr == 1 and dc == 0) or (dr == 0 and dc == 1) then
                    board:Swap(pressedRow, pressedCol, row, col)

                    local matches = board:FindMatches()
                    if #matches == 0 then
                        board:Swap(pressedRow, pressedCol, row, col)   -- откат
                    else
                        local score = 0
                        score = score + #matches
                        board:RemoveTiles(matches)
                        board:ApplyGravity()
                        board:FillEmpty()

                        -- Каскады
                        local newMatches = board:FindMatches()
                        while #newMatches > 0 do
                            score = score + #newMatches
                            board:RemoveTiles(newMatches)
                            board:ApplyGravity()
                            board:FillEmpty()
                            newMatches = board:FindMatches()
                        end
                        if not board:HasPossibleMoves() then
                            board:Mix()
                        end
                    end
                end
            end

            pressedObj = nil
            pressedRow, pressedCol = nil, nil
        end
    end
end
```

## `ui_test.lua` — минимальный тест UI

```lua
Log("UI test loaded")

function OnStart()
    local scene = GetScene()
    local obj = scene:CreateUIObject("Hello")
    obj:AddComponent_Transform()
    local a = obj:AddComponent_Anchor()
    a:SetPreset(4)              -- MiddleCenter
    a:SetSize(200, 50)
end
```

## Шаблон новой игры

```lua
-- assets/scripts/my_game.lua
Log("My game loaded")

local vertexSrc = [=[#version 330 core
layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;
uniform mat4 u_ViewProjection;
uniform mat4 u_Model;
out vec2 v_TexCoord;
void main() { gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 0.0, 1.0); v_TexCoord = a_TexCoord; }
]=]

local fragmentSrc = [=[#version 330 core
in vec2 v_TexCoord;
out vec4 FragColor;
uniform sampler2D u_Texture;
void main() { FragColor = texture(u_Texture, v_TexCoord); }
]=]

-- Состояние игры
local state = { score = 0, level = 1 }

-- Создание объектов (один раз)
function OnStart()
    local scene = GetScene()
    -- ...
end

-- Каждый кадр
function OnUpdate(dt)
    -- ...
end

-- Очистка (необязательно)
function OnShutdown()
    Log("My game shutting down")
end
```

В `Match3Game.cpp::Match3Game` указать путь:
```cpp
m_ScriptPath = "assets/scripts/my_game.lua";
```

См. также: [01_Lua_API_справочник](01_Lua_API_справочник.md), [04_Sandbox_и_Match3/02_Игра_Match3](../04_Sandbox_и_Match3/02_Игра_Match3.md).
