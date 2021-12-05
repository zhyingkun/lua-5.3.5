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
	func_ptr.GetProcAddress,
	func_ptr.GetFramebufferSize
)
bcfx.setMiscFuncs(
	func_ptr.GetTime
)

bcfx.init(window)

libuv.repl_start()

local loader = require("loader")
loader.SetPathPrefix(require("libdir").dirname(arg[0]) .. "/")

local triangle = require("triangle")
triangle.setup(window)
-- local colorcircle = require("colorcircle")
-- colorcircle.setup(window)

local input = require("input")
input.ImGUIInit(window)
local imgui = require("imgui")
imgui.setup(window)


require("watch").autoReload("triangle")
-- require("watch").autoReload("colorcircle")

require("watch").autoReload("imgui")

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

local function ImageEncodeAsync(mb, x, y, comp, type, sorq, callback) -- stride or quality
	local ptr = bcfx.image.PackImageEncodeParam(mb, x, y, comp, type, sorq);
	libuv.queue_work(bcfx.image.ImageEncodePtr, ptr, function(status, result)
		local mb = bcfx.image.UnpackImageEncodeResult(result)
		print("ImageEncodeAsync Callback:", mb)
		callback(mb)
	end)
end

local function WriteFileAsync(fileName, mb, callback)
	local ptr = bcfx.mbio.PackWriteFileParam(fileName, mb)
	libuv.queue_work(bcfx.mbio.WriteFilePtr, ptr, function(status, result)
		local ret = bcfx.mbio.UnpackWriteFileResult(result)
		callback(ret)
	end)
end

bcfx.setFrameViewCaptureCallback(function(frameId, viewId, width, height, mb)
	print("setFrameViewCaptureCallback Lua", frameId, viewId, width, height, mb)
	local fileName = tostring(frameId) .. "_" .. tostring(viewId) .. "FrameViewCapture.png";
	-- local mb = bcfx.image.ImageEncode(mb, width, height, 4, bcfx.image.image_type.PNG, width * 4)
	-- bcfx.mbio.WriteFile(fileName, mb)
	ImageEncodeAsync(mb, width, height, 4, bcfx.image.image_type.PNG, width * 4, function(mb)
		WriteFileAsync(fileName, mb, function(ret)
			print("setFrameViewCaptureCallback Async WriteFile End:", ret)
		end)
	end)
end)

local lastTime = 0.0
while not glfw.WindowShouldClose(window) do
	libuv.run_nowait()

	input.ImGUIPrePollEvent()
	glfw.PollEvents() -- will fire user input, keyboard or mouse
	input.ImGUIPostPollEvent()

	local time = glfw.GetTime()
	local delta = time - lastTime
	lastTime = time

	triangle.tick(delta)
	-- colorcircle.tick(delta)
	imgui.tick(delta)

	if bcfx.frameId() == 60 and false then
		bcfx.requestCurrentFrameViewCapture(0)
		bcfx.requestCurrentFrameViewCapture(1)
		bcfx.requestCurrentFrameViewCapture(2)
		bcfx.requestCurrentFrameViewCapture(255)
	end

	bcfx.apiFrame()
	collectgarbage()
end

-- libuv.repl_stop()

bcfx.shutdown()
glfw.Terminate()
libuv.close()
