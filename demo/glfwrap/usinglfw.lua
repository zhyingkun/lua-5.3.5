print("Start...")
local glfw = require("libglfwrap")
local input_state = glfw.input_state

glfw.set_msgh(function(msg)
	print("GLFW pcall error: ", msg, debug.traceback())
end)

glfw.SetErrorCallback(function(code, desc)
	print("GLFW ErrorCallback:", code, desc)
end)
if not glfw.Init() then
	print("GLFW Init Error")
	glfw.Terminate()
	return
end
windows = {}
local last = nil
for i = 1, 5, 1 do
	local window = glfw.CreateWindow(800, 600, "glfw-win-lua" .. tostring(i), nil, last)
	if not window then
		print("GLFW CreateWindow Error")
		glfw.Terminate()
		return
	end
	glfw.SetWindowCloseCallback(window, function(window)
		print("Close Window:", window)
		glfw.DestroyWindow(window)
	end)
	last = window
	table.insert(windows, window)
end
local window = glfw.CreateWindow(800, 600, "glfw-win-lua")
if not window then
	print("GLFW CreateWindow Error")
	glfw.Terminate()
	return
end
glfw.MakeContextCurrent(window)
glfw.SwapInterval(1)

while not glfw.WindowShouldClose(window) do
	if glfw.GetKey(window, 256) == input_state.PRESS then
		glfw.SetWindowShouldClose(window, true)
	end
	glfw.SwapBuffers(window)
	glfw.PollEvents()
end

print(tostring(glfw.GetMonitors(), 16))
print(tostring(glfw.GetVideoModes(glfw.GetPrimaryMonitor()), 16))

glfw.Terminate()
print("End.")
