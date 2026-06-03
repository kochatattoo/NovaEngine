function OnStart()
    Log("Hello from Lua!")
    SetClearColor(1.0, 0.0, 0.0, 1.0) -- красный фон
end

function OnUpdate(dt)
    if IsKeyDown(32) then -- пробел
        Log("Space pressed in Lua")
    end
end