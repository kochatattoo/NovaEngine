function OnStart()
    local scene = GetScene()
    local font = LoadFont("assets/fonts/arial.ttf")

    -- Текст
    local textObj = scene:CreateUIObject("WelcomeText")
    local t = textObj:AddComponent_Transform()
    t:SetPosition(100, 100, 0)
    local tr = textObj:AddComponent_TextRenderer()
    tr:SetFont(font)
    tr:SetText("Hello, NovaEngine UI!")
    tr:SetColor(255, 215, 0, 255) -- золотой

    -- Кнопка
    local btnObj = scene:CreateUIObject("TestButton")
    local bt = btnObj:AddComponent_Transform()
    bt:SetPosition(200, 200, 0)
    local sr = btnObj:AddComponent_SpriteRenderer()
    sr:SetTexture(GetTexture("assets/ui/button.png")) -- нужна текстура фона
    local btr = btnObj:AddComponent_TextRenderer()
    btr:SetFont(font)
    btr:SetText("Click me")
    btr:SetColor(255,255,255,255)
    local button = btnObj:AddComponent_Button()
    button:SetSize(200, 50)
    button:SetCallback(function() Log("Button clicked!") end)
end