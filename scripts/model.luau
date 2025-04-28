local model = {}

export type Vertex = {
    x: number,
    y: number,
    z: number,
    r: number,
    g: number,
    b: number,
}

export type Triangle = {number}

export type Model = {
    createModel: (vertices: {Vertex}, triangles: {Triangle}) -> any,
}

export type CFrame = {
    position: {number}, -- {x, y, z}
    look: {number},    -- {x, y, z}
    up: {number},      -- {x, y, z}
    right: {number},   -- {x, y, z}
}

function model.createCube(radius: number, cframe: CFrame)
    local vertices = {
        -- Front face
        {x = -radius, y = -radius, z = -radius, r = 1.0, g = 0.0, b = 0.0}, -- red
        {x = -radius, y =  radius, z = -radius, r = 0.0, g = 1.0, b = 0.0}, -- green
        {x =  radius, y =  radius, z = -radius, r = 0.0, g = 0.0, b = 1.0}, -- blue
        {x =  radius, y = -radius, z = -radius, r = 1.0, g = 1.0, b = 0.0}, -- yellow
        -- Back face
        {x =  radius, y = -radius, z =  radius, r = 1.0, g = 0.0, b = 1.0}, -- magenta
        {x =  radius, y =  radius, z =  radius, r = 0.0, g = 1.0, b = 1.0}, -- cyan
        {x = -radius, y =  radius, z =  radius, r = 1.0, g = 1.0, b = 1.0}, -- white
        {x = -radius, y = -radius, z =  radius, r = 0.5, g = 0.5, b = 0.5}, -- gray
    }
    
    local triangles = {
        -- Front face
        {1, 2, 3},
        {1, 3, 4},
        -- Back face
        {5, 6, 7},
        {5, 7, 8},
        -- Top face
        {2, 7, 6},
        {2, 6, 3},
        -- Bottom face
        {1, 4, 5},
        {1, 5, 8},
        -- Right face
        {4, 3, 6},
        {4, 6, 5},
        -- Left face
        {8, 7, 2},
        {8, 2, 1},
    }
    
    return {
        vertices = model.createGeometry(vertices, triangles),
        visible = true,
        cframe = cframe
    }
end

function model.createGeometry(vertices: {Vertex}, triangles: {Triangle})
    -- Modify to include color data
    local triangleStrips = {}
    for _, triangle in pairs(triangles) do
        local v1 = vertices[triangle[1]]
        local v2 = vertices[triangle[2]]
        local v3 = vertices[triangle[3]]
        -- Position and color for vertex 1
        table.insert(triangleStrips, v1.x)
        table.insert(triangleStrips, v1.y)
        table.insert(triangleStrips, v1.z)
        table.insert(triangleStrips, v1.r or 1.0)
        table.insert(triangleStrips, v1.g or 1.0)
        table.insert(triangleStrips, v1.b or 1.0)
        -- Position and color for vertex 2
        table.insert(triangleStrips, v2.x)
        table.insert(triangleStrips, v2.y)
        table.insert(triangleStrips, v2.z)
        table.insert(triangleStrips, v2.r or 1.0)
        table.insert(triangleStrips, v2.g or 1.0)
        table.insert(triangleStrips, v2.b or 1.0)
        -- Position and color for vertex 3
        table.insert(triangleStrips, v3.x)
        table.insert(triangleStrips, v3.y)
        table.insert(triangleStrips, v3.z)
        table.insert(triangleStrips, v3.r or 1.0)
        table.insert(triangleStrips, v3.g or 1.0)
        table.insert(triangleStrips, v3.b or 1.0)
    end
    return triangleStrips
end

return model 