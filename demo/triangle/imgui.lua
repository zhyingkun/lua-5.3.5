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
local font = nk.font
local panel_flag = nk.panel_flag
local text_alignment = nk.text_alignment
local index_type = bcfx.index_type
local clear_flag = bcfx.clear_flag


---@param nk nuklear
---@param pos NuklearVec2
---@param radius number
---@param icons nk_image[]
local function piemenuInternal(pos, radius, icons)
	local ret = -1
	local activeItem = 1

	local hide = nk.StyleItem()
	local ptr = nk.getStylePtrTable()
	nk.stylePushStyleItem(ptr.style_item.window.fixed_background, hide)
	nk.stylePushColor(ptr.color.window.border_color, nk.rgbaToColor(0, 0, 0, 0))
	nk.stylePushVec2(ptr.vec2.window.spacing, nk.packVec2(0, 0))
	nk.stylePushVec2(ptr.vec2.window.padding, nk.packVec2(0, 0))

	local tx, ty, tw, th = nk.unpackRect(nk.windowGetContentRegion())
	local x, y = nk.unpackVec2(pos)
	local bounds = nk.packRect(x - tx - radius, y - radius - ty, 2*radius, 2*radius)
	if nk.popupBegin(nk.popup_type.STATIC, "piemenu", nk.panel_flag.NO_SCROLLBAR, bounds) then
		nk.stylePushVec2(ptr.vec2.window.spacing, nk.packVec2(4, 4))
		nk.stylePushVec2(ptr.vec2.window.padding, nk.packVec2(8, 8))

		local canvas = nk.windowGetCanvas()
		local tx, ty, tw, th = nk.unpackRect(nk.windowGetContentRegion())
		nk.layoutRowDynamic(th, 1)
		local status, bounds = nk.widget()
		local bx, by, bw, bh = nk.unpackRect(bounds)
		nk.fillCircle(canvas, bounds, nk.rgbaToColor(50, 50, 50));
		local step = 2 * math.pi / math.max(1, #icons)
		local aMin = 0.0
		local aMax = step
		local centerX = bx + bw / 2.0
		local centerY = by + bh / 2.0
		local mx, my = nk.unpackVec2(nk.inputGetMousePos())
		local dragX = mx - centerX
		local dragY = my - centerY
		local angle = math.atan(dragY, dragX)
		if angle < 0.0 then angle = angle + 2 * math.pi end
		activeItem = math.tointeger(angle // step) + 1

		for idx, img in ipairs(icons) do
			nk.fillArc(canvas, centerX, centerY, bw / 2.0, aMin, aMax, activeItem == idx and nk.rgbaToColor(45, 100, 255) or nk.rgbaToColor(60, 60, 60))
			local rx = bw / 2.0
			local ry = 0
			local dx = rx * math.cos(aMin) - ry * math.sin(aMin)
			local dy = rx * math.sin(aMin) + ry * math.cos(aMin)
			nk.strokeLine(canvas, centerX, centerY, centerX + dx, centerY + dy, 1.0, nk.rgbaToColor(50, 50, 50))

			local a = aMin + (aMax - aMin) / 2.0
			rx = bw / 2.5
			ry = 0
			local w = 30
			local h = 30
			local x = centerX + rx * math.cos(a) - ry * math.sin(a) - w / 2.0
			local y = centerY + rx * math.sin(a) + ry * math.cos(a) - h / 2.0
			local content = nk.packRect(x, y, w, h)
			nk.drawImage(canvas, content, img, nk.rgbaToColor(255, 255, 255))

			aMin = aMax
			aMax = aMax + step
		end

		-- inner circle
		local x = bx + bw / 2.0 - bw / 4.0
		local y = by + bh / 2.0 - bh / 4.0
		local w = bw / 2.0
		local h = bh / 2.0
		local inner = nk.packRect(x, y ,w, h)
		nk.fillCircle(canvas, inner, nk.rgbaToColor(45, 45, 45))

		-- active icon content
		local boundsW = w / 2.0
		local boundsH = h / 2.0
		local boundsX = x + w / 2.0 - boundsW / 2.0
		local boundsY = y + h / 2.0 - boundsH / 2.0
		local bounds = nk.packRect(boundsX, boundsY, boundsW, boundsH)
		nk.drawImage(canvas, bounds, icons[activeItem], nk.rgbaToColor(255, 255, 255))

		nk.layoutSpaceEnd()
		if not nk.inputIsMouseDown(nk.mouse_button.RIGHT) then
			nk.popupClose()
			ret = activeItem
		end
		nk.stylePopVec2()
		nk.stylePopVec2()
	end
	nk.popupEnd()

	nk.stylePopVec2()
	nk.stylePopVec2()
	nk.stylePopColor()
	nk.stylePopStyleItem()

	return ret
end

local imgui = {}

local myFont
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

local atlas

function imgui.setup(mainWin_)
	mainWin = mainWin_
	atlas = font.Atlas()
	atlas:begin()
	local cfg = font.Config()
	myFont = atlas:addDefault(22, cfg)
	local image, release, ud, width, height = atlas:bake(1)
	-- bcfx.utils.imageWrite("zykTest.png", width, height, 4, image)
	local mb = util.MemBuffer(image, width * height * 4, release, ud)
	mb = bcfx.image.imageFlipVertical(mb, width, height)
	local imageHandle = bcfx.createTexture(mb, width, height, bcfx.texture_format.RGBA8)
	nullTex = atlas:endAtlas(imageHandle)

	local ctx = nk.Context(myFont)
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
	vertexMemBuffer = util.MemBuffer()
	indexMemBuffer = util.MemBuffer()

	uniformTex = bcfx.createUniform("Texture", 0)
	imguiShader = require("loader").LoadProgram("imgui")

	local viewMat = graphics3d.lookAt(
		vector.Vec3(0.0, 0.0, 3.0), -- eye
		vector.Vec3(0.0, 0.0, 0.0), -- center
		vector.Vec3(0.0, 1.0, 0.0) -- up
	)
	local w, h = glfw.getFramebufferSize(mainWin)
	local w, h = glfw.getWindowSize(mainWin)
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
	nk.styleSetFont(myFont)
	if nk.begin("Basic Demo", nk.packRect(0, 0, 400, 300), 
		panel_flag.BORDER  |panel_flag.MOVABLE |panel_flag.TITLE|
		panel_flag.SCALABLE|panel_flag.CLOSABLE|panel_flag.MINIMIZABLE)
	then
		local wStatus = glfw.getKey(mainWin, glfw.keyboard.W)
		local upStatus = glfw.getKey(mainWin, glfw.keyboard.UP)
		local str = "W: " .. tostring(wStatus) .. "    UP: " .. tostring(upStatus)
		nk.layoutRowDynamic(50, 1)
		nk.textWidget(str, text_alignment.LEFT)
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
	if nk.begin("Second Demo", nk.packRect(0, 0, 400, 300), 
		panel_flag.BORDER  |panel_flag.MOVABLE |panel_flag.TITLE|
		panel_flag.SCALABLE|panel_flag.CLOSABLE|panel_flag.MINIMIZABLE)
	then
		if nk.inputIsMouseClickDownInRect(nk.mouse_button.RIGHT, nk.windowGetBounds(), true) then
			piemenuPos = nk.inputGetMousePos()
			piemenuActive = true
		end
		if piemenuActive then
			--local ret = nk.piemenu(piemenuPos, 140, {
			--	_img_, _img_, _img_, _img_, _img_, _img_,
			--})
			local ret = piemenuInternal(piemenuPos, 140, {
				_img_, _img_, _img_, _img_, _img_, _img_,
			})
			if ret == -2 then piemenuActive = false end
			if ret > 0 then
				print("piemenu selected:", ret)
				piemenuActive = false;
			end
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
	local pixelWidth, pixelHeight = glfw.getFramebufferSize(mainWin)
	bcfx.setViewRect(255, 0, 0, pixelWidth, pixelHeight)

	local flags = bcfx.utils.packSamplerFlags({
		wrapU = texture_wrap.Repeat,
		wrapV = texture_wrap.Repeat,
		filterMin = texture_filter.Linear,
		filterMag = texture_filter.Linear,
	})

	local screenWidth, screenHeight = glfw.getWindowSize(mainWin)

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


local piemenuPos
local piemenuActive
---@param nk nuklear
---@param radius number
---@param icons nk_image[]
local function piemenu(nk, radius, icons)
	if nk.inputIsMouseDown(nk.mouse_button.RIGHT) then
		piemenuPos = 0 -- TODO: Get Mouse Position
		piemenuActive = true
	end
	local ret = 1
	if piemenuActive then
		local ret = piemenuInternal(nk. piemenuPos, radius, icons)
		if ret ~= -1 then piemenuActive = false end
		if ret <= 0 then ret = 1 end
	end
	return ret
end

return imgui
