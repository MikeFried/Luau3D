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

export type CFrame = {
    position: {number}, -- {x, y, z}
    look: {number},    -- {x, y, z}
    up: {number},      -- {x, y, z}
    right: {number},   -- {x, y, z}
}

export type ModelProperties = {
    vertices: {number},
    visible: boolean?,
    cframe: CFrame?,
}

export type Luau3D = {
    -- Returns true if the engine is running
    isRunning: () -> boolean,
    -- Returns the time since the last frame in seconds
    getDeltaTime: () -> number,
    -- Adds a new model and returns its index
    addModel: (properties: ModelProperties) -> number,
    -- Removes a model by index
    removeModel: (index: number) -> boolean,
    -- Removes all models
    clearModels: () -> boolean,
    -- Sets the visibility of a model
    setModelVisible: (index: number, visible: boolean) -> boolean,
    -- Updates a model's properties
    updateModel: (index: number, properties: ModelProperties) -> boolean,
    -- Sets the clear color for the next frame
    setClearColor: (r: number, g: number, b: number, a: number) -> boolean,
    -- Sets the light properties for the next frame
    setLight: (lightNumber: number, properties: LightProperties) -> boolean,
    -- Registers a callback function to be called before rendering each frame
    registerBeforeRenderCallback: (callback: () -> ()) -> boolean,
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

-- Adds a new model and returns its index
luau3d.addModel = function(properties)
    -- This is a placeholder for the actual implementation provided by the engine
    return 0
end

-- Removes a model by index
luau3d.removeModel = function(index)
    -- This is a placeholder for the actual implementation provided by the engine
    return true
end

-- Removes all models
luau3d.clearModels = function()
    -- This is a placeholder for the actual implementation provided by the engine
    return true
end

-- Sets the visibility of a model
luau3d.setModelVisible = function(index, visible)
    -- This is a placeholder for the actual implementation provided by the engine
    return true
end

-- Updates a model's properties
luau3d.updateModel = function(index, properties)
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

-- Registers a callback function to be called before rendering each frame
luau3d.registerBeforeRenderCallback = function(callback)
    -- This is a placeholder for the actual implementation provided by the engine
    return true
end

return luau3d :: Luau3D
