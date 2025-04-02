local opengl = {}

-- Type definitions
export type Vec2 = {number} -- Length 2 array of numbers    
export type Vec3 = {number} -- Length 3 array of numbers
export type Vec4 = {number} -- Length 4 array of numbers
export type Mat4 = {number} -- Length 16 array of numbers

-- Constants
opengl.GL_TRIANGLES = 0x0004
opengl.GL_TRIANGLE_STRIP = 0x0005
opengl.GL_TRIANGLE_FAN = 0x0006
opengl.GL_LINES = 0x0001
opengl.GL_LINE_STRIP = 0x0003
opengl.GL_LINE_LOOP = 0x0002
opengl.GL_POINTS = 0x0000

opengl.GL_VERTEX_SHADER = 0x8B31
opengl.GL_FRAGMENT_SHADER = 0x8B30

opengl.GL_ARRAY_BUFFER = 0x8892
opengl.GL_ELEMENT_ARRAY_BUFFER = 0x8893
opengl.GL_STATIC_DRAW = 0x88E4
opengl.GL_DYNAMIC_DRAW = 0x88E8

opengl.GL_FLOAT = 0x1406
opengl.GL_UNSIGNED_INT = 0x1405
opengl.GL_UNSIGNED_SHORT = 0x1403
opengl.GL_UNSIGNED_BYTE = 0x1401

opengl.GL_POSITION = 0x0
opengl.GL_NORMAL = 0x1
opengl.GL_COLOR = 0x2
opengl.GL_TEXCOORD = 0x3

-- Shader functions
function opengl.createShader(shaderType: number): number
    return 0
end

function opengl.shaderSource(shader: number, source: string)
end

function opengl.compileShader(shader: number)
end

function opengl.deleteShader(shader: number)
end

function opengl.createProgram(): number
    return 0
end

function opengl.attachShader(program: number, shader: number)
end

function opengl.linkProgram(program: number)
end

function opengl.useProgram(program: number)
end

function opengl.deleteProgram(program: number)
end

-- Buffer functions
function opengl.genBuffers(n: number): {number}
    return {}
end

function opengl.bindBuffer(target: number, buffer: number)
end

function opengl.bufferData(target: number, data: {number}, usage: number)
end

function opengl.deleteBuffers(buffers: {number})
end

-- Vertex attribute functions
function opengl.enableVertexAttribArray(index: number)
end

function opengl.vertexAttribPointer(index: number, size: number, type: number, normalized: boolean, stride: number, offset: number)
end

-- Matrix functions
function opengl.ortho(left: number, right: number, bottom: number, top: number, near: number, far: number): Mat4
    return {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}
end

function opengl.perspective(fovy: number, aspect: number, near: number, far: number): Mat4
    return {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}
end

function opengl.lookAt(eye: Vec3, center: Vec3, up: Vec3): Mat4
    return {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}
end

function opengl.translate(m: Mat4, v: Vec3): Mat4
    return m
end

function opengl.rotate(m: Mat4, angle: number, axis: Vec3): Mat4
    return m
end

function opengl.scale(m: Mat4, v: Vec3): Mat4
    return m
end

-- Drawing functions
function opengl.drawArrays(mode: number, first: number, count: number)
end

function opengl.drawElements(mode: number, count: number, type: number, offset: number)
end

-- Uniform functions
function opengl.getUniformLocation(program: number, name: string): number
    return 0
end

function opengl.uniform1i(location: number, v0: number)
end

function opengl.uniform1f(location: number, v0: number)
end

function opengl.uniform2f(location: number, v0: number, v1: number)
end

function opengl.uniform3f(location: number, v0: number, v1: number, v2: number)
end

function opengl.uniform4f(location: number, v0: number, v1: number, v2: number, v3: number)
end

function opengl.uniformMatrix4fv(location: number, transpose: boolean, value: Mat4)
end

-- State management
function opengl.clearColor(r: number, g: number, b: number, a: number)
end

function opengl.clear(mask: number)
end

function opengl.enable(cap: number)
end

function opengl.disable(cap: number)
end

function opengl.viewport(x: number, y: number, width: number, height: number)
end

return opengl
