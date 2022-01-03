local glfw = require("glfw")

local bcfx = require("bcfx")
local vertex_attrib = bcfx.vertex_attrib
local attrib_type = bcfx.attrib_type
local graphics3d = bcfx.math.graphics3d
local vector = bcfx.math.vector
local texture_wrap = bcfx.texture_wrap
local texture_filter = bcfx.texture_filter
local discard = bcfx.discard
local texture_format = bcfx.texture_format
local blend_equation = bcfx.blend_equation
local blend_func = bcfx.blend_func

local nk = require("nuklear")
local panel_flag = nk.panel_flag
local text_alignment = nk.text_alignment
local index_type = bcfx.index_type
local clear_flag = bcfx.clear_flag

local imgui = {}

local font
local nullTex

local vbuf1
local ebuf1
local vbuf2
local ebuf2

local cmds

local vertexHandle
local indexHandle
local vertexMemBuffer
local indexMemBuffer
local uniformTex
local imguiShader
local projMat
local state

local mainWin

function imgui.setup(mainWin_)
	mainWin = mainWin_
	local atlas = nk.FontAtlas()
	atlas:begin()
	local cfg = nk.FontConfig()
	font = atlas:add_default(22, cfg)
	local image, release, ud, width, height = atlas:bake(1)
	-- bcfx.utils.imageWrite("zykTest.png", width, height, 4, image)
	local mb = bcfx.membuf.MemBuffer(image, width * height * 4, release, ud)
	mb = bcfx.image.imageFlipVertical(mb, width, height)
	local imageHandle = bcfx.createTexture(mb, width, height, bcfx.texture_format.RGBA8)
	nullTex = atlas:endatlas(imageHandle)

	local ctx = nk.Context(font)
	nk.setContext(ctx)

	cmds = nk.Buffer()
	local vsize = 4 * 1024 * 1024
	local esize = 4 * 1024 * 1024

	local layout = bcfx.VertexLayout()
	layout:add(vertex_attrib.Position, 2, attrib_type.Float, false)
	layout:add(vertex_attrib.TexCoord0, 2, attrib_type.Float, false)
	layout:add(vertex_attrib.Color0, 4, attrib_type.Uint8, true)
	local layoutHandle = bcfx.createVertexLayout(layout)
	vertexHandle = bcfx.createDynamicVertexBuffer(vsize, layoutHandle)
	indexHandle = bcfx.createDynamicIndexBuffer(esize, index_type.Uint16)
	vertexMemBuffer = bcfx.membuf.MemBuffer()
	indexMemBuffer = bcfx.membuf.MemBuffer()

	uniformTex = bcfx.createUniform("Texture", 0)
	imguiShader = require("loader").LoadProgram("imgui")

	local viewMat = graphics3d.lookAt(
		vector.Vec3(0.0, 0.0, 3.0), -- eye
		vector.Vec3(0.0, 0.0, 0.0), -- center
		vector.Vec3(0.0, 1.0, 0.0) -- up
	)
	local w, h = glfw.GetFramebufferSize(mainWin)
	local w, h = glfw.GetWindowSize(mainWin)
	projMat = graphics3d.orthogonal(
		-w/2, w/2,-- left, right
		-h/2, h/2,-- bottom, top
		-1.0, 1.0-- zNear, zFar
	)
	local transMat = graphics3d.translate(
		vector.Vec3(-1.0, 1.0, 0.0)
	)
	local scaleMat = graphics3d.scale(
		vector.Vec3(1.0, -1.0, 0.0)
	)
	-- transMat and scaleMat for convert window coordinate to OpenGL ClipSpace

	-- projMat = bcfx.math.matrix.Mat4x4(
	-- 	2/w, 0, 0, -1,
	-- 	0, -2/h, 0, 1,
	-- 	0, 0, -1, 0,
	-- 	0, 0, 0, 1
	-- )
	projMat = transMat * scaleMat * projMat

	state = bcfx.utils.packRenderState({
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

	vbuf1 = nk.Buffer(vsize)
	ebuf1 = nk.Buffer(esize)
	vbuf2 = nk.Buffer(vsize)
	ebuf2 = nk.Buffer(esize)

	-- bcfx.setViewDebug(255, bcfx.debug.WIREFRAME)
	_img_ = nk.Image(_imgRT_)
end

local prog = 40
local slider = 10

function imgui.tick(delta)
	nk.styleSetFont(font)
	if nk.begin("Basic Demo", nk.packRect(0, 0, 400, 300), 
		panel_flag.BORDER  |panel_flag.MOVABLE |panel_flag.TITLE|
		panel_flag.SCALABLE|panel_flag.CLOSABLE|panel_flag.MINIMIZABLE)
	then
		nk.layoutRowDynamic(50, 1)
		nk.textWidget("ABCDEFGHIJKLMNOPQRSTUVWXYZ", text_alignment.LEFT)
		nk.layoutRowDynamic(20, 1)
		nk.textWidget("1234567890", text_alignment.RIGHT)
		nk.layoutRowDynamic(50, 1)
		if nk.buttonImageText(_img_, "Button", text_alignment.LEFT) then
			print("OnButtonClicked")
			_OpenPopup_ = not _OpenPopup_
		end
		if _OpenPopup_ then
			if nk.popupBegin(nk.popup_type.STATIC, "Image Popup", 0, nk.packRect(265, 0, 280, 220)) then
				nk.layoutRowStatic(82, 82, 3);
				-- nk.textWidget("ABCDEFGHIJKLMNOPQRSTUVWXYZ", text_alignment.LEFT)
				for i = 1, 9 do
					if nk.buttonImage(_img_) then
						_OpenPopup_ = false
						nk.popupClose()
					end
				end
				-- if nk.buttonImageText(_img_, "Button", text_alignment.LEFT) then
				-- 	_OpenPopup_ = false
				-- 	nk.popupClose()
				-- end
				nk.popupEnd()
			end
		end
		nk.layoutRowDynamic(110, 1)
		local ret, begin, count = nk.listViewBegin("zykTest", panel_flag.BORDER, 25, 100)
		if ret then
			nk.layoutRowDynamic(25, 1)
			for i = 1, count, 1 do
				nk.textWidget("Test" .. tostring(begin + i), text_alignment.CENTERED);
			end
			nk.listViewEnd();
		end
		nk.layoutRowStatic(120, 120, 1)
		nk.imageWidget(_img_)

		nk.layoutRowStatic(30, 160, 1)
		local bounds = nk.widgetBounds()
		nk.textWidget("Right click me for menu", text_alignment.LEFT)

		if nk.contextualBegin(0, nk.packVec2(200, 600), bounds) then
			nk.layoutRowDynamic(25, 1)
			-- nk.checkboxText("Menu", &show_menu)
			-- nk_progress(ctx, &prog, 100, NK_MODIFIABLE);
			-- nk_slider_int(ctx, 0, &slider, 16, 1);
			nk.textWidget("Contextual", text_alignment.LEFT)

			if nk.contextualItemText("About", text_alignment.CENTERED) then
				show_app_about = true
			end
			-- nk.selectable_label(ctx, select[0]?"Unselect":"Select", NK_TEXT_LEFT, &select[0]);
			-- nk_selectable_label(ctx, select[1]?"Unselect":"Select", NK_TEXT_LEFT, &select[1]);
			-- nk_selectable_label(ctx, select[2]?"Unselect":"Select", NK_TEXT_LEFT, &select[2]);
			-- nk_selectable_label(ctx, select[3]?"Unselect":"Select", NK_TEXT_LEFT, &select[3]);
			nk.contextualEnd();
		end

		nk.layoutRowStatic(30, 300, 1);
		local bounds = nk.widgetBounds()
		nk.textWidget("Hover me for tooltip", text_alignment.LEFT);
		if nk.inputIsMouseHoveringRect(bounds) then
			nk.tooltip("This is a tooltip")
		end
	end
	nk.endWindow()

	local vbuf, ebuf
	if bcfx.frameId() % 2 == 0 then
		vbuf = vbuf1
		ebuf = ebuf1
	else
		vbuf = vbuf2
		ebuf = ebuf2
	end
	vbuf:clear()
	ebuf:clear()

	nk.convert(cmds, vbuf, ebuf, nullTex)

	vertexMemBuffer:setReplace(vbuf:frontBufferPtr(), vbuf:frontBufferAllocated()) -- 
	indexMemBuffer:setReplace(ebuf:frontBufferPtr(), ebuf:frontBufferAllocated()) -- 
	bcfx.updateDynamicVertexBuffer(vertexHandle, 0, vertexMemBuffer)
	bcfx.updateDynamicIndexBuffer(indexHandle, 0, indexMemBuffer)

	bcfx.setVertexBuffer(0, vertexHandle)
	bcfx.setState(state, bcfx.color.black)
	bcfx.setViewTransform(255, viewMat, projMat)
	local pixelWidth, pixelHeight = glfw.GetFramebufferSize(mainWin)
	bcfx.setViewRect(255, 0, 0, pixelWidth, pixelHeight)

	local flags = bcfx.utils.packSamplerFlags({
		wrapU = texture_wrap.Repeat,
		wrapV = texture_wrap.Repeat,
		filterMin = texture_filter.Linear,
		filterMag = texture_filter.Linear,
	})

	local screenWidth, screenHeight = glfw.GetWindowSize(mainWin)

	nk.drawForEach(cmds, screenWidth, screenHeight, pixelWidth, pixelHeight, function(offset, count, texture, x, y, w, h)
		bcfx.setIndexBuffer(indexHandle, offset, count)
		bcfx.setTexture(0, uniformTex, texture, flags)
		bcfx.setScissor(x, y, w, h) -- in pixel coordinate
		bcfx.submit(255, imguiShader, discard.INDEX_BUFFER | discard.BINDINGS)
	end)
	--[[
	bcfx.setIndexBuffer(indexHandle, 0, 324)
	bcfx.setTexture(0, uniformTex, imageHandle, flags)
	bcfx.submit(255, imguiShader, discard.INDEX_BUFFER | discard.BINDINGS)
	--]]

	nk.clear()
	cmds:clear()
end

return imgui
