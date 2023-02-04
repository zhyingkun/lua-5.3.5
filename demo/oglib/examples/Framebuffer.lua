local viewIDClear = 0
local viewIDFB = 1
local viewID1 = 2
local viewID2 = 3
local triangle = {}
local blit = {}
local function setup(glfw, bcfx, win)
    glfw.setKeyCallback(win, function(win, key, scancode, action, mods)
        if key == glfw.keyboard.ESCAPE and action == glfw.input_state.PRESS then
            glfw.setWindowShouldClose(win, true)
        end
    end)
    glfw.setFramebufferSizeCallback(win, function(window, width, height)
        bcfx.setViewRect(viewIDClear, 0, 0, width, height)
        local offset1 = width // 40
        local offset2 = width // 3
        bcfx.setViewRect(viewID1, offset1, offset1, width // 3, height // 3)
        bcfx.setViewRect(viewID2, offset2, offset2, width // 4, height // 4)
    end)

    local width, height = glfw.getFramebufferSize(win)
    bcfx.setViewRect(viewIDClear, 0, 0, width, height)
    bcfx.setViewClear(viewIDClear, bcfx.clear_flag_mask.Color, bcfx.color.indigo, 1.0, 0)

    blit.texture = bcfx.createRenderTexture(bcfx.texture_format.RGBA8, width, height)
    local fb = bcfx.createFrameBuffer(blit.texture)
    bcfx.setViewFrameBuffer(viewIDFB, fb)
    bcfx.setViewRect(viewIDFB, 0, 0, width, height)
    bcfx.setViewClear(viewIDFB, bcfx.clear_flag_mask.Color, bcfx.color.cyan, 1.0, 0)

    local offset1 = width // 40
    bcfx.setViewRect(viewID1, offset1, offset1, width // 3, height // 3)
    bcfx.setViewClear(viewID1, bcfx.clear_flag_mask.Color, bcfx.color.black, 1.0, 0)

    local offset2 = width // 3
    bcfx.setViewRect(viewID2, offset2, offset2, width // 4, height // 4)
    bcfx.setViewClear(viewID2, bcfx.clear_flag_mask.Color, bcfx.color.black, 1.0, 0)

    local layout = bcfx.VertexLayout()
    layout:addAttrib(bcfx.vertex_attrib.Position, 3, bcfx.attrib_type.Float, false)
    local vertexData = bcfx.makeMemBuffer(bcfx.data_type.Float, {
        -0.5, -0.5,  0.0,
        0.5, -0.5,  0.0,
        0.0,  0.5,  0.0,
    })
    triangle.vertexHandle = bcfx.createVertexBuffer(vertexData, layout)

    local vs = bcfx.createShader([[
#version 410 core
in vec3 a_position;
void main() {
    gl_Position = vec4(a_position, 1.0);
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

    local layout = bcfx.VertexLayout()
    layout:addAttrib(bcfx.vertex_attrib.Position, 2, bcfx.attrib_type.Float, false)
    layout:addAttrib(bcfx.vertex_attrib.TexCoord0, 2, bcfx.attrib_type.Float, false)
    local vertexTbl = {
        -1.0, -1.0, 0.0, 0.0,
        1.0, -1.0, 1.0, 0.0,
        1.0, 1.0, 1.0, 1.0,
        -1.0, -1.0, 0.0, 0.0,
        1.0, 1.0, 1.0, 1.0,
        -1.0, 1.0, 0.0, 1.0,
    }
    local mem = bcfx.makeMemBuffer(bcfx.data_type.Float, vertexTbl)
    blit.vertex = bcfx.createVertexBuffer(mem, layout)
    blit.uniform = bcfx.createUniform("blit_texture", bcfx.uniform_type.Sampler2D)
    local vs = bcfx.createShader([[
#version 410 core
in vec2 a_position;
in vec2 a_texcoord0;
out vec2 v_texcoord0;
void main() {
    v_texcoord0 = a_texcoord0;
    gl_Position = vec4(a_position, 0.0, 1.0);
}
    ]], bcfx.shader_type.Vertex)
    local fs = bcfx.createShader([[
#version 410 core
in vec2 v_texcoord0;
out vec4 FragColor;
uniform sampler2D blit_texture;
void main() {
    FragColor = texture(blit_texture, v_texcoord0);
}
    ]], bcfx.shader_type.Fragment)
    blit.shader = bcfx.createProgram(vs, fs)
    bcfx.destroy(vs, fs)

    local textureFlags = bcfx.utils.packSamplerFlags({
        wrapU = bcfx.texture_wrap.Repeat,
        wrapV = bcfx.texture_wrap.Repeat,
        filterMin = bcfx.texture_filter.Linear,
        filterMag = bcfx.texture_filter.Linear,
    })
    blit.samplerHandle = bcfx.createSampler(textureFlags)
end
local function loop(bcfx)
    bcfx.touch(viewIDClear)

    bcfx.setVertexBuffer(0, triangle.vertexHandle)
    bcfx.submit(viewIDFB, triangle.programHandle)

    bcfx.setVertexBuffer(0, blit.vertex)
    bcfx.setTexture(0, blit.uniform, blit.texture, blit.samplerHandle)
    bcfx.submit(viewID1, blit.shader)

    bcfx.setVertexBuffer(0, blit.vertex)
    bcfx.setTexture(0, blit.uniform, blit.texture, blit.samplerHandle)
    bcfx.submit(viewID2, blit.shader)
end
local function clear(bcfx)
    bcfx.destroy(
        triangle.vertexHandle,
        triangle.programHandle,
        blit.vertex,
        blit.uniform,
        blit.texture,
        blit.samplerHandle,
        blit.shader
    )
end

return {
    setup = setup,
    loop = loop,
    clear = clear,
}
