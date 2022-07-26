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
    layout:add(bcfx.vertex_attrib.Position, 3, bcfx.attrib_type.Float, false)
    local vertexData = bcfx.makeMemBuffer(bcfx.data_type.Float, {
        -0.5, -0.5,  0.0,
        0.5, -0.5,  0.0,
        0.0,  0.5,  0.0,
    })
    triangle.vertexHandle = bcfx.createVertexBuffer(vertexData, layout)

    local vs = bcfx.createShader([[
#version 410 core
in vec3 a_position;
in vec3 i_data0;
out vec3 v_data0;
void main() {
    v_data0 = i_data0;
    float offset = float(gl_InstanceID) / 3.0;
    gl_Position = vec4(a_position + vec3(offset, offset, offset), 1.0);
}
    ]], bcfx.shader_type.Vertex)
    local fs = bcfx.createShader([[
#version 410 core
in vec3 v_data0;
out vec4 FragColor;
void main() {
    FragColor = vec4(v_data0, 1.0);
}
    ]], bcfx.shader_type.Fragment)
    triangle.programHandle = bcfx.createProgram(vs, fs)
    bcfx.destroy(vs, fs)

    triangle.renderFlags = bcfx.utils.packRenderState({
        enableDepth = true,
    })

    local instanceBuffer = bcfx.createDynamicInstanceDataBuffer(3, 1)
    local instanceOffset = {
        1.0, 0.0, 0.0, 0.0,
        0.3, 1.0, 0.3, 0.0,
        0.6, 0.6, 1.0, 0.0
    }
    local instanceData = bcfx.makeMemBuffer(bcfx.data_type.Float, instanceOffset)
    bcfx.updateDynamicBuffer(instanceBuffer, 0, instanceData)
    triangle.instanceDataBuffer = instanceBuffer
end
local function loop(bcfx)
    bcfx.setVertexBuffer(0, triangle.vertexHandle)
    --bcfx.setInstanceDataBuffer({
    --  numInstance = 3,
    --})
    bcfx.setInstanceDataBuffer(3, triangle.instanceDataBuffer, 0)
    bcfx.submit(viewID, triangle.programHandle)
end
local function clear(bcfx)
    bcfx.destroy(
        triangle.vertexHandle,
        triangle.instanceDataBuffer,
        triangle.programHandle
    )
end

return {
    setup = setup,
    loop = loop,
    clear = clear,
}
