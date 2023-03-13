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

    local width, height = glfw.getFramebufferSize(win)
    bcfx.setViewRect(viewID, 0, 0, width, height)
    bcfx.setViewClear(viewID, bcfx.clear_flag_mask.Color|bcfx.clear_flag_mask.Depth, bcfx.color.cyan, 1.0, 0)

    local graphics3d = bcfx.math.graphics3d
    local vector = bcfx.math.vector
    local lookAt = graphics3d.lookAt(vector.left * -3, vector.left, vector.forward)
    --local lookAt = graphics3d.lookAt(vector.Vec3(0, 0, 3), vector.zero, vector.Vec3(0, 1, 0))
    bcfx.setViewTransform(viewID, lookAt, graphics3d.perspective(45, 16/9, 0.1))

    local layout = bcfx.VertexLayout()
    layout:addAttrib(bcfx.vertex_attrib.Position, 3, bcfx.attrib_type.Float, false)
    layout:addAttrib(bcfx.vertex_attrib.Normal, 3, bcfx.attrib_type.Float, false)
    layout:addAttrib(bcfx.vertex_attrib.TexCoord0, 2, bcfx.attrib_type.Float, false)

    local fileData = require("libuv").fs.readFile("resources/spot_quadrangulated.obj")
    local meshs = bcfx.mesh.meshParse(fileData)
    triangle.vertexHandle = bcfx.createVertexBuffer(meshs[1].vertex, layout)
    triangle.indexHandle = bcfx.createIndexBuffer(meshs[1].index, bcfx.index_type.Uint32)

    local fileData = require("libuv").fs.readFile("resources/spot_texture.png")
    local parseMB, width, height = bcfx.image.imageDecode(fileData, bcfx.texture_format.RGBA8)
    triangle.texture = bcfx.createTexture2D(bcfx.texture_format.RGBA8, parseMB, width, height)

    triangle.uniform = bcfx.createUniform("my_texture", bcfx.uniform_type.Sampler2D)

    triangle.state = bcfx.utils.packRenderState({
        enableDepth = true,
    })

    local vs = bcfx.createShader([[
#version 410 core
in vec3 a_position;
in vec2 a_texcoord0;
uniform mat4 u_modelViewProj;
out vec2 v_texcoord0;
void main() {
    v_texcoord0 = a_texcoord0;
    gl_Position = u_modelViewProj * vec4(a_position, 1.0);
}
    ]], bcfx.shader_type.Vertex)
    local fs = bcfx.createShader([[
#version 410 core
in vec2 v_texcoord0;
uniform sampler2D my_texture;
out vec4 FragColor;
void main() {
    FragColor = texture(my_texture, v_texcoord0);
}
    ]], bcfx.shader_type.Fragment)
    triangle.programHandle = bcfx.createProgram(vs, fs)

    local textureFlags = bcfx.utils.packSamplerFlags({
        wrapU = bcfx.texture_wrap.Repeat,
        wrapV = bcfx.texture_wrap.Repeat,
        filterMin = bcfx.texture_filter.Linear,
        filterMag = bcfx.texture_filter.Linear,
    })
    triangle.samplerHandle = bcfx.createSampler(textureFlags)
end
local function loop(bcfx)
    bcfx.setVertexBuffer(0, triangle.vertexHandle)
    bcfx.setIndexBuffer(triangle.indexHandle)
    bcfx.setTexture(0, triangle.uniform, triangle.texture, triangle.samplerHandle)
    bcfx.setTransform(bcfx.math.graphics3d.rotate(bcfx.frameId() % 360, bcfx.math.vector.Vec3(0.0, 1.0, 0.0)))
    bcfx.setState(triangle.state, bcfx.color.black)
    bcfx.submit(viewID, triangle.programHandle, bcfx.discard_flag_mask.All)
end
local function clear(bcfx)
    bcfx.destroy(
        triangle.vertexHandle,
        triangle.indexHandle,
        triangle.uniform,
        triangle.texture,
        triangle.samplerHandle,
        triangle.programHandle
    )
end

return {
    setup = setup,
    loop = loop,
    clear = clear,
}
