local libglfw = require("libglfwrap")

---@class glfw:table
local glfw = {}

--[[
** {======================================================
** Context
** =======================================================
--]]

---@class GLFWwindow:lightuserdata

---@param window GLFWwindow
function glfw.makeContextCurrent(window)
	libglfw.MakeContextCurrent(window)
end
---@return GLFWwindow
function glfw.getCurrentContext()
	return libglfw.GetCurrentContext()
end
---@param interval integer
function glfw.swapInterval(interval)
	libglfw.SwapInterval(interval)
end
---@param extension string
---@return boolean
function glfw.extensionSupported(extension)
	return libglfw.ExtensionSupported(extension)
end
---@param procname string
---@return lightuserdata
function glfw.getProcAddress(procname)
	return libglfw.GetProcAddress(procname)
end

-- }======================================================

--[[
** {======================================================
** Initialization, version and error
** =======================================================
--]]

function glfw.init()
	libglfw.Init()
end
function glfw.terminate()
	libglfw.Terminate()
end
---@param hint glfw_init_hint
---@param value glfw_hint_value
function glfw.initHint(hint, value)
	libglfw.InitHint(hint, value)
end
---@return integer, integer, integer @major, minor, rev
function glfw.getVersion()
	return libglfw.GetVersion()
end
---@return string
function glfw.getVersionString()
	return libglfw.GetVersionString()
end
---@return integer, string
function glfw.getError()
	return libglfw.GetError()
end
function glfw.setErrorCallback()
	libglfw.SetErrorCallback()
end

---@class glfw_err_code
---@field public NO_ERROR integer
---@field public NOT_INITIALIZED integer
---@field public NO_CURRENT_CONTEXT integer
---@field public INVALID_ENUM integer
---@field public INVALID_VALUE integer
---@field public OUT_OF_MEMORY integer
---@field public API_UNAVAILABLE integer
---@field public VERSION_UNAVAILABLE integer
---@field public PLATFORM_ERROR integer
---@field public FORMAT_UNAVAILABLE integer
---@field public NO_WINDOW_CONTEXT integer

---@type glfw_err_code
glfw.err_code = libglfw.err_code

---@class glfw_init_hint
---@field public JOYSTICK_HAT_BUTTONS integer
---@field public COCOA_CHDIR_RESOURCES integer
---@field public COCOA_MENUBAR integer

---@type glfw_init_hint
glfw.init_hint = glfw.init_hint

-- }======================================================

--[[
** {======================================================
** Vulkan support
** =======================================================
--]]

---@return boolean
function glfw.vulkanSupported()
	return libglfw.VulkanSupported()
end
---@return string[]
function glfw.getRequiredInstanceExtensions()
	return libglfw.GetRequiredInstanceExtensions()
end

-- }======================================================

--[[
** {======================================================
** Input
** =======================================================
--]]

---@param window GLFWwindow
---@param mode glfw_input_mode
---@return integer
function glfw.getInputMode(window, mode)
	return libglfw.GetInputMode(window, mode)
end
---@param window GLFWwindow
---@param mode glfw_input_mode
---@param value integer
function glfw.setInputMode(window, mode, value)
	libglfw.SetInputMode(window, mode, value)
end
---@return boolean
function glfw.rawMouseMotionSupported()
	return libglfw.RawMouseMotionSupported()
end
---@param key glfw_keyboard
---@param scancode integer
---@return string
function glfw.getKeyName(key, scancode)
	return libglfw.GetKeyName(key, scancode)
end
---@param key glfw_keyboard
---@return integer
function glfw.getKeyScancode(key)
	return libglfw.GetKeyScancode(key)
end
---@param window GLFWwindow
---@param key glfw_keyboard
---@return glfw_input_state
function glfw.getKey(window, key)
	return libglfw.GetKey(window, key)
end
---@param window GLFWwindow
---@param button glfw_mouse_button
---@return glfw_input_state
function glfw.getMouseButton(window, button)
	return libglfw.GetMouseButton(window, button)
end
---@param window GLFWwindow
---@return number, number
function glfw.getCursorPos(window)
	return libglfw.GetCursorPos(window)
end
---@param window GLFWwindow
---@param xpos number
---@param ypos number
function glfw.setCursorPos(window, xpos, ypos)
	libglfw.SetCursorPos(window, xpos, ypos)
end

---@param image GLFWimage
---@param xhot integer
---@param yhot integer
---@return boolean
function glfw.createCursor(image, xhot, yhot)
	return libglfw.CreateCursor(image, xhot, yhot)
end

---@class GLFWcursor:lightuserdata

---@param shape glfw_standard_cursor_shape
---@return GLFWcursor
function glfw.createStandardCursor(shape)
	return libglfw.CreateStandardCursor(shape)
end
---@param cursor GLFWcursor
function glfw.destroyCursor(cursor)
	libglfw.DestroyCursor(cursor)
end
---@param window GLFWwindow
---@param cursor GLFWcursor
function glfw.setCursor(window, cursor)
	libglfw.SetCursor(window, cursor)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, key:glfw_keyboard, scancode:integer, action:glfw_input_state, mods:glfw_modifier_key):void
function glfw.setKeyCallback(window, callback)
	libglfw.SetKeyCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, codepoint:integer):void
function glfw.setCharCallback(window, callback)
	libglfw.SetCharCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, codepoint:integer, mods:glfw_modifier_key):void
function glfw.setCharModsCallback(window, callback)
	libglfw.SetCharModsCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, button:glfw_mouse_button, action:glfw_input_state, mods:glfw_modifier_key):void
function glfw.setMouseButtonCallback(window, callback)
	libglfw.SetMouseButtonCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, xpos:number, ypos:number):void
function glfw.setCursorPosCallback(window, callback)
	libglfw.SetCursorPosCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, entered:boolean):void
function glfw.setCursorEnterCallback(window, callback)
	libglfw.SetCursorEnterCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, xoffset:number, yoffset:number):void
function glfw.setScrollCallback(window, callback)
	libglfw.SetScrollCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, paths:string[]):void
function glfw.setDropCallback(window, callback)
	libglfw.SetDropCallback(window, callback)
end
---@param jid glfw_joystick
---@return boolean
function glfw.joystickPresent(jid)
	return libglfw.JoystickPresent(jid)
end
---@param jid glfw_joystick
---@return nil | number[]
function glfw.getJoystickAxes(jid)
	return libglfw.GetJoystickAxes(jid)
end
---@param jid glfw_joystick
---@return nil | glfw_input_state[]
function glfw.getJoystickButtons(jid)
	return libglfw.GetJoystickButtons(jid)
end
---@param jid glfw_joystick
---@return nil | glfw_input_state[]
function glfw.getJoystickHats(jid)
	return libglfw.GetJoystickHats(jid)
end
---@param jid glfw_joystick
---@return string
function glfw.getJoystickName(jid)
	return libglfw.GetJoystickName(jid)
end
---@param jid glfw_joystick
---@return string
function glfw.getJoystickGUID(jid)
	return libglfw.GetJoystickGUID(jid)
end
---@param jid glfw_joystick
---@return boolean
function glfw.joystickIsGamepad(jid)
	return libglfw.JoystickIsGamepad(jid)
end
---@param callback fun(jid:glfw_joystick, event:glfw_device_event):void
function glfw.setJoystickCallback(callback)
	libglfw.SetJoystickCallback(callback)
end
---@param str string
---@return boolean
function glfw.updateGamepadMappings(str)
	return libglfw.UpdateGamepadMappings(str)
end
---@param jid glfw_joystick
---@return string
function glfw.getGamepadName(jid)
	return libglfw.GetGamepadName(jid)
end

---@class GLFWgamepadstate:table
---@field buttons glfw_input_state[]
---@field axes number[]

---@param jid glfw_joystick
---@return nil | GLFWgamepadstate
function glfw.getGamepadState(jid)
	return libglfw.GetGamepadState(jid)
end
---@param window GLFWwindow | nil
---@param str string
function glfw.setClipboardString(window, str)
	libglfw.SetClipboardString(window, str)
end
---@overload fun():string
---@overload fun(window:GLFWwindow):string
---@param window GLFWwindow | nil
---@return string
function glfw.getClipboardString(window)
	return libglfw.GetClipboardString(window)
end
---@return number
function glfw.getTime()
	return libglfw.GetTime()
end
---@param time number
function glfw.setTime(time)
	libglfw.SetTime(time)
end
---@return integer
function glfw.getTimerValue()
	return libglfw.GetTimerValue()
end
---@return integer
function glfw.getTimerFrequency()
	return libglfw.GetTimerFrequency()
end

---@class glfw_gamepad_axis
---@field public LEFT_X integer
---@field public LEFT_Y integer
---@field public RIGHT_X integer
---@field public RIGHT_Y integer
---@field public LEFT_TRIGGER integer
---@field public RIGHT_TRIGGER integer
---@field public LAST integer

---@type glfw_gamepad_axis
glfw.gamepad_axis = libglfw.gamepad_axis

---@class glfw_gamepad_button
---@field public A integer
---@field public B integer
---@field public X integer
---@field public Y integer
---@field public LEFT_BUMPER integer
---@field public RIGHT_BUMPER integer
---@field public BACK integer
---@field public START integer
---@field public GUIDE integer
---@field public LEFT_THUMB integer
---@field public RIGHT_THUMB integer
---@field public DPAD_UP integer
---@field public DPAD_RIGHT integer
---@field public DPAD_DOWN integer
---@field public DPAD_LEFT integer
---@field public LAST integer
---@field public CROSS integer
---@field public CIRCLE integer
---@field public SQUARE integer
---@field public TRIANGLE integer

---@type glfw_gamepad_button
glfw.gamepad_button = libglfw.gamepad_button

---@class glfw_joystick_hat
---@field public CENTERED integer
---@field public UP integer
---@field public RIGHT integer
---@field public DOWN integer
---@field public LEFT integer
---@field public RIGHT_UP integer
---@field public RIGHT_DOWN integer
---@field public LEFT_UP integer
---@field public LEFT_DOWN integer

---@type glfw_joystick_hat
glfw.joystick_hat = libglfw.joystick_hat

---@class glfw_joystick
---@field public _1 integer
---@field public _2 integer
---@field public _3 integer
---@field public _4 integer
---@field public _5 integer
---@field public _6 integer
---@field public _7 integer
---@field public _8 integer
---@field public _9 integer
---@field public _10 integer
---@field public _11 integer
---@field public _12 integer
---@field public _13 integer
---@field public _14 integer
---@field public _15 integer
---@field public _16 integer
---@field public LAST integer

---@type glfw_joystick
glfw.joystick = libglfw.joystick

---@class glfw_keyboard
---@field public UNKNOWN integer
---@field public SPACE integer
---@field public APOSTROPHE integer
---@field public COMMA integer
---@field public MINUS integer
---@field public PERIOD integer
---@field public SLASH integer
---@field public _0 integer
---@field public _1 integer
---@field public _2 integer
---@field public _3 integer
---@field public _4 integer
---@field public _5 integer
---@field public _6 integer
---@field public _7 integer
---@field public _8 integer
---@field public _9 integer
---@field public SEMICOLON integer
---@field public EQUAL integer
---@field public A integer
---@field public B integer
---@field public C integer
---@field public D integer
---@field public E integer
---@field public F integer
---@field public G integer
---@field public H integer
---@field public I integer
---@field public J integer
---@field public K integer
---@field public L integer
---@field public M integer
---@field public N integer
---@field public O integer
---@field public P integer
---@field public Q integer
---@field public R integer
---@field public S integer
---@field public T integer
---@field public U integer
---@field public V integer
---@field public W integer
---@field public X integer
---@field public Y integer
---@field public Z integer
---@field public LEFT_BRACKET integer
---@field public BACKSLASH integer
---@field public RIGHT_BRACKET integer
---@field public GRAVE_ACCENT integer
---@field public WORLD_1 integer
---@field public WORLD_2 integer
---@field public ESCAPE integer
---@field public ENTER integer
---@field public TAB integer
---@field public BACKSPACE integer
---@field public INSERT integer
---@field public DELETE integer
---@field public RIGHT integer
---@field public LEFT integer
---@field public DOWN integer
---@field public UP integer
---@field public PAGE_UP integer
---@field public PAGE_DOWN integer
---@field public HOME integer
---@field public END integer
---@field public CAPS_LOCK integer
---@field public SCROLL_LOCK integer
---@field public NUM_LOCK integer
---@field public PRINT_SCREEN integer
---@field public PAUSE integer
---@field public F1 integer
---@field public F2 integer
---@field public F3 integer
---@field public F4 integer
---@field public F5 integer
---@field public F6 integer
---@field public F7 integer
---@field public F8 integer
---@field public F9 integer
---@field public F10 integer
---@field public F11 integer
---@field public F12 integer
---@field public F13 integer
---@field public F14 integer
---@field public F15 integer
---@field public F16 integer
---@field public F17 integer
---@field public F18 integer
---@field public F19 integer
---@field public F20 integer
---@field public F21 integer
---@field public F22 integer
---@field public F23 integer
---@field public F24 integer
---@field public F25 integer
---@field public KP_0 integer
---@field public KP_1 integer
---@field public KP_2 integer
---@field public KP_3 integer
---@field public KP_4 integer
---@field public KP_5 integer
---@field public KP_6 integer
---@field public KP_7 integer
---@field public KP_8 integer
---@field public KP_9 integer
---@field public KP_DECIMAL integer
---@field public KP_DIVIDE integer
---@field public KP_MULTIPLY integer
---@field public KP_SUBTRACT integer
---@field public KP_ADD integer
---@field public KP_ENTER integer
---@field public KP_EQUAL integer
---@field public LEFT_SHIFT integer
---@field public LEFT_CONTROL integer
---@field public LEFT_ALT integer
---@field public LEFT_SUPER integer
---@field public RIGHT_SHIFT integer
---@field public RIGHT_CONTROL integer
---@field public RIGHT_ALT integer
---@field public RIGHT_SUPER integer
---@field public MENU integer
---@field public LAST integer

---@type glfw_keyboard
glfw.keyboard = libglfw.keyboard

---@class glfw_modifier_key
---@field public SHIFT integer
---@field public CONTROL integer
---@field public ALT integer
---@field public SUPER integer
---@field public CAPS_LOCK integer
---@field public NUM_LOCK integer

---@type glfw_modifier_key
glfw.modifier_key = libglfw.modifier_key

---@class glfw_mouse_button
---@field public _1 integer
---@field public _2 integer
---@field public _3 integer
---@field public _4 integer
---@field public _5 integer
---@field public _6 integer
---@field public _7 integer
---@field public _8 integer
---@field public LAST integer
---@field public LEFT integer
---@field public RIGHT integer
---@field public MIDDLE integer

---@type glfw_mouse_button
glfw.mouse_button = libglfw.mouse_button

---@class glfw_standard_cursor_shape
---@field public ARROW integer
---@field public IBEAM integer
---@field public CROSSHAIR integer
---@field public HAND integer
---@field public HRESIZE integer
---@field public VRESIZE integer

---@type glfw_standard_cursor_shape
glfw.standard_cursor_shape = libglfw.standard_cursor_shape

---@class glfw_input_mode
---@field public CURSOR integer
---@field public STICKY_KEYS integer
---@field public STICKY_MOUSE_BUTTONS integer
---@field public LOCK_KEY_MODS integer
---@field public RAW_MOUSE_MOTION integer

---@type glfw_input_mode
glfw.input_mode = libglfw.input_mode

---@class glfw_cursor_mode
---@field public NORMAL integer
---@field public HIDDEN integer
---@field public DISABLED integer

---@type glfw_cursor_mode
glfw.cursor_mode = libglfw.cursor_mode

---@class glfw_input_state
---@field public PRESS integer
---@field public RELEASE integer
---@field public REPEAT integer

---@type glfw_input_state
glfw.input_state = libglfw.input_state

-- }======================================================

--[[
** {======================================================
** Monitor
** =======================================================
--]]

---@class GLFWmonitor:lightuserdata

---@return nil | GLFWmonitor[]
function glfw.getMonitors()
	return libglfw.GetMonitors()
end
---@return GLFWmonitor
function glfw.getPrimaryMonitor()
	return libglfw.GetPrimaryMonitor()
end
---@param monitor GLFWmonitor
---@return integer, integer @xpos, ypos
function glfw.getMonitorPos(monitor)
	return libglfw.GetMonitorPos(monitor)
end
---@param monitor GLFWmonitor
---@return integer, integer, integer, integer @x, y, w, h
function glfw.getMonitorWorkarea(monitor)
	return libglfw.GetMonitorWorkarea(monitor)
end
---@param monitor GLFWmonitor
---@return integer, integer @w, h
function glfw.getMonitorPhysicalSize(monitor)
	return libglfw.GetMonitorPhysicalSize(monitor)
end
---@param monitor GLFWmonitor
---@return number, number
function glfw.getMonitorContentScale(monitor)
	return libglfw.GetMonitorContentScale(monitor)
end
---@param monitor GLFWmonitor
---@return string
function glfw.getMonitorName(monitor)
	return libglfw.GetMonitorName(monitor)
end
---@param callback fun(monitor:GLFWmonitor, event:glfw_device_event):void
function glfw.setMonitorCallback(callback)
	libglfw.SetMonitorCallback(callback)
end

---@class GLFWvidmode:table
---@field width integer
---@field height integer
---@field redBits integer
---@field greenBits integer
---@field blueBits integer
---@field refreshRate integer

---@param monitor GLFWmonitor
---@return GLFWvidmode[]
function glfw.getVideoModes(monitor)
	return libglfw.GetVideoModes(monitor)
end
---@param monitor GLFWmonitor
---@return GLFWvidmode
function glfw.getVideoMode(monitor)
	return libglfw.GetVideoMode(monitor)
end
---@param monitor GLFWmonitor
---@param gamma number
function glfw.setGamma(monitor, gamma)
	libglfw.SetGamma(monitor, gamma)
end

---@class GLFWgammaramp:lightuserdata

---@param monitor GLFWmonitor
---@return GLFWgammaramp
function glfw.getGammaRamp(monitor)
	return libglfw.GetGammaRamp(monitor)
end
---@param monitor GLFWmonitor
---@param ramp GLFWgammaramp
function glfw.setGammaRamp(monitor, ramp)
	libglfw.SetGammaRamp(monitor, ramp)
end

---@class glfw_device_event
---@field public CONNECTED integer
---@field public DISCONNECTED integer

---@type glfw_device_event
glfw.device_event = libglfw.device_event

-- }======================================================

--[[
** {======================================================
** GLFWimage
** =======================================================
--]]

---@class GLFWimage:userdata
---@field getValue fun(self:GLFWimage):integer, integer, string

---@param width integer
---@param height integer
---@param pixels string
---@return GLFWimage
function glfw.GLFWimage(width, height, pixels)
	return libglfw.GLFWimage(width, height, pixels)
end

-- }======================================================

--[[
** {======================================================
** Window
** =======================================================
--]]

function glfw.defaultWindowHints()
	libglfw.DefaultWindowHints()
end
---@param hint glfw_window_hint
---@param value glfw_hint_value
function glfw.windowHint(hint, value)
	libglfw.WindowHint(hint, value)
end
---@param hint glfw_window_hint
---@param value string
function glfw.windowHintString(hint, value)
	libglfw.WindowHintString(hint, value)
end
---@param width integer
---@param height integer
---@param title string
---@param monitor GLFWmonitor
---@param shared GLFWwindow
---@return GLFWwindow
function glfw.createWindow(width, height, title, monitor, shared)
	return libglfw.CreateWindow(width, height, title, monitor, shared)
end
---@param window GLFWwindow
function glfw.destroyWindow(window)
	libglfw.DestroyWindow(window)
end
---@param window GLFWwindow
---@return boolean
function glfw.windowShouldClose(window)
	return libglfw.WindowShouldClose(window)
end
---@param window GLFWwindow
---@param value boolean
function glfw.setWindowShouldClose(window, value)
	libglfw.SetWindowShouldClose(window, value)
end
---@param window GLFWwindow
---@param title string
function glfw.setWindowTitle(window, title)
	libglfw.SetWindowTitle(window, title)
end
---@param window GLFWwindow
---@param image nil | GLFWimage
function glfw.setWindowIcon(window, image)
	libglfw.SetWindowIcon(window, image)
end
---@param window GLFWwindow
---@return integer, integer @xpos, ypos
function glfw.getWindowPos(window)
	return libglfw.GetWindowPos(window)
end
---@param window GLFWwindow
---@param xpos integer
---@param ypos integer
function glfw.setWindowPos(window, xpos, ypos)
	libglfw.SetWindowPos(window, xpos, ypos)
end
---@param window GLFWwindow
---@return integer, integer
function glfw.getWindowSize(window)
	return libglfw.GetWindowSize(window)
end
---@param window GLFWwindow
---@param minWidth integer
---@param minHeight integer
---@param maxWidth integer
---@param maxHeight integer
function glfw.setWindowSizeLimits(window, minWidth, minHeight, maxWidth, maxHeight)
	libglfw.SetWindowSizeLimits(window, minWidth, minHeight, maxWidth, maxHeight)
end
---@param window GLFWwindow
---@param numer integer
---@param denom integer
function glfw.setWindowAspectRatio(window, numer, denom)
	libglfw.SetWindowAspectRatio(window, numer, denom)
end
---@param window GLFWwindow
---@param width integer
---@param height integer
function glfw.setWindowSize(window, width, height)
	libglfw.SetWindowSize(window, width, height)
end
---@param window GLFWwindow
---@return integer, integer @w, h
function glfw.getFramebufferSize(window)
	return libglfw.GetFramebufferSize(window)
end
---@param window GLFWwindow
---@return integer, integer, integer, integer @left, top, right, bottom
function glfw.getWindowFrameSize(window)
	return libglfw.GetWindowFrameSize(window)
end
---@param window GLFWwindow
---@return number, number
function glfw.getWindowContentScale(window)
	return libglfw.GetWindowContentScale(window)
end
---@param window GLFWwindow
---@return number
function glfw.getWindowOpacity(window)
	return libglfw.GetWindowOpacity(window)
end
---@param window GLFWwindow
---@param opacity number
function glfw.setWindowOpacity(window, opacity)
	libglfw.SetWindowOpacity(window, opacity)
end
---@param window GLFWwindow
function glfw.iconifyWindow(window)
	libglfw.IconifyWindow(window)
end
---@param window GLFWwindow
function glfw.restoreWindow(window)
	libglfw.RestoreWindow(window)
end
---@param window GLFWwindow
function glfw.maximizeWindow(window)
	libglfw.MaximizeWindow(window)
end
---@param window GLFWwindow
function glfw.showWindow(window)
	libglfw.ShowWindow(window)
end
---@param window GLFWwindow
function glfw.hideWindow(window)
	libglfw.HideWindow(window)
end
---@param window GLFWwindow
function glfw.focusWindow(window)
	libglfw.FocusWindow(window)
end
---@param window GLFWwindow
function glfw.requestWindowAttention(window)
	libglfw.RequestWindowAttention(window)
end
---@param window GLFWwindow
---@return GLFWmonitor
function glfw.getWindowMonitor(window)
	return libglfw.GetWindowMonitor(window)
end
---@param window GLFWwindow
---@param monitor GLFWmonitor
---@param xpos integer
---@param ypos integer
---@param width integer
---@param height integer
---@param refreshRate integer
function glfw.setWindowMonitor(window, monitor, xpos, ypos, width, height, refreshRate)
	libglfw.SetWindowMonitor(window, monitor, xpos, ypos, width, height, refreshRate)
end
---@param window GLFWwindow
---@param attrib glfw_window_attribute
---@return integer
function glfw.getWindowAttrib(window, attrib)
	return libglfw.GetWindowAttrib(window, attrib)
end
---@param window GLFWwindow
---@param attrib glfw_window_attribute
---@param value integer
function glfw.setWindowAttrib(window, attrib, value)
	libglfw.SetWindowAttrib(window, attrib, value)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, xpos:integer, ypos:integer):void
function glfw.setWindowPosCallback(window, callback)
	libglfw.SetWindowPosCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, width:integer, height:integer):void
function glfw.setWindowSizeCallback(window, callback)
	libglfw.SetWindowSizeCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow):void
function glfw.setWindowCloseCallback(window, callback)
	libglfw.SetWindowCloseCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow):void
function glfw.setWindowRefreshCallback(window, callback)
	libglfw.SetWindowRefreshCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, focused:boolean):void
function glfw.setWindowFocusCallback(window, callback)
	libglfw.SetWindowFocusCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, iconified:boolean):void
function glfw.setWindowIconifyCallback(window, callback)
	libglfw.SetWindowIconifyCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, maximized:boolean):void
function glfw.setWindowMaximizeCallback(window, callback)
	libglfw.SetWindowMaximizeCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, width:integer, height:integer):void
function glfw.setFramebufferSizeCallback(window, callback)
	libglfw.SetFramebufferSizeCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, xscale:number, yscale:number):void
function glfw.setWindowContentScaleCallback(window, callback)
	libglfw.SetWindowContentScaleCallback(window, callback)
end
function glfw.pollEvents()
	libglfw.PollEvents()
end
function glfw.waitEvents()
	libglfw.WaitEvents()
end
---@param timeout number
function glfw.waitEventsTimeout(timeout)
	libglfw.WaitEventsTimeout(timeout)
end
function glfw.postEmptyEvent()
	libglfw.PostEmptyEvent()
end
---@param window GLFWwindow
function glfw.swapBuffers(window)
	libglfw.SwapBuffers(window)
end

---@class glfw_window_hint
---@field public RESIZABLE integer
---@field public VISIBLE integer
---@field public DECORATED integer
---@field public FOCUSED integer
---@field public AUTO_ICONIFY integer
---@field public FLOATING integer
---@field public MAXIMIZED integer
---@field public CENTER_CURSOR integer
---@field public TRANSPARENT_FRAMEBUFFER integer
---@field public FOCUS_ON_SHOW integer
---@field public SCALE_TO_MONITOR integer
---@field public RED_BITS integer
---@field public GREEN_BITS integer
---@field public BLUE_BITS integer
---@field public ALPHA_BITS integer
---@field public DEPTH_BITS integer
---@field public STENCIL_BITS integer
---@field public ACCUM_RED_BITS integer
---@field public ACCUM_GREEN_BITS integer
---@field public ACCUM_BLUE_BITS integer
---@field public ACCUM_ALPHA_BITS integer
---@field public AUX_BUFFERS integer
---@field public SAMPLES integer
---@field public REFRESH_RATE integer
---@field public STEREO integer
---@field public SRGB_CAPABLE integer
---@field public DOUBLEBUFFER integer
---@field public CLIENT_API integer
---@field public CONTEXT_CREATION_API integer
---@field public CONTEXT_VERSION_MAJOR integer
---@field public CONTEXT_VERSION_MINOR integer
---@field public CONTEXT_ROBUSTNESS integer
---@field public CONTEXT_RELEASE_BEHAVIOR integer
---@field public OPENGL_FORWARD_COMPAT integer
---@field public OPENGL_DEBUG_CONTEXT integer
---@field public OPENGL_PROFILE integer
---@field public COCOA_RETINA_FRAMEBUFFER integer
---@field public COCOA_FRAME_NAME integer
---@field public COCOA_GRAPHICS_SWITCHING integer
---@field public X11_CLASS_NAME integer
---@field public X11_INSTANCE_NAME integer

---@type glfw_window_hint
glfw.window_hint = libglfw.window_hint

---@class glfw_window_attribute
---@field public FOCUSED integer
---@field public ICONIFIED integer
---@field public MAXIMIZED integer
---@field public HOVERED integer
---@field public VISIBLE integer
---@field public RESIZABLE integer
---@field public DECORATED integer
---@field public AUTO_ICONIFY integer
---@field public FLOATING integer
---@field public TRANSPARENT_FRAMEBUFFER integer
---@field public FOCUS_ON_SHOW integer
---@field public CLIENT_API integer
---@field public CONTEXT_CREATION_API integer
---@field public CONTEXT_VERSION_MAJOR integer
---@field public CONTEXT_VERSION_MINOR integer
---@field public CONTEXT_REVISION integer
---@field public OPENGL_FORWARD_COMPAT integer
---@field public OPENGL_DEBUG_CONTEXT integer
---@field public OPENGL_PROFILE integer
---@field public CONTEXT_RELEASE_BEHAVIOR integer
---@field public CONTEXT_NO_ERROR integer
---@field public CONTEXT_ROBUSTNESS integer

---@type glfw_window_attribute
glfw.window_attribute = libglfw.window_attribute

---@class glfw_hint_value
---@field public TRUE integer
---@field public FALSE integer
---@field public DONT_CARE integer
---@field public OPENGL_API integer
---@field public OPENGL_ES_API integer
---@field public NO_API integer
---@field public NATIVE_CONTEXT_API integer
---@field public EGL_CONTEXT_API integer
---@field public OSMESA_CONTEXT_API integer
---@field public NO_ROBUSTNESS integer
---@field public NO_RESET_NOTIFICATION integer
---@field public LOSE_CONTEXT_ON_RESET integer
---@field public ANY_RELEASE_BEHAVIOR integer
---@field public RELEASE_BEHAVIOR_FLUSH integer
---@field public RELEASE_BEHAVIOR_NONE integer
---@field public OPENGL_ANY_PROFILE integer
---@field public OPENGL_COMPAT_PROFILE integer
---@field public OPENGL_CORE_PROFILE integer

---@type glfw_hint_value
glfw.hint_value = libglfw.hint_value

-- }======================================================

--[[
** {======================================================
** Function Pointers
** =======================================================
--]]

---@class glfw_func_ptr:table
---@field public MakeContextCurrent lightuserdata
---@field public SwapBuffers lightuserdata
---@field public SwapInterval lightuserdata
---@field public GetProcAddress lightuserdata
---@field public GetFramebufferSize lightuserdata
---@field public GetTime lightuserdata

---@type glfw_func_ptr
glfw.func_ptr = libglfw.func_ptr

-- }======================================================

return glfw
