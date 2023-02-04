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
    bcfx.setViewClear(viewID, bcfx.clear_flag_mask.Color, bcfx.color.cyan, 1.0, 0)

    local layout = bcfx.VertexLayout()
    layout:addAttrib(bcfx.vertex_attrib.Position, 3, bcfx.attrib_type.Float, false)
    local vertexData = bcfx.makeMemBuffer(bcfx.data_type.Float, {
        -0.5, -0.5, 0.0,
        0.5, -0.5, 0.0,
        0.0, 0.5, 0.0,
    })
    triangle.vertexHandle = bcfx.createVertexBuffer(vertexData, layout)
    local vs = bcfx.createShader([[
#version 410 core
in vec3 a_position;
uniform mat4 u_model;
out vec3 v_color0;
void main() {
    gl_Position = u_model * vec4(a_position, 1.0);
}
]], bcfx.shader_type.Vertex)
    local fs = bcfx.createShader([[
#version 410 core
out vec4 FragColor;
void main() {
    FragColor = vec4(0.8, 0.3, 0.3, 1.0);
}
]], bcfx.shader_type.Fragment)
    triangle.programHandle = bcfx.createProgram(vs, fs)
    bcfx.destroy(vs, fs)
end
local function loop(bcfx)
    bcfx.setVertexBuffer(0, triangle.vertexHandle)
    bcfx.setTransform(bcfx.math.graphics3d.rotate(bcfx.frameId() % 360, bcfx.math.vector.Vec3(0.0, 0.0, 1.0)))
    bcfx.submit(viewID, triangle.programHandle)
end
local function clear(bcfx)
    bcfx.destroy(
        triangle.vertexHandle,
        triangle.programHandle
    )
end

return {
    setup = setup,
    loop = loop,
    clear = clear,
}
