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
uniform vec4 u_Color;
void main() {
    FragColor = texture(u_Texture, v_TexCoord) * u_Color;
}
]=]

-- Параметры доски
local ROWS = 8
local COLS = 8
local CELL_SIZE = 64
local BOARD_OFFSET_X = 1.0   -- смещение в мировых единицах (небольшое, чтобы отступить от края)
local BOARD_OFFSET_Y = 1.0

-- Цвета для типов плиток (0..5)
local TILE_COLORS = {
    {1,0,0,1}, -- красный
    {0,1,0,1}, -- зелёный
    {0,0,1,1}, -- синий
    {1,1,0,1}, -- жёлтый
    {1,0,1,1}, -- фиолетовый
    {0,1,1,1}  -- циан
}

function OnStart()
    local scene = GetScene()
    board = Match3Board.new(ROWS, COLS, CELL_SIZE, 100)

       -- Вычисляем центрированные смещения ДО того, как использовать их в коллбеке
    local gameCamera = scene:GetGameCamera()
    local camLeft = gameCamera:GetLeft()
    local camRight = gameCamera:GetRight()
    local camBottom = gameCamera:GetBottom()
    local camTop = gameCamera:GetTop()

    local boardWidthWorld = (COLS * CELL_SIZE) / 100
    local boardHeightWorld = (ROWS * CELL_SIZE) / 100

    BOARD_OFFSET_X = (camLeft + camRight - boardWidthWorld) * 0.5
    BOARD_OFFSET_Y = (camBottom + camTop - boardHeightWorld) * 0.5

    local tileTexture = GetTexture("assets/textures/cell.png")

    local tileObjects = {}
    board.OnTileChanged = function(row, col, newType)
        local key = row .. "," .. col
        if newType == -1 then
            if tileObjects[key] then
                -- удаление из сцены пока не поддерживается, просто делаем невидимым
                local sr = tileObjects[key]:GetSpriteRenderer()
                if sr then sr:SetUseColor(false); sr:SetTexture(nil) end
            end
        else
            local obj
            if not tileObjects[key] then
                -- Создать новый объект
                obj = scene:CreateGameObject("Tile_" .. key)
                local t = obj:AddComponent_Transform()
                local x, y = board:GetCellPosition(row, col)
                t:SetPosition(x + BOARD_OFFSET_X, y + BOARD_OFFSET_Y, 0)
                local sr = obj:AddComponent_SpriteRenderer()
                sr:SetShader(GetShader("SpriteShader", vertexSrc, fragmentSrc))  -- нужен vertexSrc/fragmentSrc определённый выше в скрипте
                sr:SetIsUI(false)
                tileObjects[key] = obj
            end
            local obj = tileObjects[key]
            local sr = obj:GetSpriteRenderer()
            sr:SetTexture(tileTexture)
            local color = TILE_COLORS[newType + 1]
            sr:SetColor(color[1], color[2], color[3], color[4])
            sr:SetUseColor(true)
        end
    end

    -- Заполняем доску
    board:FillRandom()

    -- Обработка кликов (пока заглушка)
end