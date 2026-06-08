-- game_match3.lua
Log("Script loaded: game_match3.lua")

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

-- Массив цветных текстур (по одной на каждый тип)
local tileTextures = {
    CreateSolidColorTexture(255, 0, 0, 255),   -- 0 красный
    CreateSolidColorTexture(0, 255, 0, 255),   -- 1 зелёный
    CreateSolidColorTexture(0, 0, 255, 255),   -- 2 синий
    CreateSolidColorTexture(255, 255, 0, 255), -- 3 жёлтый
    CreateSolidColorTexture(255, 0, 255, 255), -- 4 фиолетовый
    CreateSolidColorTexture(0, 255, 255, 255)  -- 5 циан
}

-- Полностью прозрачная текстура для удалённых плиток
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
    local camLeft = gameCamera:GetLeft()
    local camRight = gameCamera:GetRight()
    local camBottom = gameCamera:GetBottom()
    local camTop = gameCamera:GetTop()
    local boardWidthWorld = (COLS * CELL_SIZE) / PPU
    local boardHeightWorld = (ROWS * CELL_SIZE) / PPU
    BOARD_OFFSET_X = (camLeft + camRight - boardWidthWorld) * 0.5
    BOARD_OFFSET_Y = (camBottom + camTop - boardHeightWorld) * 0.5

    -- Коллбек, вызываемый при изменении клетки
    board.OnTileChanged = function(row, col, newType)
        local key = row .. "," .. col
        if newType == -1 then
            -- Плитка удалена: ставим прозрачную текстуру
            if tileObjects[key] then
                local sr = tileObjects[key]:GetSpriteRenderer()
                if sr then
                    sr:SetTexture(transparentTexture)
                end
            end
        else
            local obj
            if not tileObjects[key] then
                -- Создаём новый объект, если его нет
                obj = scene:CreateGameObject("Tile_" .. key)
                local t = obj:AddComponent_Transform()
                local sr = obj:AddComponent_SpriteRenderer()
                sr:SetShader(GetShader("SpriteShader_final", vertexSrc, fragmentSrc))
                sr:SetIsUI(false)
                tileObjects[key] = obj
            else
                obj = tileObjects[key]
            end

            -- Обновляем позицию (важно для Swap и гравитации)
            local visualRow = (ROWS - 1) - row
            local x, y = board:GetCellPosition(visualRow, col)

            obj:GetTransform():SetPosition(x + BOARD_OFFSET_X, y + BOARD_OFFSET_Y, 0)

            -- Назначаем правильную цветную текстуру по типу
            local sr = obj:GetSpriteRenderer()
            local tex = tileTextures[newType+1]
            Log("newType = " .. newType)
            if tex then
                sr:SetTexture(tex)
                Log("[Callback] Cells ["..key.."] -> C++ type: "..newType.." | texture set #"..(newType+1))

                local actualType = board:GetTile(row, col)
                Log("[FINAL] Actual type from board: " ..actualType)
            else
                Log("[Warning] No texture for type "..newType)
            end
        end
    end

    -- Заполняем доску
    Log("--- START BOARD GENERATION ---")
    board:FillRandom()
        if not board:HasPossibleMoves() then
        Log("No possible moves at start, mixing...")
        board:Mix()
        Log("Board mixed")
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

    if IsMouseButtonDown(1) then
        -- Кнопка нажата
        if not pressedObj and board:IsValidCell(row, col) then
            pressedRow, pressedCol = row, col
            local tileType = board:GetTile(row, col)
            Log("Pressed tile at "..row..","..col.." type="..tileType)
            local key = row .. "," .. col
            local obj = tileObjects[key]
            if obj then
                pressedObj = obj
                pressedObj:GetTransform():SetScale(1.2, 1.2)  -- выделение
            end
        end
    else
        -- Кнопка отпущена
        if pressedObj then
            -- Снимаем выделение с начальной ячейки
            pressedObj:GetTransform():SetScale(1.0, 1.0)

            -- Проверяем, куда пришлось отпускание
            if board:IsValidCell(row, col) then
                local dr = math.abs(pressedRow - row)
                local dc = math.abs(pressedCol - col)
                -- Обмен, только если соседняя ячейка
                if (dr == 1 and dc == 0) or (dr == 0 and dc == 1) then
                    board:Swap(pressedRow, pressedCol, row, col)
                    Log("Swapped "..pressedRow..","..pressedCol.." with "..row..","..col)

                    local matches = board:FindMatches()
                    Log("Matches found: " .. #matches)
                    if #matches == 0 then
                        -- Неудачный обмен — возвращаем обратно
                        board:Swap(pressedRow, pressedCol, row, col)
                    else
                        -- Удаляем совпадения, применяем гравитацию и заполняем пустоты
                        board:RemoveTiles(matches)
                        board:ApplyGravity()
                        board:FillEmpty()
                        -- Цепная реакция
                        local newMatches = board:FindMatches()
                        while #newMatches > 0 do
                            board:RemoveTiles(newMatches)
                            board:ApplyGravity()
                            board:FillEmpty()
                            newMatches = board:FindMatches()
                        end
                        if not board:HasPossibleMoves() then
                             Log("No possible moves, mixing...")
                             board:Mix()
                        end
                    end
                end
            end

            -- Сбрасываем состояние
            pressedObj = nil
            pressedRow, pressedCol = nil, nil
        end
    end
end