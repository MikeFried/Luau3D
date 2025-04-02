local geometry = {}

export type Vertex = {
    x: number,
    y: number,
    z: number,
}

export type Triangle = {number}

export type Geometry = {
    createGeometry: (vertices: {Vertex}, triangles: {Triangle}) -> any,
}

function geometry.createCube()
    local vertices = {
        {x = -1, y = -1, z = -1},
        {x = 1, y = -1, z = -1},
        {x = 1, y = 1, z = -1},
        {x = -1, y = 1, z = -1},
        {x = -1, y = -1, z = 1},
        {x = 1, y = -1, z = 1},
        {x = 1, y = 1, z = 1},
        {x = -1, y = 1, z = 1},
    }
    -- Make triangles - 12 triangles for the 6 faces of the cube
    local triangles = {
        -- Front face
        {1, 2, 3},
        {1, 3, 4},
        -- Back face
        {5, 6, 7},
        {5, 7, 8},
        -- Top face
        {1, 2, 6},
        {1, 6, 5},
        -- Bottom face
        {3, 4, 8},
        {3, 8, 7},
        -- Right face
        {2, 3, 7},
        {2, 7, 6},
        -- Left face
        {4, 1, 5},
        {4, 5, 8},
    }
    return geometry.createGeometry(vertices, triangles)
end

function geometry.createGeometry(vertices: {Vertex}, triangles: {Triangle})
    -- produce a flat list of x, y, and z ordered coordinates from all triangles suitable to be used in OpenGL
    local triangleStrips = {}
    for _, triangle in pairs(triangles) do
        local v1 = vertices[triangle[1]]
        local v2 = vertices[triangle[2]]
        local v3 = vertices[triangle[3]]
        table.insert(triangleStrips, v1.x)
        table.insert(triangleStrips, v1.y)
        table.insert(triangleStrips, v1.z)
        table.insert(triangleStrips, v2.x)
        table.insert(triangleStrips, v2.y)
        table.insert(triangleStrips, v2.z)
        table.insert(triangleStrips, v3.x)
        table.insert(triangleStrips, v3.y)
        table.insert(triangleStrips, v3.z)
    end
    return triangleStrips
end

return geometry 