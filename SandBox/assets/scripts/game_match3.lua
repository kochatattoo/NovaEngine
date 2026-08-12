-- game_match3.lua
Log("Script loaded: game_match3.lua (v0.1.3 — fixed input)")

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

local ROWS = 10
local COLS = 10
local CELL_SIZE = 64
local PPU = 100

-- Текстуры плиток (по одной на цвет)
local tileTextures = {
    CreateSolidColorTexture(255, 0, 0, 255),   -- 0 красный
    CreateSolidColorTexture(0, 255, 0, 255),   -- 1 зеленый
    CreateSolidColorTexture(0, 0, 255, 255),   -- 2 синий
    CreateSolidColorTexture(255, 255, 0, 255), -- 3 желтый
    CreateSolidColorTexture(255, 0, 255, 255), -- 4 пурпурный
    CreateSolidColorTexture(0, 255, 255, 255)  -- 5 голубой
}

-- Прозрачная текстура для удаленных плиток
local transparentTexture = CreateSolidColorTexture(0, 0, 0, 0)

local board
local tileObjects = {}
local pressedObj, pressedRow, pressedCol
local gameCamera
local BOARD_OFFSET_X, BOARD_OFFSET_Y
local score = 0

-- v0.2: оборачиваем OnStart в pcall, чтобы ошибка (например, в OnTileChanged setter)
-- не валила весь скрипт и не ломала OnUpdate.
function SafeOnStart()
    local ok, err = pcall(OnStartInner)
    if not ok then
        Log("Match3: OnStart FAILED: " .. tostring(err))
    end
end

function OnStartInner()
    Log("Match3: OnStart begin (v0.2 — ECS-backed Match3System)")
    local scene = GetScene()

    -- v0.2: Match3System создаётся в C++ (Match3Game::Start), здесь получаем указатель.
    -- Внутри C++ он забинден под именем Match3Board для обратной совместимости API.
    board = GetBoard()

    gameCamera = scene:GetGameCamera()
    local camLeft = gameCamera:GetLeft()
    local camRight = gameCamera:GetRight()
    local camBottom = gameCamera:GetBottom()
    local camTop = gameCamera:GetTop()
    local boardWidthWorld = (COLS * CELL_SIZE) / PPU
    local boardHeightWorld = (ROWS * CELL_SIZE) / PPU
    BOARD_OFFSET_X = (camLeft + camRight - boardWidthWorld) * 0.5
    BOARD_OFFSET_Y = (camBottom + camTop - boardHeightWorld) * 0.5

    Log(string.format("Match3: camera=(%f..%f, %f..%f), boardOffset=(%f, %f)",
        camLeft, camRight, camBottom, camTop, BOARD_OFFSET_X, BOARD_OFFSET_Y))

    -- Callback: вызывается при изменении плитки
    board.OnTileChanged = function(row, col, newType)
        local key = row .. "," .. col
        if newType == -1 then
            -- Пустая клетка: ставим прозрачную текстуру
            if tileObjects[key] then
                local sr = tileObjects[key]:GetSpriteRenderer()
                if sr then
                    sr:SetTexture(transparentTexture)
                end
            end
        else
            local obj
            if not tileObjects[key] then
                -- Создаем новый объект, если его еще нет
                obj = scene:CreateGameObject("Tile_" .. key)
                local t = obj:AddComponent_Transform()
                local sr = obj:AddComponent_SpriteRenderer()
                sr:SetShader(GetShader("SpriteShader_final", vertexSrc, fragmentSrc))
                sr:SetIsUI(false)
                -- v0.1.3: задаем размер плитки. Без этого renderW = texW/ppu = 1/100 = 0.01
                -- мировых единиц = 1 пиксель — плитка не видна.
                sr:SetCustomSize(CELL_SIZE / PPU, CELL_SIZE / PPU)
                sr:SetPixelsPerUnit(PPU)
                tileObjects[key] = obj
            else
                obj = tileObjects[key]
            end

            -- Вычисляем позицию (визуально: row=0 внизу, row=ROWS-1 вверху)
            local visualRow = (ROWS - 1) - row
            local x, y = board:GetCellPosition(visualRow, col)

            obj:GetTransform():SetPosition(x + BOARD_OFFSET_X, y + BOARD_OFFSET_Y, 0)

            local sr = obj:GetSpriteRenderer()
            local tex = tileTextures[newType+1]
            if tex then
                sr:SetTexture(tex)
            end
        end
    end

    -- Заполняем поле
    Log("Match3: START BOARD GENERATION")
    board:FillRandom()
    if not board:HasPossibleMoves() then
        Log("Match3: No possible moves at start, mixing...")
        board:Mix()
    end
    Log("Match3: END BOARD GENERATION")
end

function OnUpdate(dt)
    if not board then return end

    -- v0.1.3: используем GetMousePos (клиентские координаты) и GetMouseButton (новый API)
    local mx, my = GetMousePos()
    local ww, wh = GetWindowWidth(), GetWindowHeight()
    local worldPos = gameCamera:ScreenToWorldPoint(mx, my, ww, wh)
    local cellSizeWorld = CELL_SIZE / PPU
    local col = math.floor((worldPos.x - BOARD_OFFSET_X) / cellSizeWorld)
    local row = math.floor((worldPos.y - BOARD_OFFSET_Y) / cellSizeWorld)

    if GetMouseButton(MouseButton.Left) then
        -- Кнопка нажата
        if not pressedObj and board:IsValidCell(row, col) then
            pressedRow, pressedCol = row, col
            local key = row .. "," .. col
            local obj = tileObjects[key]
            if obj then
                pressedObj = obj
                pressedObj:GetTransform():SetScale(1.2, 1.2)  -- визуальный feedback
            end
        end
    else
        -- Кнопка отпущена
        if pressedObj then
            -- Убираем выделение и проверяем swap
            pressedObj:GetTransform():SetScale(1.0, 1.0)

            -- Проверяем, куда отпустили
            if board:IsValidCell(row, col) then
                local dr = math.abs(pressedRow - row)
                local dc = math.abs(pressedCol - col)
                -- Swap, только если это соседняя клетка
                if (dr == 1 and dc == 0) or (dr == 0 and dc == 1) then
                    board:Swap(pressedRow, pressedCol, row, col)

                    local matches = board:FindMatches()
                    if #matches == 0 then
                        -- Не получилось — откатываем swap
                        board:Swap(pressedRow, pressedCol, row, col)
                    else
                        -- Нашли матч
                        score = score + #matches

                        -- Удаляем совпадения, применяем гравитацию и заполняем пустоты
                        board:RemoveTiles(matches)
                        board:ApplyGravity()
                        board:FillEmpty()

                        -- Каскад падения
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

            -- Сбрасываем выделение
            pressedObj = nil
            pressedRow, pressedCol = nil, nil
        end
    end
end
