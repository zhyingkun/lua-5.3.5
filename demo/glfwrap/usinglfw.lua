print("Start...")
local glfw = require("libglfwrap")
local input_state = glfw.input_state

glfw.setErrorMessageHandler(function(msg)
	print("GLFW pcall error: ", msg, debug.traceback())
end)

glfw.setErrorCallback()
if not glfw.init() then
	print("GLFW Init Error")
	glfw.terminate()
	return
end
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

glfw.terminate()
print("End.")
