-- Simple test script for Luau3D engine

print("Script started")

-- Initial setup
local time = 0
local colors = {
    {1, 0, 0, 1},  -- Red
    {0, 1, 0, 1},  -- Green
    {0, 0, 1, 1},  -- Blue
}
local currentColor = 1
local colorChangeInterval = 1.0  -- Change color every second

print("Entering main loop")

-- Main game loop
while isRunning() do
    -- Get the time elapsed since last frame
    local dt = getDeltaTime()
    time = time + dt
    
    -- Change color every second
    if time >= colorChangeInterval then
        time = time - colorChangeInterval
        currentColor = (currentColor % #colors) + 1
        local color = colors[currentColor]
        print("Setting color:", color[1], color[2], color[3], color[4])
        setClearColor(color[1], color[2], color[3], color[4])
    end
    
    -- Present the frame
    present()
end

print("Script ended") 