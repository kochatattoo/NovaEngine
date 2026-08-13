-- game_match3.lua (v0.2.6 — только ввод, рендер через SpriteRenderSystem)
Log("Script loaded: game_match3.lua (v0.2.6 — ECS render)")

local ROWS = 10
local COLS = 10
local CELL_SIZE = 64
local PPU = 100

local board
local pressedRow, pressedCol
local gameCamera
local BOARD_OFFSET_X, BOARD_OFFSET_Y
local score = 0

function SafeOnStart()
    local ok, err = pcall(OnStartInner)
    if not ok then
        Log("Match3: OnStart FAILED: " .. tostring(err))
    end
end

function OnStartInner()
    Log("Match3: OnStart begin (v0.2.6 — рендер из ECS)")
    local scene = GetScene()
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

    -- v0.2.6: OnTileChanged больше не нужен — SpriteRenderSystem рисует все плитки из ECS.
    -- Lua отвечает только за ввод.

    Log("Match3: START BOARD GENERATION")
    board:FillRandom()
    if not board:HasPossibleMoves() then
        Log("Match3: No possible moves at start, mixing...")
        board:Mix()
    end
    Log("Match3: END BOARD GENERATION")
end

function OnUpdate(dt)
    if not board or not gameCamera then return end

    local mx, my = GetMousePos()
    local ww, wh = GetWindowWidth(), GetWindowHeight()
    local worldPos = gameCamera:ScreenToWorldPoint(mx, my, ww, wh)
    local cellSizeWorld = CELL_SIZE / PPU
    local col = math.floor((worldPos.x - BOARD_OFFSET_X) / cellSizeWorld)
    local row = math.floor((worldPos.y - BOARD_OFFSET_Y) / cellSizeWorld)

    if GetMouseButton(MouseButton.Left) then
        if not pressedRow and board:IsValidCell(row, col) then
            pressedRow, pressedCol = row, col
        end
    else
        if pressedRow then
            if board:IsValidCell(row, col) then
                local dr = math.abs(pressedRow - row)
                local dc = math.abs(pressedCol - col)
                if (dr == 1 and dc == 0) or (dr == 0 and dc == 1) then
                    board:Swap(pressedRow, pressedCol, row, col)
                    local matches = board:FindMatches()
                    if #matches == 0 then
                        board:Swap(pressedRow, pressedCol, row, col)
                    else
                        score = score + #matches
                        board:RemoveTiles(matches)
                        board:ApplyGravity()
                        board:FillEmpty()
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
            pressedRow, pressedCol = nil, nil
        end
    end
end
