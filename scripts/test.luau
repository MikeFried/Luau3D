-- Simple test script for Luau3D engine

-- Load required modules
local model = require("model.luau")
local luau3d = require("luau3d.luau")
local gui = require("gui.luau")

-- Create a cube with initial CFrame
local cube = model.createCube(0.5, {
    position = {0, 0, -3},  -- Move cube back for better viewing
    look = {0, 0, -1},
    up = {0, 1, 0},
    right = {1, 0, 0}
})

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

local cubePosition = {0, 0, -3}
local keysPressed = {}
gui.registerKeyboardCallback(function(key: string, action: gui.KeyboardAction)
    if action == "press" then keysPressed[key] = true end
    if action == "release" then keysPressed[key] = nil end
end)

-- Moves the cube by moveAmt units per second
local moveAmt = 2
local keyMoveRates = {
    w = {0, moveAmt, 0},
    a = {-moveAmt, 0, 0},
    s = {0, -moveAmt, 0},
    d = {moveAmt, 0, 0},
    q = {0, 0, -moveAmt},
    e = {0, 0, moveAmt},
}

local function fmaVec3(vec1: {number}, vec2: {number}, scalar: number)
    return {
        vec1[1] + vec2[1] * scalar,
        vec1[2] + vec2[2] * scalar,
        vec1[3] + vec2[3] * scalar,
    }
end

local changeBackground = false
local angle = 0
local xAngle = 0  -- New angle for X-axis rotation
local zAngle = 0  -- New angle for Z-axis rotation

-- Main game loop
function beforeRender()
    -- Get the time elapsed since last frame
    local dt = luau3d.getDeltaTime()
    --print("dt: " .. tostring(dt))
    time = time + dt
    
    -- Change color every second
    if time >= colorChangeInterval and changeBackground then
        time = time - colorChangeInterval
        currentColor = (currentColor % #colors) + 1
        color = colors[currentColor]
        luau3d.setClearColor(table.unpack(color))
    end
    
    -- Update cube's position
    for key, vec in pairs(keyMoveRates) do
        if keysPressed[key] then
            cubePosition = fmaVec3(cubePosition, vec, dt)
        end
    end

    -- Update cube's CFrame to make it rotate
    if keysPressed["-"] then
        angle = angle + dt * math.pi * 0.5  -- Rotate 90 degrees per second
    end
    if keysPressed["="] then
        angle = angle - dt * math.pi * 0.5  -- Rotate 90 degrees per second
    end
    if keysPressed["["] then
        xAngle = xAngle + dt * math.pi * 0.5  -- Rotate 90 degrees per second
    end
    if keysPressed["]"] then
        xAngle = xAngle - dt * math.pi * 0.5  -- Rotate 90 degrees per second
    end
    if keysPressed[";"] then
        zAngle = zAngle + dt * math.pi * 0.5  -- Rotate 90 degrees per second
    end
    if keysPressed["'"] then
        zAngle = zAngle - dt * math.pi * 0.5  -- Rotate 90 degrees per second
    end
    
    local cosAngle = math.cos(angle)
    local sinAngle = math.sin(angle)
    local cosXAngle = math.cos(xAngle)
    local sinXAngle = math.sin(xAngle)
    local cosZAngle = math.cos(zAngle)
    local sinZAngle = math.sin(zAngle)
    
    -- Calculate the rotated basis vectors
    local look = {
        sinAngle * cosXAngle * cosZAngle - sinXAngle * sinZAngle,
        sinXAngle * cosZAngle + sinAngle * cosXAngle * sinZAngle,
        -cosAngle * cosXAngle
    }
    
    local up = {
        -sinAngle * sinXAngle * cosZAngle - cosXAngle * sinZAngle,
        cosXAngle * cosZAngle - sinAngle * sinXAngle * sinZAngle,
        cosAngle * sinXAngle
    }
    
    local right = {
        cosAngle * cosZAngle,
        cosAngle * sinZAngle,
        sinAngle
    }
    
    luau3d.updateModel(cubeIndex, {
        vertices = cube.vertices,
        visible = true,
        cframe = {
            position = cubePosition,
            look = look,
            up = up,
            right = right
        }
    })
end

-- Register the beforeRender callback
luau3d.registerBeforeRenderCallback(beforeRender)
