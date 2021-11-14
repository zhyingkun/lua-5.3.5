local libuv = require("libuv")
local glfw = require("glfw")

local bcfx = require("bcfx")
local clear_flag = bcfx.clear_flag
local vertex_attrib = bcfx.vertex_attrib
local attrib_type = bcfx.attrib_type
local membuf = bcfx.membuf
local data_type = membuf.data_type
local graphics3d = bcfx.math.graphics3d
local vector = bcfx.math.vector
local texture_wrap = bcfx.texture_wrap
local texture_filter = bcfx.texture_filter
local depth_func = bcfx.depth_func

local loader = require("loader")

local FrameRate = 0.0
local function GetFrameRate()
	return FrameRate
end
local AddOneFrame = (function()
	local FrameCount = 0
	local TotalTime = 0.0
	local MaintainFrameTime = (function()
		local TimeCache = {}
		local TimeIdx = 1
		local MAX_FRAME_COUNT = 30
		for i = 1, MAX_FRAME_COUNT, 1 do
			TimeCache[i] = 0.0
		end
		return function(delta)
			local Cache = TimeCache[TimeIdx]
			TimeCache[TimeIdx] = delta
			TotalTime = TotalTime - Cache + delta
			if FrameCount < MAX_FRAME_COUNT then
				FrameCount = FrameCount + 1
			end
			TimeIdx = TimeIdx % MAX_FRAME_COUNT + 1 -- [1, 30]
		end
	end)()
	return function(delta)
		MaintainFrameTime(delta)
		FrameRate = FrameCount / TotalTime
	end
end)()

local function CreateTriangleBuffer()
	local layout = bcfx.vertexlayout.new()
	layout:add(vertex_attrib.Position, 3, attrib_type.Float, false)
	local layoutHandle = bcfx.createVertexLayout(layout)
	local vertexTbl = {
		-1.0, -1.0, 0.0,
		1.0, -1.0, 0.0,
		0.0, 1.0, 0.0,
	}
	local mem = membuf.MakeMemoryBuffer(data_type.Float, vertexTbl)
	local vertexHandle = bcfx.createVertexBuffer(mem, layoutHandle)

	local layout = bcfx.vertexlayout.new()
	layout:add(vertex_attrib.Color0, 3, attrib_type.Float, false)
	local layoutHandle = bcfx.createVertexLayout(layout)
	local colorTbl = {
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
	}
	local mem = membuf.MakeMemoryBuffer(data_type.Float, colorTbl)
	local colorHandle = bcfx.createVertexBuffer(mem, layoutHandle)

	local indexTbl = {
		0, 1, 2,
	}
	local mem = membuf.MakeMemoryBuffer(data_type.Uint8, indexTbl)
	local idxHandle = bcfx.createIndexBuffer(mem)

	local shaderProgramHandle = loader.LoadProgram("triangle")

	return {
		vertex = vertexHandle,
		color = colorHandle,
		index = idxHandle,
		shader = shaderProgramHandle,
	}
end

local function CreateCubeBuffer()
	local layout = bcfx.vertexlayout.new()
	layout:add(vertex_attrib.Position, 3, attrib_type.Float, false)
	local layoutHandle = bcfx.createVertexLayout(layout)
	local vertexTbl = {
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
	local mem = membuf.MakeMemoryBuffer(data_type.Float, vertexTbl)
	local vertexHandle = bcfx.createVertexBuffer(mem, layoutHandle)

	local layout = bcfx.vertexlayout.new()
	layout:add(vertex_attrib.TexCoord0, 2, attrib_type.Float, false)
	local layoutHandle = bcfx.createVertexLayout(layout)
	local texCoordTbl = {
		0.0, 0.0, -- In OpenGL: front
		1.0, 0.0, -- In Unity : back
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0, -- In OpenGL: back
		1.0, 0.0, -- In Unity : front
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0, -- left
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0, -- right
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0, -- up
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0, -- down
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
	  }
	local mem = membuf.MakeMemoryBuffer(data_type.Float, texCoordTbl)
	local texCoordHandle = bcfx.createVertexBuffer(mem, layoutHandle)

	local indexTbl = {
		0,  1,  2,  0,  2,  3, -- In OpenGL: front, In Unity: back
		4,  5,  6,  4,  6,  7, -- In OpenGL: back, In Unity: front
		8,  9,  10, 8,  10, 11, -- left
		12, 13, 14, 12, 14, 15, -- right
		16, 17, 18, 16, 18, 19, -- up
		20, 21, 22, 20, 22, 23, -- down
	}
	local mem = membuf.MakeMemoryBuffer(data_type.Uint8, indexTbl)
	local idxHandle = bcfx.createIndexBuffer(mem)

	uniformHandle = bcfx.createUniform("my_texture", 0)

	local shaderProgramHandle = loader.LoadProgram("cube")

	local textureHandle = loader.LoadTexture("awesomeface.png")

	return {
		vertex = vertexHandle,
		texCoord = texCoordHandle,
		index = idxHandle,
		uniform = uniformHandle,
		shader = shaderProgramHandle,
		texture = textureHandle,
	}
end

local function SetupViewFull(viewID, win, width, height)
	bcfx.setViewWindow(viewID, win)
	local color = bcfx.color.Pack(51, 76, 76, 255)
	bcfx.setViewClear(viewID, clear_flag.COLOR | clear_flag.DEPTH, color, 1.0, 0)
	bcfx.setViewRect(viewID, 0, 0, width, height)
end
local function SetupViewHalf(viewID, win, width, height)
	bcfx.setViewWindow(viewID, win)
	local color = bcfx.color.Pack(199, 174, 174, 255)
	bcfx.setViewClear(viewID, clear_flag.COLOR | clear_flag.DEPTH, color, 1.0, 0)
	bcfx.setViewRect(viewID, 0, 0, width, height)

	local viewMat = graphics3d.lookAt(
		vector.Vec3(0.0, 0.0, 3.0), -- eye
		vector.Vec3(0.0, 0.0, 0.0), -- center
		vector.Vec3(0.0, 1.0, 0.0) -- up
	)
	local projMat = graphics3d.perspective(
		45.0, -- fovy
		width / height, -- aspect
		0.1, -- zNear
		40.0 -- zFar
	)
	bcfx.setViewTransform(viewID, viewMat, projMat)
end
local function SetupAnotherView(viewID, mainWin)
	local win = glfw.CreateWindow(800, 600, "Another", nil, mainWin)
	if not win then
		print_err("GLFW CreateWindow Error:", glfw.GetError())
		glfw.Terminate()
		return
	end

	bcfx.setViewWindow(viewID, win)
	bcfx.setViewRect(viewID, 0, 0, glfw.GetFramebufferSize(win))
	glfw.SetFramebufferSizeCallback(win, function(window, width, height)
		bcfx.setViewRect(viewID, 0, 0, width, height)
	end)
end

local triangle
local cube

local timer
local function setup(mainWin)
	triangle = CreateTriangleBuffer()
	cube = CreateCubeBuffer()

	glfw.SetFramebufferSizeCallback(mainWin, function(window, width, height)
		bcfx.setViewRect(0, 0, 0, width, height)
		bcfx.setViewRect(1, 0, 0, width / 2, height / 2)
	end)
	local pixelw, pixelh = glfw.GetFramebufferSize(mainWin)
	SetupViewFull(0, mainWin, pixelw, pixelh)
	SetupViewHalf(1, mainWin, math.floor(pixelw / 2), math.floor(pixelh / 2))

	-- SetupAnotherView(2, mainWin)

	-- timer = libuv.timer.new()
	-- timer:start(function()
	-- 	print_err("FrameRate:", string.format("%.2f", GetFrameRate()))
	-- end, 1000, 1000)
end

local angle = 0
local function tick(delta)
	AddOneFrame(delta)

	bcfx.setVertexBuffer(0, triangle.vertex)
	bcfx.setVertexBuffer(1, triangle.color)
	bcfx.setIndexBuffer(triangle.index)
	local mat = graphics3d.scale(vector.Vec3(0.5, 0.5, 0.5))
	bcfx.setTransform(mat)
	bcfx.submit(0, triangle.shader)

	bcfx.setVertexBuffer(0, cube.vertex)
	bcfx.setVertexBuffer(1, cube.texCoord)
	bcfx.setIndexBuffer(cube.index)
	angle = (angle + 1) % 360
	local matScale = graphics3d.scale(vector.Vec3(1.5, 1.5, 1.5))
	local matRotate = graphics3d.rotate(angle, vector.Vec3(1.0, 1.0, 1.0))
	-- local mat = graphics3d.rotate(45, vector.Vec3(1.0, 0.0, 0.0))
	bcfx.setTransform(matRotate * matScale)

	local flags = bcfx.utils.PackSamplerFlags({
		wrapU = texture_wrap.Repeat,
		wrapV = texture_wrap.Repeat,
		filterMin = texture_filter.Linear,
		filterMag = texture_filter.Linear,
	})
	bcfx.setTexture(0, cube.uniform, cube.texture, flags)

	local state = bcfx.utils.PackRenderState({
		depthFunc = depth_func.Less,
	})
	bcfx.setState(state, bcfx.color.black)

	-- bcfx.setViewDebug(1, bcfx.debug.WIREFRAME)
	bcfx.submit(1, cube.shader)

	-- bcfx.setVertexBuffer(0, vertexHandle)
	-- bcfx.setVertexBuffer(1, colorHandle)
	-- bcfx.setIndexBuffer(idxHandle)
	-- bcfx.submit(2, shaderProgramHandle)

	-- local color = bcfx.color.Pack(255, 255, 0, 255)
	-- bcfx.setViewClear(2, clear_flag.COLOR, color, 0.0, 0)
end

return {
	setup = setup,
	tick = tick,
}
