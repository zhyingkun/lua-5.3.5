local glfw = require("glfw")
local input_state = glfw.input_state
local keyboard = glfw.keyboard
local modifier_key = glfw.modifier_key

util.setErrorMessageHandler(function(msg)
	return "callback to lua pcall error: " .. msg .. debug.traceback()
end)

glfw.setErrorCallback()
glfw.init()
local window = glfw.createWindow(200, 200, "Clipboard Test")
if not window then
	print("Failed to open GLFW window")
	glfw.terminate()
	return
end

local MODIFIER
if os.sysname == "MacOSX" then
	MODIFIER = modifier_key.SUPER
else
	MODIFIER = modifier_key.CONTROL
end

glfw.setKeyCallback(window, function(window, key, scancode, action, mods)
	if action ~= input_state.PRESS then return end
	if key == keyboard.ESCAPE then
		glfw.setWindowShouldClose(window, true)
	elseif key == keyboard.V then
		if mods == MODIFIER then
			local string = glfw.getClipboardString(NULL)
			print("Clipboard contains", string)
		end
	elseif key == keyboard.C then
		if mods == MODIFIER then
			local string = "Hello GLFW World!"
			glfw.setClipboardString(NULL, string)
			print("Setting clipboard to", string)
		end
	end
end)

while not glfw.windowShouldClose(window) do
	glfw.pollEvents()
end

glfw.terminate()
