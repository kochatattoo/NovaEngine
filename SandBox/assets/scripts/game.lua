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
    local player = scene:CreateGameObject("Player")
    local transform = player:AddComponent_Transform()
    transform:SetPosition(0, 0, 0)
    local renderer = player:AddComponent_SpriteRenderer()
    local shader = GetShader("SpriteShader", vertexSrc, fragmentSrc)
    local texture = GetTexture("assets/textures/test.png")
    renderer:SetShader(shader)
    renderer:SetTexture(texture)
end

function OnUpdate(dt)
    if IsKeyDown(32) then -- пробел
        Log("Space pressed in Lua")
    end
end