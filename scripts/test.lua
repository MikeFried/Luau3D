-- Simple test script for Luau3D engine

print("Script started")

-- Load required modules
local model = require("model.luau")
local luau3d = require("luau3d.luau")
print("Model module loaded")

-- Create a cube with initial CFrame
local cube = model.createCube(0.5, {
    position = {0, 0, -3},  -- Move cube back for better viewing
    look = {0, 0, -1},
    up = {0, 1, 0},
    right = {1, 0, 0}
})
print("Cube created")

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
luau3d.setClearColor(table.unpack(color))

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

-- Add the cube model
local cubeIndex = luau3d.addModel(cube)
print("Cube added with index", cubeIndex)

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
        luau3d.setClearColor(table.unpack(color))
    end
    
    -- Update cube's CFrame to make it rotate
    local angle = time * math.pi * 0.5  -- Rotate 90 degrees per second
    local cosAngle = math.cos(angle)
    local sinAngle = math.sin(angle)
    
    luau3d.updateModel(cubeIndex, {
        vertices = cube.vertices,
        visible = true,
        cframe = {
            position = {0, 0, -3},  -- Keep the same position
            look = {sinAngle, 0, -cosAngle},
            up = {0, 1, 0},
            right = {cosAngle, 0, sinAngle}
        }
    })
    
    -- Present the frame
    luau3d.present()
end

print("Script ended") 