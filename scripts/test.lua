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
local time = 0 -- seconds of simulation time total
local colors = {
    {0, 0, 0, 1},  -- Black
    {1, 0, 0, 1},  -- Red
    {0, 1, 0, 1},  -- Green
    {0, 0, 1, 1},  -- Blue
}
local currentColor = 1
local colorChangeInterval = 1.0  -- Change color every second

-- Set the initial color
local color = colors[currentColor]
luau3d.setClearColor( table.unpack(color) )

print("Entering main loop")

-- Set up main light
luau3d.setLight(1, {
    position = {2.0, 2.0, 2.0, 1.0},  -- w=1 for positional light
    ambient = {0.2, 0.2, 0.2},
    diffuse = {1.0, 1.0, 1.0},
    specular = {1.0, 1.0, 1.0}
})

-- Add a red spotlight
luau3d.setLight(2, {
    position = {-1.0, 2.0, 1.0, 1.0},
    diffuse = {1.0, 0.0, 0.0},
    spotDirection = {1.0, -2.0, -1.0},
    spotCutoff = 45.0,
    spotExponent = 2.0
})

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
        --print("Setting color:", color[1], color[2], color[3], color[4])
        luau3d.setClearColor(color[1], color[2], color[3], color[4])
    end
    
    -- Draw the cube
    luau3d.drawGeometry(cube)
    
    -- Present the frame
    luau3d.present()
end

print("Script ended") 