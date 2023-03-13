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
    layout:addAttrib(bcfx.vertex_attrib.TexCoord0, 2, bcfx.attrib_type.Float, false)
    local vertexData = bcfx.makeMemBuffer(bcfx.data_type.Float, {
        -0.5, -0.5, 0.0, 0.0, 0.0,
        0.5, -0.5, 0.0, 1.0, 0.0,
        0.0, 0.7, 0.0, 0.0, 1.0,
    })
    triangle.vertexHandle = bcfx.createVertexBuffer(vertexData, layout)
    triangle.uniformHandle = bcfx.createUniform("u_texture", bcfx.uniform_type.Sampler2D)
    local vs = bcfx.createShader([[
#version 410 core
in vec3 a_position;
in vec2 a_texcoord0;
uniform mat4 u_model;
out vec2 v_texcoord0;
void main() {
    v_texcoord0 = a_texcoord0;
    gl_Position = u_model * vec4(a_position, 1.0);
}
    ]], bcfx.shader_type.Vertex)
    local fs = bcfx.createShader([[
#version 410 core
in vec2 v_texcoord0;
uniform sampler2D u_texture;
out vec4 FragColor;
void main() {
    FragColor = texture(u_texture, v_texcoord0);
}
    ]], bcfx.shader_type.Fragment)
    triangle.programHandle = bcfx.createProgram(vs, fs)
    bcfx.destroy(vs, fs)

    local libuv = require("libuv")
    local mb = libuv.fs.readFile("resources/awesomeface.png")
    local data, w, h = bcfx.image.imageDecode(mb, bcfx.texture_format.RGB8)
    triangle.textureHandle = bcfx.createTexture2D(bcfx.texture_format.RGB8, data, w, h)
    local textureFlags = bcfx.utils.packSamplerFlags({
        wrapU = bcfx.texture_wrap.Clamp,
        wrapV = bcfx.texture_wrap.Clamp,
        filterMag = bcfx.texture_filter.Linear,
        filterMin = bcfx.texture_filter.Linear,
    })
    triangle.samplerHandle = bcfx.createSampler(textureFlags)
end
local function loop(bcfx)
    -- bcfx.touch(viewID)
    bcfx.setVertexBuffer(0, triangle.vertexHandle)
    -- bcfx.setTransform(bcfx.math.graphics3d.rotate(bcfx.frameId() % 360, bcfx.math.vector.Vec3(0.0, 0.0, 1.0)))
    bcfx.setTexture(0, triangle.uniformHandle, triangle.textureHandle, triangle.samplerHandle)
    bcfx.submit(viewID, triangle.programHandle)
end
local function clear(bcfx)
    bcfx.destroy(
        triangle.vertexHandle,
        triangle.uniformHandle,
        triangle.textureHandle,
        triangle.samplerHandle,
        triangle.programHandle
    )
end

return {
    setup = setup,
    loop = loop,
    clear = clear,
}
