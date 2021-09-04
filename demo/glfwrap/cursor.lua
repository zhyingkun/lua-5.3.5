local glfw = require("libglfwrap")
local input_state = glfw.input_state
local keyboard = glfw.keyboard
local modifier_key = glfw.modifier_key
local standard_cursor_shape = glfw.standard_cursor_shape
local window_hint = glfw.window_hint
local input_mode = glfw.input_mode
local cursor_mode = glfw.cursor_mode

glfw.set_msgh(function(msg)
	print("GLFW pcall error: ", msg, debug.traceback())
end)

glfw.SetErrorCallback(function(code, desc)
	print("GLFW ErrorCallback:", code, desc)
end)

local function CHECK(value, msg)
	if not value then
		glfw.Terminate()
		error(msg)
	end
end

CHECK(glfw.Init(), "Failed to initialize GLFW")

local function create_tracking_cursor()
	local i = 0
	local width = 32
	local height = 32
	local value = coroutine.wrap(function()
		for y = 0, width - 1, 1 do
			for x = 0, height - 1, 1 do
				if x == 7 or y == 7 then
					coroutine.yield(255)
					coroutine.yield(0)
					coroutine.yield(0)
					coroutine.yield(255)
				else
					coroutine.yield(0)
					coroutine.yield(0)
					coroutine.yield(0)
					coroutine.yield(0)
				end
			end
		end
	end)
	local buffer = string.char(function(idx)
		return value()
	end)
	local image = glfw.CreateGLFWimage(width, height, buffer)
	return glfw.CreateCursor(image, width / 2, height / 2)
end
local tracking_cursor = create_tracking_cursor()
CHECK(tracking_cursor, "Failed to create cursor")

local function star(x, y, t)
	local c = 64 / 2.0
	local i = 0.25 * math.sin(2 * math.pi * t) + 0.75
	local k = 64 * 0.046875 * i
	local dist = math.sqrt((x - c)^2 + (y - c)^2)
	local salpha = 1 - dist / c
	local xalpha = x == c and c or k / math.abs(x - c)
	local yalpha = y == c and c or k / math.abs(y - c)
	return math.max(0, math.min(1, i * salpha * 0.2 + salpha * xalpha * yalpha))
end
local function create_cursor_frame(t)
	local i = 0
	local width = 64
	local height = 64
	local value = coroutine.wrap(function()
		for y = 0, width - 1, 1 do
			for x = 0, height - 1, 1 do
				coroutine.yield(255)
				coroutine.yield(255)
				coroutine.yield(255)
				coroutine.yield(math.floor(255 * star(x, y, t)))
			end
		end
	end)
	local buffer = string.char(function(idx)
		return value()
	end)
	local image = glfw.CreateGLFWimage(width, height, buffer)
	return glfw.CreateCursor(image, width / 2, height / 2)
end
local star_cursors = {}
local CURSOR_FRAME_COUNT = 60
for i = 0, CURSOR_FRAME_COUNT - 1, 1 do
	local star_cursor = create_cursor_frame(i / CURSOR_FRAME_COUNT)
	CHECK(star_cursor, "Failed to create frame cursor")
	table.insert(star_cursors, star_cursor)
end

local standard_cursors = {}
for _, shape in pairs(standard_cursor_shape) do
	local standard_cursor = glfw.CreateStandardCursor(shape)
	CHECK(standard_cursor, "Failed to create standard cursor")
	table.insert(standard_cursors, standard_cursor)
end

glfw.WindowHint(window_hint.CONTEXT_VERSION_MAJOR, 2)
glfw.WindowHint(window_hint.CONTEXT_VERSION_MINOR, 0)

local window = glfw.CreateWindow(640, 480, "Cursor Test")
CHECK(window, "Failed to open GLFW window")

glfw.MakeContextCurrent(window)

local cursor_x
local cursor_y
local swap_interval = 1
local wait_events = true
local animate_cursor = false

cursor_x, cursor_y = glfw.GetCursorPos(window)
print("Cursor position:", cursor_x, cursor_y)

glfw.SetCursorPosCallback(window, function(window, x, y)
	print(string.format("%0.3f: Cursor position: %f %f (%+f %+f)", glfw.GetTime(), x, y, x - cursor_x, y - cursor_y))
	cursor_x = x
	cursor_y = y
end)

local MODIFIER
if os.sysname == "MacOSX" then
	MODIFIER = modifier_key.SUPER
else
	MODIFIER = modifier_key.CONTROL
end

glfw.SetKeyCallback(window, function(window, key, scancode, action, mods)
	if action ~= input_state.PRESS then return end
	if key == keyboard.A then
		if animate_cursor then
			glfw.SetCursor(window, NULL)
		end
		animate_cursor = not animate_cursor
	elseif key == keyboard.ESCAPE then
		if glfw.GetInputMode(window, input_mode.CURSOR) ~= cursor_mode.DISABLED then
			glfw.SetWindowShouldClose(window, true)
		end
		glfw.SetInputMode(window, input_mode.CURSOR, cursor_mode.NORMAL)
		cursor_x, cursor_y = glfw.GetCursorPos(window)
		print("(( cursor is normal ))")
	elseif key == keyboard.N then
		glfw.SetInputMode(window, input_mode.CURSOR, cursor_mode.NORMAL)
		cursor_x, cursor_y = glfw.GetCursorPos(window)
		print("(( cursor is normal ))")
	elseif key == keyboard.D then
		glfw.SetInputMode(window, input_mode.CURSOR, cursor_mode.DISABLED)
		print("(( cursor is disabled ))")
	elseif key == keyboard.H then
		glfw.SetInputMode(window, input_mode.CURSOR, cursor_mode.HIDDEN)
		print("(( cursor is hidden ))")
	elseif key == keyboard.R then
		if glfw.RawMouseMotionSupported() then
			if glfw.GetInputMode(window, input_mode.RAW_MOUSE_MOTION) ~= 0 then
				glfw.SetInputMode(window, input_mode.RAW_MOUSE_MOTION, 0)
				print("(( raw input is disabled ))")
			else
				glfw.SetInputMode(window, input_mode.RAW_MOUSE_MOTION, 1)
				print("(( raw input is enabled ))")
			end
		end
	elseif key == keyboard.SPACE then
		swap_interval = 1 - swap_interval
		print(string.format("(( swap interval: %i ))", swap_interval))
		glfw.SwapInterval(swap_interval)
	elseif key == keyboard.W then
		wait_events = not wait_events
		print("(( %sing for events ))", wait_events and "wait" or "poll")
	elseif key == keyboard.T then
	elseif key == keyboard.P then
		local x, y = glfw.GetCursorPos(window)

		print(string.format("Query before set: %f %f (%+f %+f)", x, y, x - cursor_x, y - cursor_y))
		cursor_x = x
		cursor_y = y

		glfw.SetCursorPos(window, cursor_x, cursor_y)
		x, y = glfw.GetCursorPos(window)

		print(string.format("Query after set: %f %f (%+f %+f)", x, y, x - cursor_x, y - cursor_y))
		cursor_x = x
		cursor_y = y
	elseif key == keyboard.UP then
		glfw.SetCursorPos(window, 0, 0)
		cursor_x, cursor_y = glfw.GetCursorPos(window)
	elseif key == keyboard.DOWN then
		local width, height = glfw.GetWindowSize(window)
		glfw.SetCursorPos(window, width - 1, height - 1)
		cursor_x, cursor_y = glfw.GetCursorPos(window)
	elseif key == keyboard._0 then
		glfw.SetCursor(window, NULL)
	elseif key == keyboard._1 then
		glfw.SetCursor(window, standard_cursors[1])
	elseif key == keyboard._2 then
		glfw.SetCursor(window, standard_cursors[2])
	elseif key == keyboard._3 then
		glfw.SetCursor(window, standard_cursors[3])
	elseif key == keyboard._4 then
		glfw.SetCursor(window, standard_cursors[4])
	elseif key == keyboard._5 then
		glfw.SetCursor(window, standard_cursors[5])
	elseif key == keyboard._6 then
		glfw.SetCursor(window, standard_cursors[6])
	elseif key == keyboard.F11 or key == keyboard.ENTER then
		if mods ~= modifier_key.ALT then return end
		if glfw.GetWindowMonitor(window) then
			glfw.SetWindowMonitor(window, NULL, x, y, width, height, 0)
		else
			local monitor = glfw.GetPrimaryMonitor()
			local mode = glfw.GetVideoMode(monitor)
			x, y = glfw.GetWindowPos(window)
			width, height = glfw.GetWindowSize(window)
			glfw.SetWindowMonitor(window, monitor, 0, 0, mode:width(), mode:height(), mode:refreshRate())
		end
	end
end)

while not glfw.WindowShouldClose(window) do
	glfw.SwapBuffers(window)

	if animate_cursor then
		local i = math.floor(glfw.GetTime() * 30.0) % CURSOR_FRAME_COUNT + 1
		if current_frame ~= star_cursors[i] then
			glfw.SetCursor(window, star_cursors[i])
			current_frame = star_cursors[i]
		end
	else
		current_frame = nil
	end

	if wait_events then
		if animate_cursor then
			glfw.WaitEventsTimeout(1.0 / 30.0)
		else
			glfw.WaitEvents()
		end
	else
		glfw.PollEvents()
	end
end

glfw.DestroyWindow(window)
for _, cursor in pairs(star_cursors) do
	glfw.DestroyCursor(cursor)
end
for _, cursor in pairs(standard_cursors) do
	glfw.DestroyCursor(cursor)
end
glfw.Terminate()
