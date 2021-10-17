local libuv = require("libuv")
local glfw = require("glfw")

local bcfx = require("bcfx")
local clear_flag = bcfx.clear_flag
local vertex_attrib = bcfx.vertex_attrib
local attrib_type = bcfx.attrib_type
local shader_type = bcfx.shader_type
local membuf = bcfx.membuf
local data_type = membuf.data_type

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

local function CreateVertexBuffer()
	local layout = bcfx.vertexlayout.new()
	layout:add(vertex_attrib.Position, 3, attrib_type.Float, false)
	local layoutHandle = bcfx.createVertexLayout(layout)
	local vertexTbl = {
		-0.8, -0.8, 0.0,
		0.8, -0.8, 0.0,
		0.0, 0.8, 0.0,
	}
	local mem = membuf.MakeMemoryBuffer(data_type.Float, vertexTbl)
	local vertexHandle = bcfx.createVertexBuffer(mem, layoutHandle)
	return layout, layoutHandle, vertexHandle
end
local function CreateColorBuffer()
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
	return layout, layoutHandle, colorHandle
end
local function CreateIndexBuffer()
	local indexTbl = {
		0, 1, 2,
	}
	local mem = membuf.MakeMemoryBuffer(data_type.Uint8, indexTbl)
	local idxHandle = bcfx.createIndexBuffer(mem)
	return idxHandle
end

local vsStr = [[
#version 410 core
in vec3 a_position;
in vec3 a_color0;

out vec3 v_color0;

void main() {
	gl_Position = vec4(a_position, 1.0);
	v_color0 = a_color0;
}
]]
local fsStr = [[
#version 410 core
in vec3 v_color0;

out vec4 FragColor;

void main() {
	FragColor = vec4(v_color0, 1.0);
}
]]
local function CreateShaderProgram()
	local vertexShaderHandle = bcfx.createShader(vsStr, shader_type.Vertex)
	local fragmentShaderHandle = bcfx.createShader(fsStr, shader_type.Fragment)
	local shaderProgramHandle = bcfx.createProgram(vertexShaderHandle, fragmentShaderHandle)
	return shaderProgramHandle
end

local function SetupViewFull(viewID, win, width, height)
	bcfx.setViewWindow(viewID, win)
	local color = bcfx.color.pack(51, 76, 76, 255)
	bcfx.setViewClear(viewID, clear_flag.COLOR, color, 0.0, 0)
	bcfx.setViewRect(viewID, 0, 0, width, height)
end
local function SetupViewHalf(viewID, win, width, height)
	bcfx.setViewWindow(viewID, win)
	local color = bcfx.color.pack(199, 174, 174, 255)
	bcfx.setViewClear(viewID, clear_flag.COLOR, color, 0.0, 0)
	bcfx.setViewRect(viewID, 0, 0, width, height)
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

local layout
local layoutHandle
local vertexHandle
local colorLayout
local colorLayoutHandle
local colorHandle
local idxHandle
local shaderProgramHandle

local timer
local function setup(mainWin)
	layout, layoutHandle, vertexHandle = CreateVertexBuffer()
	colorLayout, colorLayoutHandle, colorHandle = CreateColorBuffer()
	idxHandle = CreateIndexBuffer()
	shaderProgramHandle = CreateShaderProgram()

	glfw.SetFramebufferSizeCallback(mainWin, function(window, width, height)
		bcfx.setViewRect(0, 0, 0, width, height)
		bcfx.setViewRect(1, 0, 0, width / 2, height / 2)
	end)
	local pixelw, pixelh = glfw.GetFramebufferSize(mainWin)
	SetupViewFull(0, mainWin, pixelw, pixelh)
	SetupViewHalf(1, mainWin, pixelw / 2, pixelh / 2)
	SetupAnotherView(2, mainWin)

	timer = libuv.timer.new()
	timer:start(function()
		print_err("FrameRate:", GetFrameRate())
	end, 1000, 1000)
end

local function tick(delta)
	AddOneFrame(delta)

	bcfx.setVertexBuffer(0, vertexHandle)
	bcfx.setVertexBuffer(1, colorHandle)
	bcfx.setIndexBuffer(idxHandle)
	bcfx.submit(0, shaderProgramHandle)

	bcfx.setVertexBuffer(0, vertexHandle)
	bcfx.setVertexBuffer(1, colorHandle)
	bcfx.setIndexBuffer(idxHandle)
	bcfx.submit(1, shaderProgramHandle)

	bcfx.setVertexBuffer(0, vertexHandle)
	bcfx.setVertexBuffer(1, colorHandle)
	bcfx.setIndexBuffer(idxHandle)
	bcfx.submit(2, shaderProgramHandle)

	local color = bcfx.color.pack(255, 255, 0, 255)
	bcfx.setViewClear(2, clear_flag.COLOR, color, 0.0, 0)
end

return {
	setup = setup,
	tick = tick,
}
