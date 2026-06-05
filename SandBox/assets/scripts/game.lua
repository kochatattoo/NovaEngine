Log("Script loaded: game.lua")

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
    Log("Scene obtained")

    -- Player
    local player = scene:CreateGameObject("Player")
    Log("Player created")
    local transform = player:AddComponent_Transform()
    transform:SetPosition(0, 0, 0)
    transform:SetScale(2.0, 2.0)
    Log("Transform added")
    local renderer = player:AddComponent_SpriteRenderer()
    Log("SpriteRenderer added")
    local shader = GetShader("SpriteShader", vertexSrc, fragmentSrc)
    Log("Shader obtained: " .. tostring(shader))
    local texture = GetTexture("assets/textures/test.png")
    Log("Texture obtained: " .. tostring(texture))
    renderer:SetShader(shader)
    renderer:SetTexture(texture)
    Log("Shader and Texture set")

      -- UI: текст
    local font = LoadFont("assets/fonts/arial.ttf")  -- убедись, что шрифт лежит здесь
     Log("Font loaded: " .. tostring(font))
    if font then
        local textObj = scene:CreateUIObject("WelcomeText")

        local tt = textObj:AddComponent_Transform()

        local anchor = textObj:AddComponent_Anchor()
        anchor:SetPreset(1)
        anchor:SetSize(200, 50)

        local tr = textObj:AddComponent_TextRenderer()
        tr:SetFont(font)
        tr:SetFontSize(64)
        tr:SetText("Hello, UI!")
        tr:SetColor(255, 215, 0, 255) -- золотой
        tr:SetAlignment(0.4, 0.5)
    else
        Log("Failed to load font")
    end

     -- UI: кнопка
    local btnObj = scene:CreateUIObject("TestButton")
    btnObj:SetZOrder(2)  -- ниже текста
    local bt = btnObj:AddComponent_Transform()

    local anchor = btnObj:AddComponent_Anchor()
    anchor:SetScreenAnchor(0.5, 0.5) -- центр экрана
    anchor:SetObjectAnchor(0.5, 0.5) -- центр объекта
    anchor:SetPreset(8)
    anchor:SetSize(200, 50)

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
        btr:SetFontSize(42)   -- добавьте эту строку, чтобы текст был читаемым
        btr:SetAlignment(0.65, 0.6)
    end

    local button = btnObj:AddComponent_Button()
    button:SetSize(200, 50)
    button:SetCallback(function() Log("Button clicked!") end)

        Log("Scene setup finished")
    end

function OnUpdate(dt)
    if IsKeyDown(32) then -- пробел
        Log("Space pressed in Lua")
    end
end