local glfw = require("glfw")
local RELEASE = glfw.input_state.RELEASE

local bcfx = require("bcfx")
local vertex_attrib = bcfx.vertex_attrib
local attrib_type = bcfx.attrib_type
local graphics3d = bcfx.math.graphics3d
local vector = bcfx.math.vector
local texture_wrap = bcfx.texture_wrap
local texture_filter = bcfx.texture_filter
local discard = bcfx.discard_flag_mask
local blend_equation = bcfx.blend_equation
local blend_func = bcfx.blend_func

local nk = require("nuklear")
local panel_flag = nk.panel_flag
local text_alignment = nk.text_alignment
local index_type = bcfx.index_type

local ToNKMouseButton = {
	[glfw.mouse_button.LEFT] = nk.mouse_button.LEFT,
	[glfw.mouse_button.RIGHT] = nk.mouse_button.RIGHT,
	[glfw.mouse_button.MIDDLE] = nk.mouse_button.MIDDLE,
}

local imgui = {
	bImGUI = false,
	unicodeCount = 0,

	winSizeX = 0, winSizeY = 0,
	fbSizeX = 0, fbSizeY = 0,

	atlas = nil, imguiFont = nil, nullTex = nil,

	layout = nil, vertex = nil, index = nil,
	textureUniform = nil, sampler = nil,
	imguiShader = nil,

	transScaleMat = nil, viewMat = nil, projMat = nil,
	renderState = nil,
}

function imgui:BindAction(input)
	input:bindAction("ImGUIKey", function(key, scancode, action, mods)
		if not self.bImGUI then return end
	end)
	input:bindAction("ImGUIChar", function(codepoint)
		if not self.bImGUI then return end
		self.unicodeCount = self.unicodeCount + 1
		if self.unicodeCount >= nk.INPUT_MAX then
			printerr("Error: too many input unicode in one frame, maybe low fps")
		else
			if codepoint < 128 then
				nk.inputChar(codepoint)
			else
				nk.inputUnicode(codepoint)
			end
		end
	end)
	input:bindAction("ImGUIMouseButton", function(button, action, mods, xPos, yPos)
		if not self.bImGUI then return end
		local nkBtn = ToNKMouseButton[button]
		if not nkBtn then return end
		nk.inputButton(nkBtn, math.floor(xPos), math.floor(yPos), action ~= RELEASE)
	end)
	input:bindAction("ImGUICursorPos", function(xPos, yPos)
		if not self.bImGUI then return end
		nk.inputMotion(math.floor(xPos), math.floor(yPos))
	end)
	input:bindAction("ImGUIScroll", function(xOffset, yOffset)
		if not self.bImGUI then return end
		nk.inputScroll(nk.packVec2(xOffset, yOffset))
	end)
	input:bindAction("ImGUIWindowSize", function(width, height)
		self.winSizeX, self.winSizeY = width, height
		self:UpdateProjectionMatrix()
	end)
	input:bindAction("ImGUIWindowFramebufferSize", function(width, height)
		self.fbSizeX, self.fbSizeY = width, height
	end)
end

function imgui:prePollEvent()
	nk.inputBegin()
	self.bImGUI = true
	self.unicodeCount = 0
end
function imgui:postPollEvent()
	nk.inputEnd()
	self.bImGUI = false
	self.unicodeCount = 0
end

function imgui:UpdateProjectionMatrix()
	-- projMat = bcfx.math.matrix.Mat4x4(
	-- 	2/w, 0, 0, -1,
	-- 	0, -2/h, 0, 1,
	-- 	0, 0, -1, 0,
	-- 	0, 0, 0, 1
	-- )
	local w, h = self.winSizeX, self.winSizeY
	local orthogonalMat = graphics3d.orthogonal(
			-w/2, w/2,-- left, right
			-h/2, h/2,-- bottom, top
			-1.0, 1.0-- zNear, zFar
	)
	self.projMat = self.transScaleMat * orthogonalMat
end

function imgui:init(input, winSzX, winSzY, fbSzX, fbSzY)
	self.winSizeX, self.winSizeY = winSzX, winSzY
	self.fbSizeX, self.fbSizeY = fbSzX, fbSzY
	self:BindAction(input)

	self.atlas = nk.font.Atlas()
	self.atlas:begin()
	self.imguiFont = self.atlas:addDefault(22, nk.font.Config())
	local mb, width, height = self.atlas:bake(nk.font.atlas_format.ALPHA8)
	local imageHandle = bcfx.createTexture2D(bcfx.utils.packTextureParameter({
		format = bcfx.texture_format.R8,
		swizzleR = bcfx.texture_swizzle.One,
		swizzleG = bcfx.texture_swizzle.One,
		swizzleB = bcfx.texture_swizzle.One,
		swizzleA = bcfx.texture_swizzle.Red,
	}), bcfx.image.imageFlipVertical(mb, width, height), width, height)
	self.nullTex = self.atlas:endAtlas(imageHandle)

	self.sampler = bcfx.createSampler(bcfx.utils.packSamplerFlags({
		wrapU = texture_wrap.Repeat,
		wrapV = texture_wrap.Repeat,
		filterMin = texture_filter.Linear,
		filterMag = texture_filter.Linear,
	}))

	local layout = bcfx.VertexLayout()
	layout:addAttrib(vertex_attrib.Position, 2, attrib_type.Float, false)
	layout:addAttrib(vertex_attrib.TexCoord0, 2, attrib_type.Float, false)
	layout:addAttrib(vertex_attrib.Color0, 4, attrib_type.Uint8, true)
	self.layout = layout

	-- TODO: Using dynamic size for this two Buffer
	local vsize = 4 * 1024 * 1024
	local esize = 4 * 1024 * 1024
	self.vertex = bcfx.createDynamicVertexBuffer(vsize, layout)
	self.index = bcfx.createDynamicIndexBuffer(esize, index_type.Uint16)

	self.textureUniform = bcfx.createUniform("Texture", bcfx.uniform_type.Sampler2D)
	self.imguiShader = require("loader").LoadProgram("imgui")

	self.viewMat = graphics3d.lookAt(
		vector.Vec3(0.0, 0.0, 3.0), -- eye
		vector.Vec3(0.0, 0.0, 0.0), -- center
		vector.Vec3(0.0, 1.0, 0.0) -- up
	)
	local transMat = graphics3d.translate(
		vector.Vec3(-1.0, 1.0, 0.0)
	)
	local scaleMat = graphics3d.scale(
		vector.Vec3(1.0, -1.0, 0.0)
	)
	-- transMat and scaleMat for convert window coordinate to OpenGL ClipSpace
	self.transScaleMat = transMat * scaleMat
	self:UpdateProjectionMatrix()

	self.renderState = bcfx.utils.packRenderState({
		enableCull = false,
		enableDepth = false,
		enableBlend = true,
		blendEquRGB = blend_equation.FuncAdd,
		blendEquA = blend_equation.FuncAdd,
		srcRGB = blend_func.SrcAlpha,
		srcAlpha = blend_func.SrcAlpha,
		dstRGB = blend_func.OneMinusSrcAlpha,
		dstAlpha = blend_func.OneMinusSrcAlpha,
	})

	nk.setContext(nk.Context(self.imguiFont))
	-- bcfx.setViewDebug(255, bcfx.debug.WIREFRAME)
end

function imgui:tick(delta)
	nk.styleSetFont(self.imguiFont)
	nk.stylePushStyleItem(
			nk.getStylePtrTable().style_item.window.fixed_background,
			nk.StyleItem(nk.rgbaToColor(0, 0, 0, 0))
	)
	if nk.begin("You Should not see this Title",
			nk.packRect(0, 0, self.winSizeX, self.winSizeY),
			panel_flag.NO_SCROLLBAR
	) then
		-- Insert ImGUI code here start
		nk.layoutRowDynamic(20, 1)
		nk.textWidget("1234567890", text_alignment.LEFT)
		nk.layoutRowStatic(50, 100, 1)
		if nk.buttonText("Button") then
			print("Button Clicked")
		end
		-- Insert ImGUI code here end
	end
	nk.endWindow()
	nk.stylePopStyleItem()

	local vmb, imb = nk.convertAsMemBuffer(self.nullTex, bcfx.frameId())
	bcfx.updateDynamicBuffer(self.vertex, 0, vmb)
	bcfx.updateDynamicBuffer(self.index, 0, imb)

	bcfx.setVertexBuffer(0, self.vertex)
	bcfx.setState(self.renderState, bcfx.color.black)
	bcfx.setViewTransform(255, self.viewMat, self.projMat)
	bcfx.setViewRect(255, 0, 0, self.fbSizeX, self.fbSizeY)

	nk.drawForEach(self.winSizeX, self.winSizeY, self.fbSizeX, self.fbSizeY, function(offset, count, texture, x, y, w, h)
		bcfx.setIndexBuffer(self.index, offset, count)
		bcfx.setTexture(0, self.textureUniform, texture, self.sampler)
		bcfx.setScissor(x, y, w, h) -- in pixel coordinate
		bcfx.submit(255, self.imguiShader, discard.IndexBuffer | discard.Bindings)
	end)

	nk.clear()
end

return imgui
