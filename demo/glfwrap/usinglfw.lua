print("Start...")
local glfw = require("glfw")
local input_state = glfw.input_state

util.setErrorMessageHandler(function(msg)
	return "callback to lua pcall error: " .. msg .. debug.traceback()
end)

glfw.setErrorCallback()
glfw.init()
windows = {}
local last = nil
for i = 1, 5, 1 do
	local window = glfw.createWindow(800, 600, "glfw-win-lua" .. tostring(i), nil, last)
	if not window then
		print("GLFW CreateWindow Error")
		glfw.terminate()
		return
	end
	glfw.setWindowCloseCallback(window, function(window)
		print("Close Window:", window)
		glfw.destroyWindow(window)
	end)
	last = window
	table.insert(windows, window)
end
local window = glfw.createWindow(800, 600, "glfw-win-lua")
if not window then
	print("GLFW CreateWindow Error")
	glfw.terminate()
	return
end
glfw.makeContextCurrent(window)
glfw.swapInterval(1)

while not glfw.windowShouldClose(window) do
	if glfw.getKey(window, 256) == input_state.PRESS then
		glfw.setWindowShouldClose(window, true)
	end
	glfw.swapBuffers(window)
	glfw.pollEvents()
end

print(tostring(glfw.getMonitors(), 16))
print(tostring(glfw.getVideoModes(glfw.getPrimaryMonitor()), 16))

windows = nil
window = nil

glfw.terminate()
print("End.")
