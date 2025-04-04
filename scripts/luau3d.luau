-- Internal binary implementation of Luau3D conforms to this type mapping
export type Luau3D = {
    -- Returns true if the engine is running
    isRunning: () -> boolean,
    -- Returns the time since the last frame in seconds
    getDeltaTime: () -> number,
    -- Presents the current frame
    present: () -> boolean,
    -- Draws the geometry
    drawGeometry: (geometry: {}) -> boolean,
    -- Sets the clear color for the next frame
    setClearColor: (r: number, g: number, b: number, a: number) -> boolean
}

local luau3d = {}

-- Returns true if the engine is running
luau3d.isRunning = function()
    -- This is a placeholder for the actual implementation provided by the engine
    return true
end

-- Returns the time since the last frame in seconds
luau3d.getDeltaTime = function()
    -- This is a placeholder for the actual implementation provided by the engine
    return 0
end

-- Presents the current frame
luau3d.present = function()
    -- This is a placeholder for the actual implementation provided by the engine
    return true
end

-- Draws the geometry
luau3d.drawGeometry = function(geometry)
    -- This is a placeholder for the actual implementation provided by the engine
    return true
end

-- Sets the clear color for the next frame
luau3d.setClearColor = function(r, g, b, a)
    -- This is a placeholder for the actual implementation provided by the engine
    return true
end

return luau3d :: Luau3D
