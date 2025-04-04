-- Simple test script for Luau3D engine

print("Script started")

-- Load required modules
local geometry = require("geometry.luau")
local luau3d = require("luau3d.luau")
print("Geometry module loaded")

-- Create a cube
local cube = geometry.createCube()
--print("Cube created with", #cube, "vertices")

-- Initial setup
local time = 0
local colors = {
    {1, 0, 0, 1},  -- Red
    {0, 1, 0, 1},  -- Green
    {0, 0, 1, 1},  -- Blue
}
local currentColor = 1
local colorChangeInterval = 1.0  -- Change color every second

-- Set the initial color
local color = colors[currentColor]
luau3d.setClearColor(color[1], color[2], color[3], color[4])

print("Entering main loop")

-- Main game loop
while luau3d.isRunning() do
    -- Get the time elapsed since last frame
    local dt = luau3d.getDeltaTime()
    time = time + dt
    
    -- Change color every second
    if time >= colorChangeInterval then
        time = time - colorChangeInterval
        currentColor = (currentColor % #colors) + 1
        color = colors[currentColor]
        print("Setting color:", color[1], color[2], color[3], color[4])
        luau3d.setClearColor(color[1], color[2], color[3], color[4])
    end
    
    -- Draw the cube
    luau3d.drawGeometry(cube)
    
    -- Present the frame
    luau3d.present()
end

print("Script ended") 