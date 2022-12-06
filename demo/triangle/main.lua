package.path = "demo/triangle/?.lua;" .. package.path

local libuv = require("libuv")
local mbio = libuv.mbio

local glfw = require("glfw")
local window_hint = glfw.window_hint
local hint_value = glfw.hint_value
local input_state = glfw.input_state

local bcfx = require("bcfx")

--[[
package.cpath = package.cpath .. ';/Users/zyk/Library/Application Support/JetBrains/IdeaIC2021.1/plugins/EmmyLua/classes/debugger/emmy/mac/?.dylib'
local dbg = require('emmy_core')
dbg.tcpConnect('localhost', 9966)
--]]

util.setErrorMessageHandler(function(msg)
	return "callback to lua pcall error: " .. msg .. debug.traceback()
end)

glfw.setErrorCallback()

glfw.init()

glfw.windowHint(window_hint.CONTEXT_VERSION_MAJOR, 4)
glfw.windowHint(window_hint.CONTEXT_VERSION_MINOR, 1)
glfw.windowHint(window_hint.OPENGL_PROFILE, hint_value.OPENGL_CORE_PROFILE)
glfw.windowHint(window_hint.OPENGL_FORWARD_COMPAT, hint_value.TRUE)
glfw.windowHint(window_hint.SRGB_CAPABLE, hint_value.TRUE)

local window = glfw.createWindow(800, 600, "Triangle")
if not window then
	print("GLFW CreateWindow Error:", glfw.getError())
	glfw.terminate()
	return
end

bcfx.init(window)

libuv.setLoop()

local repl = require("repl")
repl.init(libuv)
--repl.startAsync()
--repl.startAsync(repl.evalDefault)
--repl.startAsync(repl.evalDefaultLua)
repl.serverStartAsync()

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

local function runInCoroutine(func)
	coroutine.wrap(func)()
end

bcfx.setFrameViewCaptureCallback(function(frameId, viewId, width, height, mb)
	print("setFrameViewCaptureCallback Lua", frameId, viewId, width, height, mb)
	local fileName = tostring(frameId) .. "_" .. tostring(viewId) .. "FrameViewCapture.png";
	-- local mb = bcfx.image.imageEncode(mb, width, height, 4, bcfx.image.image_type.PNG, width * 4)
	-- mbio.writeFile(fileName, mb)
	-- bcfx.image.imageEncodeAsync(mb, width, height, 4, bcfx.image.image_type.PNG, width * 4, function(mb)
	-- 	mbio.writeFileAsync(fileName, mb, function(ret)
	-- 		print("setFrameViewCaptureCallback Async WriteFile End:", ret)
	-- 	end)
	-- end)
	runInCoroutine(function()
		local mb = bcfx.image.imageEncodeAsyncWait(mb, width, height, 4, bcfx.image.image_type.PNG, width * 4)
		local ret = mbio.writeFileAsyncWait(fileName, mb)
		print("setFrameViewCaptureCallback Async WriteFile End:", ret)
	end)
end)

local lastTime = 0.0
while not glfw.windowShouldClose(window) do
	libuv.runNoWait()

	input.ImGUIPrePollEvent()
	glfw.pollEvents() -- will fire user input, keyboard or mouse
	input.ImGUIPostPollEvent()

	local time = glfw.getTime()
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
glfw.terminate()
libuv.close()
