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
    local player = scene:CreateGameObject("Player")
    Log("Player created")
    local transform = player:AddComponent_Transform()
    transform:SetPosition(0, 0, 0)
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
        Log("Font is valid")
        local textObj = scene:CreateUIObject("WelcomeText")
        Log("Text object created")
        local tt = textObj:AddComponent_Transform()
        tt:SetPosition(50, 50, 0)
        local tr = textObj:AddComponent_TextRenderer()
        Log("TextRenderer added")
        tr:SetFont(font)
        Log("Font set")
        tr:SetText("Hello, UI!")
        Log("Text set")
        tr:SetColor(255, 215, 0, 255) -- золотой
        Log("Color set")
    else
        Log("Failed to load font")
    end

     -- UI: кнопка
    local btnObj = scene:CreateUIObject("TestButton")
    Log("Button object created")
    local bt = btnObj:AddComponent_Transform()
    bt:SetPosition(200, 200, 0)
    local bgr = btnObj:AddComponent_SpriteRenderer()
    -- чтобы увидеть кнопку даже без текстуры фона, зададим ей цветной прямоугольник? Но SpriteRenderer не умеет цвет без текстуры.
    -- Поэтому временно зададим любую существующую текстуру, например test.png
    bgr:SetTexture(GetTexture("assets/textures/test.png"))
    Log("Button background set")
    local btr = btnObj:AddComponent_TextRenderer()
    if font then
        btr:SetFont(font)
        btr:SetText("Click me")
        btr:SetColor(255, 255, 255, 255)
        Log("Button text set")
    else
        Log("Font not available for button text")
    end
    local button = btnObj:AddComponent_Button()
    button:SetSize(200, 50)
    button:SetCallback(function() Log("Button clicked!") end)
    Log("Button setup done")

    Log("Scene setup finished")
end

function OnUpdate(dt)
    if IsKeyDown(32) then -- пробел
        Log("Space pressed in Lua")
    end
end