local viewID = 0
local triangle = {}
local function setup(glfw, bcfx, win)
    glfw.setKeyCallback(win, function(win, key, scancode, action, mods)
        if key == glfw.keyboard.ESCAPE and action == glfw.input_state.PRESS then
            glfw.setWindowShouldClose(win, true)
        end
    end)
    glfw.setFramebufferSizeCallback(win, function(window, width, height)
        bcfx.setViewRect(viewID, 0, 0, width, height)
    end)
    bcfx.setViewRect(viewID, 0, 0, glfw.getFramebufferSize(win))
    bcfx.setViewClear(viewID, bcfx.clear_flag_mask.Color | bcfx.clear_flag_mask.Depth, bcfx.color.cyan, 1.0, 0)

    local lookAtMat = bcfx.math.graphics3d.lookAt(bcfx.math.vector.Vec3(0, -2, 0), bcfx.math.vector.zero, bcfx.math.vector.up)
    local projMat = bcfx.math.graphics3d.perspective(60, 4/3, 0.1)
    bcfx.setViewTransform(viewID, lookAtMat, projMat)

    local layout = bcfx.VertexLayout()
    layout:add(bcfx.vertex_attrib.Position, 3, bcfx.attrib_type.Float, false)
    local vertexData = bcfx.makeMemBuffer(bcfx.data_type.Float, {
        -0.5, -0.5,  0.5, -- In OpenGL: front
        0.5, -0.5,  0.5, -- In Unity : back
        0.5,  0.5,  0.5,
        -0.5,  0.5,  0.5,
        -0.5,  0.5, -0.5, -- In OpenGL: back
        0.5,  0.5, -0.5, -- In Unity : front
        0.5, -0.5, -0.5,
        -0.5, -0.5, -0.5,
        -0.5, -0.5,  0.5, -- left
        -0.5,  0.5,  0.5,
        -0.5,  0.5, -0.5,
        -0.5, -0.5, -0.5,
        0.5, -0.5, -0.5, -- right
        0.5,  0.5, -0.5,
        0.5,  0.5,  0.5,
        0.5, -0.5,  0.5,
        0.5,  0.5,  0.5, -- up
        0.5,  0.5, -0.5,
        -0.5,  0.5, -0.5,
        -0.5,  0.5,  0.5,
        -0.5, -0.5,  0.5, -- down
        -0.5, -0.5, -0.5,
        0.5, -0.5, -0.5,
        0.5, -0.5,  0.5,
    }
    )
    triangle.vertexHandle = bcfx.createVertexBuffer(vertexData, layout)
    local mem = bcfx.makeMemBuffer(bcfx.data_type.Uint8, {
        0,  1,  2,  0,  2,  3, -- In OpenGL: front, In Unity: back
        4,  5,  6,  4,  6,  7, -- In OpenGL: back, In Unity: front
        8,  9,  10, 8,  10, 11, -- left
        12, 13, 14, 12, 14, 15, -- right
        16, 17, 18, 16, 18, 19, -- up
        20, 21, 22, 20, 22, 23, -- down
    })
    triangle.indexHandle = bcfx.createIndexBuffer(mem, bcfx.index_type.Uint8)

    local vs = bcfx.createShader([[
#version 410 core
in vec3 a_position;
uniform mat4 u_modelViewProj;
out vec3 v_pos;
void main() {
v_pos = a_position;
    gl_Position = u_modelViewProj * vec4(a_position, 1.0);
}
    ]], bcfx.shader_type.Vertex)
    local fs = bcfx.createShader([[
#version 410 core
in vec3 v_pos;
out vec4 FragColor;
void main() {
    FragColor = vec4(v_pos, 1.0);
}
    ]], bcfx.shader_type.Fragment)
    triangle.programHandle = bcfx.createProgram(vs, fs)
    bcfx.destroy(vs, fs)

    triangle.renderFlags = bcfx.utils.packRenderState({
        enableDepth = true,
    })
end
local function loop(bcfx)
    bcfx.setVertexBuffer(0, triangle.vertexHandle)
    bcfx.setIndexBuffer(triangle.indexHandle)
    bcfx.setTransform(bcfx.math.graphics3d.rotate(bcfx.frameId() % 360, bcfx.math.vector.Vec3(1.0, 1.0, 0.0)))
    bcfx.setState(triangle.renderFlags, bcfx.color.black)
    bcfx.submit(viewID, triangle.programHandle)
end
local function clear(bcfx)
    bcfx.destroy(
        triangle.vertexHandle,
        triangle.indexHandle,
        triangle.programHandle
    )
end

return {
    setup = setup,
    loop = loop,
    clear = clear,
}
