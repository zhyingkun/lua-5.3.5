local nk = require("nuklear")

local imgui = {}
local glfw_
function imgui:setup(glfw, bcfx, mainWin)
    glfw_ = glfw
    self.mainWin = mainWin
    self.atlas = nk.font.Atlas()
    self.atlas:begin()
    local cfg = nk.font.Config()
    self.myFont = self.atlas:addDefault(22, cfg)
    local image, release, ud, width, height = self.atlas:bake(1)
    local mb = util.MemBuffer(image, width * height * 4, release, ud)
    mb = bcfx.image.imageFlipVertical(mb, width, height)
    self.imageHandle = bcfx.createTexture2D(bcfx.texture_format.RGBA8, mb, width, height)
    self.nullTex = self.atlas:endAtlas(self.imageHandle)

    local ctx = nk.Context(self.myFont)
    nk.setContext(ctx)

    self.cmds = nk.Buffer()
    local vsize = 4 * 1024 * 1024
    local esize = 4 * 1024 * 1024

    local layout = bcfx.VertexLayout()
    layout:addAttrib(bcfx.vertex_attrib.Position, 2, bcfx.attrib_type.Float, false)
    layout:addAttrib(bcfx.vertex_attrib.TexCoord0, 2, bcfx.attrib_type.Float, false)
    layout:addAttrib(bcfx.vertex_attrib.Color0, 4, bcfx.attrib_type.Uint8, true)
    self.vertexHandle = bcfx.createDynamicVertexBuffer(vsize, layout)
    self.indexHandle = bcfx.createDynamicIndexBuffer(esize, bcfx.index_type.Uint16)
    self.vertexMemBuffer = util.MemBuffer()
    self.indexMemBuffer = util.MemBuffer()

    self.uniformTex = bcfx.createUniform("Texture", bcfx.uniform_type.Sampler2D)

    local vs = bcfx.createShader([[
#version 410 core
in vec2 a_position;
in vec2 a_texcoord0;
in vec4 a_color0;
uniform mat4 u_proj;
out vec2 v_texcoord0;
out vec4 v_color0;
void main() {
    v_texcoord0 = a_texcoord0;
    v_color0 = a_color0;
    gl_Position = u_proj * vec4(a_position.xy, 0, 1);
}
    ]], bcfx.shader_type.Vertex)
    local fs = bcfx.createShader([[
#version 410 core
uniform sampler2D Texture;
in vec2 v_texcoord0;
in vec4 v_color0;
out vec4 FragColor;
void main(){
    vec2 uv = vec2(v_texcoord0.s, 1.0 - v_texcoord0.t);
    FragColor = v_color0 * texture(Texture, uv);
}
    ]], bcfx.shader_type.Fragment)
    self.shader = bcfx.createProgram(vs, fs)
    bcfx.destroy(vs, fs)

    local w, h = glfw.getWindowSize(mainWin)
    local projMat = bcfx.math.graphics3d.orthogonal(
            -w/2, w/2,-- left, right
            -h/2, h/2,-- bottom, top
            -1.0, 1.0-- zNear, zFar
    )
    local transMat = bcfx.math.graphics3d.translate(
            bcfx.math.vector.Vec3(-1.0, 1.0, 0.0)
    )
    local scaleMat = bcfx.math.graphics3d.scale(
            bcfx.math.vector.Vec3(1.0, -1.0, 0.0)
    )
    -- transMat and scaleMat for convert window coordinate to OpenGL ClipSpace
    self.projMat = transMat * scaleMat * projMat

    self.state = bcfx.utils.packRenderState({
        enableCull = false,
        enableDepth = false,
        enableBlend = true,
        blendEquRGB = bcfx.blend_equation.FuncAdd,
        blendEquA = bcfx.blend_equation.FuncAdd,
        srcRGB = bcfx.blend_func.SrcAlpha,
        srcAlpha = bcfx.blend_func.SrcAlpha,
        dstRGB = bcfx.blend_func.OneMinusSrcAlpha,
        dstAlpha = bcfx.blend_func.OneMinusSrcAlpha,
    })

    self.vbuf1 = nk.Buffer(vsize)
    self.ebuf1 = nk.Buffer(esize)
    self.vbuf2 = nk.Buffer(vsize)
    self.ebuf2 = nk.Buffer(esize)

    local libuv = require("libuv")
    local mb = libuv.fs.readFile("resources/awesomeface.png")
    local data, w, h = bcfx.image.imageDecode(mb, bcfx.texture_format.RGB8)
    self.textureHandle = bcfx.createTexture2D(bcfx.texture_format.RGB8, data, w, h)

    self.texture = nk.Image(self.textureHandle)

    local textureFlags = bcfx.utils.packSamplerFlags({
        wrapU = bcfx.texture_wrap.Repeat,
        wrapV = bcfx.texture_wrap.Repeat,
        filterMin = bcfx.texture_filter.Linear,
        filterMag = bcfx.texture_filter.Linear,
    })
    self.samplerHandle = bcfx.createSampler(textureFlags)
end
function imgui:loop(bcfx)
    local glfw = glfw_

    nk.styleSetFont(self.myFont)
    ---[[
    if nk.begin("Basic Demo", nk.packRect(20, 20, 200, 130),
            nk.panel_flag.BORDER | nk.panel_flag.MOVABLE | nk.panel_flag.TITLE |
                    nk.panel_flag.SCALABLE | nk.panel_flag.CLOSABLE | nk.panel_flag.MINIMIZABLE)
    then
        nk.layoutRowDynamic(20, 1)
        nk.textWidget("1234567890", nk.text_alignment.RIGHT)
        nk.layoutRowStatic(50, 50, 2)
        if nk.buttonText("AAA") then
            print("OnButtonClicked")
        end
        if nk.buttonImage(self.texture) then
            print("OnImageButtonClicked")
        end
    end
    nk.endWindow()

    local vbuf, ebuf
    if bcfx.frameId() % 2 == 0 then
        vbuf = self.vbuf1
        ebuf = self.ebuf1
    else
        vbuf = self.vbuf2
        ebuf = self.ebuf2
    end
    vbuf:clear()
    ebuf:clear()

    nk.convert(self.cmds, vbuf, ebuf, self.nullTex)

    self.vertexMemBuffer:setReplace(vbuf:frontBufferPtr(), vbuf:frontBufferAllocated()) --
    self.indexMemBuffer:setReplace(ebuf:frontBufferPtr(), ebuf:frontBufferAllocated()) --
    bcfx.updateDynamicBuffer(self.vertexHandle, 0, self.vertexMemBuffer)
    bcfx.updateDynamicBuffer(self.indexHandle, 0, self.indexMemBuffer)

    bcfx.setVertexBuffer(0, self.vertexHandle)
    bcfx.setState(self.state, bcfx.color.black)
    bcfx.setViewTransform(255, nil, self.projMat)
    local pixelWidth, pixelHeight = glfw.getFramebufferSize(self.mainWin)
    bcfx.setViewRect(255, 0, 0, pixelWidth, pixelHeight)
    bcfx.setViewClear(255, bcfx.clear_flag_mask.Color, bcfx.color.blue, 1.0, 0)

    local screenWidth, screenHeight = glfw.getWindowSize(self.mainWin)
    nk.drawForEach(self.cmds, screenWidth, screenHeight, pixelWidth, pixelHeight, function(offset, count, texture, x, y, w, h)
        bcfx.setIndexBuffer(self.indexHandle, offset, count)
        bcfx.setTexture(0, self.uniformTex, texture, self.samplerHandle)
        bcfx.setScissor(x, y, w, h) -- in pixel coordinate
        bcfx.submit(255, self.shader, bcfx.discard_flag_mask.IndexBuffer | bcfx.discard_flag_mask.Bindings)
    end)

    nk.clear()
    self.cmds:clear()
end
function imgui:clear(bcfx)
end

return {
    setup = function(...) return imgui:setup(...) end,
    loop = function(...) return imgui:loop(...) end,
    clear = function(...) return imgui:clear(...) end,
}
