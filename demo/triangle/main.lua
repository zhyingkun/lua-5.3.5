local libuv = require("libuv")
local thread = libuv.thread
local sem = thread.sem

local glfw = require("glfw")
local window_hint = glfw.window_hint
local hint_value = glfw.hint_value
local input_state = glfw.input_state
local winctx = glfw.func_ptr

local bcfx = require("bcfx")
local clear_flag = bcfx.clear_flag
local vertex_attrib = bcfx.vertex_attrib
local attrib_type = bcfx.attrib_type
local shader_type = bcfx.shader_type

glfw.set_msgh(function(msg)
	print("GLFW pcall error: ", msg, debug.traceback())
end)
glfw.SetErrorCallback()

glfw.Init()

glfw.WindowHint(window_hint.CONTEXT_VERSION_MAJOR, 4)
glfw.WindowHint(window_hint.CONTEXT_VERSION_MINOR, 1)
glfw.WindowHint(window_hint.OPENGL_PROFILE, hint_value.OPENGL_CORE_PROFILE)
glfw.WindowHint(window_hint.OPENGL_FORWARD_COMPAT, hint_value.TRUE)

local window = glfw.CreateWindow(800, 600, "Triangle")
if not window then
	print("GLFW CreateWindow Error:", glfw.GetError())
	glfw.Terminate()
	return
end

bcfx.setThreadFuncs(
	thread.thread_create,
	thread.thread_self,
	thread.thread_invalid,
	thread.thread_join,
	thread.thread_equal
)
bcfx.setSemFuncs(
	sem.sem_init,
	sem.sem_destroy,
	sem.sem_post,
	sem.sem_wait,
	sem.sem_trywait
)
bcfx.setWinCtxFuncs(
	winctx.MakeContextCurrent,
	winctx.SwapBuffers,
	winctx.SwapInterval,
	winctx.GetProcAddress
)

bcfx.init(window)

local layout = bcfx.vertexlayout.new()
layout:add(vertex_attrib.Position, 3, attrib_type.Float, false)
local layoutHandle = bcfx.createVertexLayout(layout)
local vertexTbl = {
	-0.6, -0.6, 0.0,
	0.6, -0.6, 0.0,
	0.0, 0.6, 0.0,
}
local ptr, sz = bcfx.MakeFloatArray(vertexTbl, #vertexTbl)
local vertexHandle = bcfx.createVertexBuffer(ptr, sz, layoutHandle)

local layoutColor = bcfx.vertexlayout.new()
layoutColor:add(vertex_attrib.Color0, 3, attrib_type.Float, false)
local layoutHandle = bcfx.createVertexLayout(layoutColor)
local colorTbl = {
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0,
}
local ptr, sz = bcfx.MakeFloatArray(colorTbl, #colorTbl)
local colorHandle = bcfx.createVertexBuffer(ptr, sz, layoutHandle)

local indexTbl = {
	0, 1, 2,
}
local ptr, sz = bcfx.MakeUintArray(indexTbl, #indexTbl)
local idxHandle = bcfx.createIndexBuffer(ptr, sz)

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
local vertexShaderHandle = bcfx.createShader(vsStr, shader_type.Vertex)
local fragmentShaderHandle = bcfx.createShader(fsStr, shader_type.Fragment)
local shaderProgramHandle = bcfx.createProgram(vertexShaderHandle, fragmentShaderHandle)

local viewID = 0
-- bcfx.setViewWindow(viewID, window)
local color = bcfx.PackColor(51, 76, 76, 255)
bcfx.setViewClear(viewID, clear_flag.COLOR, color, 0.0, 0)
bcfx.setViewRect(viewID, 0, 0, glfw.GetFramebufferSize(window))
-- bcfx.setViewRect(viewID, 0, 0, glfw.GetWindowSize(window))

local viewID1 = 1
bcfx.setViewWindow(viewID1, window)
local color = bcfx.PackColor(255, 255, 76, 255)
bcfx.setViewClear(viewID1, clear_flag.COLOR, color, 0.0, 0)
bcfx.setViewRect(viewID1, 0, 0, 600, 600)

glfw.SetFramebufferSizeCallback(window, function(window, width, height)
	bcfx.setViewRect(viewID, 0, 0, width, height)
end)


local window2 = glfw.CreateWindow(800, 600, "Another", nil, window)
if not window2 then
	print("GLFW CreateWindow Error:", glfw.GetError())
	glfw.Terminate()
	return
end

local viewID2 = 2
bcfx.setViewWindow(viewID2, window2)
local color = bcfx.PackColor(128, 0, 0, 255)
bcfx.setViewClear(viewID2, clear_flag.COLOR, color, 0.0, 0)
bcfx.setViewRect(viewID2, 0, 0, glfw.GetFramebufferSize(window2)) -- 

glfw.SetFramebufferSizeCallback(window2, function(window, width, height)
	bcfx.setViewRect(viewID2, 0, 0, width, height)
end)

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

-- local timer = libuv.timer.new()
-- timer:start(function()
-- 	print("FrameRate:", GetFrameRate())
-- end, 1000, 1000)

libuv.repl_start()

local lastTime = 0.0
while not glfw.WindowShouldClose(window) do
	if glfw.GetKey(window, 256) == input_state.PRESS then
		glfw.SetWindowShouldClose(window, true)
	end
	glfw.PollEvents()
	libuv.run_nowait()

	local time = glfw.GetTime()
	AddOneFrame(time - lastTime)
	lastTime = time

	bcfx.setVertexBuffer(0, vertexHandle)
	bcfx.setVertexBuffer(1, colorHandle)
	bcfx.setIndexBuffer(idxHandle)

	bcfx.submit(viewID, shaderProgramHandle)

	-- bcfx.setVertexBuffer(0, vertexHandle)
	-- bcfx.setVertexBuffer(1, colorHandle)
	-- bcfx.setIndexBuffer(idxHandle)

	bcfx.submit(viewID1, shaderProgramHandle)

	-- bcfx.setVertexBuffer(0, vertexHandle)
	-- bcfx.setVertexBuffer(1, colorHandle)
	-- bcfx.setIndexBuffer(idxHandle)

	bcfx.submit(viewID2, shaderProgramHandle)

	bcfx.apiFrame()
end

bcfx.shutdown()
glfw.Terminate()
libuv.close()
