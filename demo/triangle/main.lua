package.path = "demo/triangle/?.lua;" .. package.path

local libuv = require("libuv")
local thread = libuv.thread
local sem = thread.sem

local glfw = require("glfw")
local window_hint = glfw.window_hint
local hint_value = glfw.hint_value
local input_state = glfw.input_state
local func_ptr = glfw.func_ptr

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
	func_ptr.MakeContextCurrent,
	func_ptr.SwapBuffers,
	func_ptr.SwapInterval,
	func_ptr.GetProcAddress
)
bcfx.setMiscFuncs(
	func_ptr.GetTime
)

bcfx.init(window)

libuv.repl_start()

local triangle = require("triangle")
triangle.setup(window)
local colorcircle = require("colorcircle")
colorcircle.setup(window)

require("watch").watch("triangle")
require("watch").watch("colorcircle")

--[[
local graphics3d = bcfx.math.graphics3d
local vector = bcfx.math.vector

local eye = vector.Vec3(0, 0, 3)
local center = vector.Vec3(0, 0, -1)
local up = vector.Vec3(0, 1, 0)
local lookAtMat = graphics3d.lookAt(eye, center, up)
print("lookAtMat:", lookAtMat)

local perspMat = graphics3d.perspective(45, 1.0, 0.1, 50)
print("perspMat:", perspMat)

local orthoMat = graphics3d.orthogonal(-3.4, 8, -6.8, 4, 0.1, 50)
print("orthoMat:", orthoMat)
--]]

local lastTime = 0.0
while not glfw.WindowShouldClose(window) do
	if glfw.GetKey(window, 256) == input_state.PRESS then
		glfw.SetWindowShouldClose(window, true)
	end
	glfw.PollEvents()
	libuv.run_nowait()

	local time = glfw.GetTime()
	local delta = time - lastTime
	lastTime = time

	triangle.tick(delta)
	colorcircle.tick(delta)

	bcfx.apiFrame()
	collectgarbage()
end

-- libuv.repl_stop()

bcfx.shutdown()
glfw.Terminate()
libuv.close()
