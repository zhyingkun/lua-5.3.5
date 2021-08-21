print("Start...")
local glfw = require("libglfwrap")

glfw.SetErrorCallback(function(code, desc)
	print("GLFW ErrorCallback:", code, desc)
end)
glfw.Init()
local window = glfw.CreateWindow(800, 600, "glfw-win-lua")
if window == NULL then
	print("GLFW CreateWindow Error")
	glfw.Terminate()
	return
end
glfw.MakeContextCurrent(window)
glfw.SwapInterval(1)

while not glfw.WindowShouldClose(window) do
	if glfw.GetKey(window, 256) == glfw.PRESS then
		glfw.SetWindowShouldClose(window, true)
	end
	glfw.SwapBuffers(window)
	glfw.PollEvents()
end

glfw.Terminate()
print("End.")
