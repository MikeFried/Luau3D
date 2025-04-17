-- Internal binary implementation of Luau3D conforms to this type mapping
export type LightProperties = {
    position: {number}?, -- {x, y, z} or {x, y, z, w} for directional/point light
    ambient: {number}?,  -- {r, g, b} or {r, g, b, a}
    diffuse: {number}?,  -- {r, g, b} or {r, g, b, a}
    specular: {number}?, -- {r, g, b} or {r, g, b, a}
    spotDirection: {number}?, -- {x, y, z}
    spotExponent: number?, -- 0-128
    spotCutoff: number?, -- 0-90
    constantAttenuation: number?, -- 0-1
    linearAttenuation: number?, -- 0-1
    quadraticAttenuation: number?, -- 0-1
}

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
    setClearColor: (r: number, g: number, b: number, a: number) -> boolean,
    -- Sets the light properties for the next frame
    setLight: (lightNumber: number, properties: LightProperties) -> boolean,
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

-- Sets the light properties for the next frame
luau3d.setLight = function(lightNumber, properties)
    -- This is a placeholder for the actual implementation provided by the engine
    return true
end

return luau3d :: Luau3D
