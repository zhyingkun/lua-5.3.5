local glfw = require("libglfwrap")
local input_state = glfw.input_state
local keyboard = glfw.keyboard
local modifier_key = glfw.modifier_key

glfw.set_msgh(function(msg)
	print("GLFW pcall error: ", msg, debug.traceback())
end)

glfw.SetErrorCallback(function(code, desc)
	print("GLFW ErrorCallback:", code, desc)
end)
if not glfw.Init() then
	print("Failed to initialize GLFW")
	glfw.Terminate()
	return
end
local window = glfw.CreateWindow(200, 200, "Clipboard Test")
if not window then
	print("Failed to open GLFW window")
	glfw.Terminate()
	return
end

local MODIFIER
if os.sysname == "MacOSX" then
	MODIFIER = modifier_key.SUPER
else
	MODIFIER = modifier_key.CONTROL
end

glfw.SetKeyCallback(window, function(window, key, scancode, action, mods)
	if action ~= input_state.PRESS then return end
	if key == keyboard.ESCAPE then
		glfw.SetWindowShouldClose(window, true)
	elseif key == keyboard.V then
		if mods == MODIFIER then
			local string = glfw.GetClipboardString(NULL)
			print("Clipboard contains", string)
		end
	elseif key == keyboard.C then
		if mods == MODIFIER then
			local string = "Hello GLFW World!"
			glfw.SetClipboardString(NULL, string)
			print("Setting clipboard to", string)
		end
	end
end)

while not glfw.WindowShouldClose(window) do
	glfw.PollEvents()
end

glfw.Terminate()
