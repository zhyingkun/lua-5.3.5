local libglfw = require("libglfwrap")

---@class glfw
local glfw = {}

--[[
** {======================================================
** Context
** =======================================================
--]]

---@class GLFWwindow

---@param window GLFWwindow
function glfw.makeContextCurrent(window)
	libglfw.MakeContextCurrent(window)
end
---@return GLFWwindow
function glfw.getCurrentContext()
	return libglfw.GetCurrentContext()
end
---@param interval number
function glfw.swapInterval(interval)
	libglfw.SwapInterval(interval)
end
---@param extension string
---@return boolean
function glfw.extensionSupported(extension)
	return libglfw.ExtensionSupported(extension)
end
---@param window GLFWwindow
---@return userdata
function glfw.getProcAddress(procname)
	return libglfw.GetProcAddress(procname)
end

-- }======================================================

--[[
** {======================================================
** Error Handler
** =======================================================
--]]

---@param callback fun(msg:string):string
function glfw.setErrorMessageHandler(callback)
	libglfw.SetErrorMessageHandler(callback)
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
---@return number, number, number
function glfw.getVersion()
	return libglfw.GetVersion()
end
---@return string
function glfw.getVersionString()
	return libglfw.GetVersionString()
end
---@return number, string
function glfw.getError()
	return libglfw.GetError()
end
function glfw.setErrorCallback()
	libglfw.SetErrorCallback()
end

---@class glfw_err_code
---@field public NO_ERROR number
---@field public NOT_INITIALIZED number
---@field public NO_CURRENT_CONTEXT number
---@field public INVALID_ENUM number
---@field public INVALID_VALUE number
---@field public OUT_OF_MEMORY number
---@field public API_UNAVAILABLE number
---@field public VERSION_UNAVAILABLE number
---@field public PLATFORM_ERROR number
---@field public FORMAT_UNAVAILABLE number
---@field public NO_WINDOW_CONTEXT number

---@type glfw_err_code
glfw.err_code = libglfw.err_code

---@class glfw_init_hint
---@field public JOYSTICK_HAT_BUTTONS number
---@field public COCOA_CHDIR_RESOURCES number
---@field public COCOA_MENUBAR number

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
---@return number
function glfw.getInputMode(window, mode)
	return libglfw.GetInputMode(window, mode)
end
---@param window GLFWwindow
---@param mode glfw_input_mode
---@param value number
function glfw.setInputMode(window, mode, value)
	libglfw.SetInputMode(window, mode, value)
end
---@return boolean
function glfw.rawMouseMotionSupported()
	return libglfw.RawMouseMotionSupported()
end
---@param key glfw_keyboard
---@param scancode number
---@return string
function glfw.getKeyName(key, scancode)
	return libglfw.GetKeyName(key, scancode)
end
---@param key glfw_keyboard
---@return number
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

---@class GLFWimage

---@param image GLFWimage
---@param xhot number
---@param yhot number
---@return boolean
function glfw.createCursor(image, xhot, yhot)
	return libglfw.CreateCursor(image, xhot, yhot)
end

---@class GLFWcursor

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
---@param callback fun(window:GLFWwindow, key:glfw_keyboard, scancode:number, action:glfw_input_state, mods:glfw_modifier_key):void
function glfw.setKeyCallback(window, callback)
	libglfw.SetKeyCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, codepoint:number):void
function glfw.setCharCallback(window, callback)
	libglfw.SetCharCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, codepoint:number, mods:glfw_modifier_key):void
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
---@param callback fun(window:GLFWwindow, entered:boolean, yscale:number):void
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

---@class GLFWgamepadstate
---@field buttons glfw_input_state[]
---@field axes number[]

---@param jid glfw_joystick
---@return nil | GLFWgamepadstate
function glfw.getGamepadState(jid)
	return libglfw.GetGamepadState(jid)
end
---@param window GLFWwindow
---@param str string
function glfw.setClipboardString(window, str)
	libglfw.SetClipboardString(window, str)
end
---@param window GLFWwindow
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
---@return number
function glfw.getTimerValue()
	return libglfw.GetTimerValue()
end
---@return number
function glfw.getTimerFrequency()
	return libglfw.GetTimerFrequency()
end

---@class glfw_gamepad_axis
---@field public LEFT_X number
---@field public LEFT_Y number
---@field public RIGHT_X number
---@field public RIGHT_Y number
---@field public LEFT_TRIGGER number
---@field public RIGHT_TRIGGER number
---@field public LAST number

---@type glfw_gamepad_axis
glfw.gamepad_axis = libglfw.gamepad_axis

---@class glfw_gamepad_button
---@field public A number
---@field public B number
---@field public X number
---@field public Y number
---@field public LEFT_BUMPER number
---@field public RIGHT_BUMPER number
---@field public BACK number
---@field public START number
---@field public GUIDE number
---@field public LEFT_THUMB number
---@field public RIGHT_THUMB number
---@field public DPAD_UP number
---@field public DPAD_RIGHT number
---@field public DPAD_DOWN number
---@field public DPAD_LEFT number
---@field public LAST number
---@field public CROSS number
---@field public CIRCLE number
---@field public SQUARE number
---@field public TRIANGLE number

---@type glfw_gamepad_button
glfw.gamepad_button = libglfw.gamepad_button

---@class glfw_joystick_hat
---@field public CENTERED number
---@field public UP number
---@field public RIGHT number
---@field public DOWN number
---@field public LEFT number
---@field public RIGHT_UP number
---@field public RIGHT_DOWN number
---@field public LEFT_UP number
---@field public LEFT_DOWN number

---@type glfw_joystick_hat
glfw.joystick_hat = libglfw.joystick_hat

---@class glfw_joystick
---@field public _1 number
---@field public _2 number
---@field public _3 number
---@field public _4 number
---@field public _5 number
---@field public _6 number
---@field public _7 number
---@field public _8 number
---@field public _9 number
---@field public _10 number
---@field public _11 number
---@field public _12 number
---@field public _13 number
---@field public _14 number
---@field public _15 number
---@field public _16 number
---@field public LAST number

---@type glfw_joystick
glfw.joystick = libglfw.joystick

---@class glfw_keyboard
---@field public UNKNOWN number
---@field public SPACE number
---@field public APOSTROPHE number
---@field public COMMA number
---@field public MINUS number
---@field public PERIOD number
---@field public SLASH number
---@field public _0 number
---@field public _1 number
---@field public _2 number
---@field public _3 number
---@field public _4 number
---@field public _5 number
---@field public _6 number
---@field public _7 number
---@field public _8 number
---@field public _9 number
---@field public SEMICOLON number
---@field public EQUAL number
---@field public A number
---@field public B number
---@field public C number
---@field public D number
---@field public E number
---@field public F number
---@field public G number
---@field public H number
---@field public I number
---@field public J number
---@field public K number
---@field public L number
---@field public M number
---@field public N number
---@field public O number
---@field public P number
---@field public Q number
---@field public R number
---@field public S number
---@field public T number
---@field public U number
---@field public V number
---@field public W number
---@field public X number
---@field public Y number
---@field public Z number
---@field public LEFT_BRACKET number
---@field public BACKSLASH number
---@field public RIGHT_BRACKET number
---@field public GRAVE_ACCENT number
---@field public WORLD_1 number
---@field public WORLD_2 number
---@field public ESCAPE number
---@field public ENTER number
---@field public TAB number
---@field public BACKSPACE number
---@field public INSERT number
---@field public DELETE number
---@field public RIGHT number
---@field public LEFT number
---@field public DOWN number
---@field public UP number
---@field public PAGE_UP number
---@field public PAGE_DOWN number
---@field public HOME number
---@field public END number
---@field public CAPS_LOCK number
---@field public SCROLL_LOCK number
---@field public NUM_LOCK number
---@field public PRINT_SCREEN number
---@field public PAUSE number
---@field public F1 number
---@field public F2 number
---@field public F3 number
---@field public F4 number
---@field public F5 number
---@field public F6 number
---@field public F7 number
---@field public F8 number
---@field public F9 number
---@field public F10 number
---@field public F11 number
---@field public F12 number
---@field public F13 number
---@field public F14 number
---@field public F15 number
---@field public F16 number
---@field public F17 number
---@field public F18 number
---@field public F19 number
---@field public F20 number
---@field public F21 number
---@field public F22 number
---@field public F23 number
---@field public F24 number
---@field public F25 number
---@field public KP_0 number
---@field public KP_1 number
---@field public KP_2 number
---@field public KP_3 number
---@field public KP_4 number
---@field public KP_5 number
---@field public KP_6 number
---@field public KP_7 number
---@field public KP_8 number
---@field public KP_9 number
---@field public KP_DECIMAL number
---@field public KP_DIVIDE number
---@field public KP_MULTIPLY number
---@field public KP_SUBTRACT number
---@field public KP_ADD number
---@field public KP_ENTER number
---@field public KP_EQUAL number
---@field public LEFT_SHIFT number
---@field public LEFT_CONTROL number
---@field public LEFT_ALT number
---@field public LEFT_SUPER number
---@field public RIGHT_SHIFT number
---@field public RIGHT_CONTROL number
---@field public RIGHT_ALT number
---@field public RIGHT_SUPER number
---@field public MENU number
---@field public LAST number

---@type glfw_keyboard
glfw.keyboard = libglfw.keyboard

---@class glfw_modifier_key
---@field public SHIFT number
---@field public CONTROL number
---@field public ALT number
---@field public SUPER number
---@field public CAPS_LOCK number
---@field public NUM_LOCK number

---@type glfw_modifier_key
glfw.modifier_key = libglfw.modifier_key

---@class glfw_mouse_button
---@field public _1 number
---@field public _2 number
---@field public _3 number
---@field public _4 number
---@field public _5 number
---@field public _6 number
---@field public _7 number
---@field public _8 number
---@field public LAST number
---@field public LEFT number
---@field public RIGHT number
---@field public MIDDLE number

---@type glfw_mouse_button
glfw.mouse_button = libglfw.mouse_button

---@class glfw_standard_cursor_shape
---@field public ARROW number
---@field public IBEAM number
---@field public CROSSHAIR number
---@field public HAND number
---@field public HRESIZE number
---@field public VRESIZE number

---@type glfw_standard_cursor_shape
glfw.standard_cursor_shape = libglfw.standard_cursor_shape

---@class glfw_input_mode
---@field public CURSOR number
---@field public STICKY_KEYS number
---@field public STICKY_MOUSE_BUTTONS number
---@field public LOCK_KEY_MODS number
---@field public RAW_MOUSE_MOTION number

---@type glfw_input_mode
glfw.input_mode = libglfw.input_mode

---@class glfw_cursor_mode
---@field public NORMAL number
---@field public HIDDEN number
---@field public DISABLED number

---@type glfw_cursor_mode
glfw.cursor_mode = libglfw.cursor_mode

---@class glfw_input_state
---@field public PRESS number
---@field public RELEASE number
---@field public REPEAT number

---@type glfw_input_state
glfw.input_state = libglfw.input_state

-- }======================================================

--[[
** {======================================================
** Monitor
** =======================================================
--]]

---@class GLFWmonitor

---@return nil | GLFWmonitor[]
function glfw.getMonitors()
	return libglfw.GetMonitors()
end
---@return GLFWmonitor
function glfw.getPrimaryMonitor()
	return libglfw.GetPrimaryMonitor()
end
---@param monitor GLFWmonitor
---@return number, number
function glfw.getMonitorPos(monitor)
	return libglfw.GetMonitorPos(monitor)
end
---@param monitor GLFWmonitor
---@return number, number, number, number
function glfw.getMonitorWorkarea(monitor)
	return libglfw.GetMonitorWorkarea(monitor)
end
---@param monitor GLFWmonitor
---@return number, number
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

---@class GLFWvidmode
---@field width fun():number
---@field height fun():number
---@field redBits fun():number
---@field greenBits fun():number
---@field blueBits fun():number
---@field refreshRate fun():number

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

---@class GLFWgammaramp

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
---@field public CONNECTED number
---@field public DISCONNECTED number

---@type glfw_device_event
glfw.device_event = libglfw.device_event

-- }======================================================

--[[
** {======================================================
** GLFWimage
** =======================================================
--]]

---@class GLFWimage
---@field getValue fun(self:GLFWimage):number, number, string

---@param width number
---@param height number
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
---@param width number
---@param height number
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
---@return number, number
function glfw.getWindowPos(window)
	return libglfw.GetWindowPos(window)
end
---@param window GLFWwindow
---@param xpos number
---@param ypos number
function glfw.setWindowPos(window, xpos, ypos)
	libglfw.SetWindowPos(window, xpos, ypos)
end
---@param window GLFWwindow
---@return number, number
function glfw.getWindowSize(window)
	return libglfw.GetWindowSize(window)
end
---@param window GLFWwindow
---@param minWidth number
---@param minHeight number
---@param maxWidth number
---@param maxHeight number
function glfw.setWindowSizeLimits(window, minWidth, minHeight, maxWidth, maxHeight)
	libglfw.SetWindowSizeLimits(window, minWidth, minHeight, maxWidth, maxHeight)
end
---@param window GLFWwindow
---@param numer number
---@param denom number
function glfw.setWindowAspectRatio(window, numer, denom)
	libglfw.SetWindowAspectRatio(window, numer, denom)
end
---@param window GLFWwindow
---@param width number
---@param height number
function glfw.setWindowSize(window, width, height)
	libglfw.SetWindowSize(window, width, height)
end
---@param window GLFWwindow
---@return number, number
function glfw.getFramebufferSize(window)
	return libglfw.GetFramebufferSize(window)
end
---@param window GLFWwindow
---@return number, number, number, number
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
---@param xpos number
---@param ypos number
---@param width number
---@param height number
---@param refreshRate number
function glfw.setWindowMonitor(window, monitor, xpos, ypos, width, height, refreshRate)
	libglfw.SetWindowMonitor(window, monitor, xpos, ypos, width, height, refreshRate)
end
---@param window GLFWwindow
---@param attrib glfw_window_attribute
---@return number
function glfw.getWindowAttrib(window, attrib)
	return libglfw.GetWindowAttrib(window, attrib)
end
---@param window GLFWwindow
---@param attrib glfw_window_attribute
---@param value number
function glfw.setWindowAttrib(window, attrib, value)
	libglfw.SetWindowAttrib(window, attrib, value)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, xpos:number, ypos:number):void
function glfw.setWindowPosCallback(window, callback)
	libglfw.SetWindowPosCallback(window, callback)
end
---@param window GLFWwindow
---@param callback fun(window:GLFWwindow, width:number, height:number):void
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
---@param callback fun(window:GLFWwindow, width:number, height:number):void
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
---@field public RESIZABLE number
---@field public VISIBLE number
---@field public DECORATED number
---@field public FOCUSED number
---@field public AUTO_ICONIFY number
---@field public FLOATING number
---@field public MAXIMIZED number
---@field public CENTER_CURSOR number
---@field public TRANSPARENT_FRAMEBUFFER number
---@field public FOCUS_ON_SHOW number
---@field public SCALE_TO_MONITOR number
---@field public RED_BITS number
---@field public GREEN_BITS number
---@field public BLUE_BITS number
---@field public ALPHA_BITS number
---@field public DEPTH_BITS number
---@field public STENCIL_BITS number
---@field public ACCUM_RED_BITS number
---@field public ACCUM_GREEN_BITS number
---@field public ACCUM_BLUE_BITS number
---@field public ACCUM_ALPHA_BITS number
---@field public AUX_BUFFERS number
---@field public SAMPLES number
---@field public REFRESH_RATE number
---@field public STEREO number
---@field public SRGB_CAPABLE number
---@field public DOUBLEBUFFER number
---@field public CLIENT_API number
---@field public CONTEXT_CREATION_API number
---@field public CONTEXT_VERSION_MAJOR number
---@field public CONTEXT_VERSION_MINOR number
---@field public CONTEXT_ROBUSTNESS number
---@field public CONTEXT_RELEASE_BEHAVIOR number
---@field public OPENGL_FORWARD_COMPAT number
---@field public OPENGL_DEBUG_CONTEXT number
---@field public OPENGL_PROFILE number
---@field public COCOA_RETINA_FRAMEBUFFER number
---@field public COCOA_FRAME_NAME number
---@field public COCOA_GRAPHICS_SWITCHING number
---@field public X11_CLASS_NAME number
---@field public X11_INSTANCE_NAME number

---@type glfw_window_hint
glfw.window_hint = libglfw.window_hint

---@class glfw_window_attribute
---@field public FOCUSED number
---@field public ICONIFIED number
---@field public MAXIMIZED number
---@field public HOVERED number
---@field public VISIBLE number
---@field public RESIZABLE number
---@field public DECORATED number
---@field public AUTO_ICONIFY number
---@field public FLOATING number
---@field public TRANSPARENT_FRAMEBUFFER number
---@field public FOCUS_ON_SHOW number
---@field public CLIENT_API number
---@field public CONTEXT_CREATION_API number
---@field public CONTEXT_VERSION_MAJOR number
---@field public CONTEXT_VERSION_MINOR number
---@field public CONTEXT_REVISION number
---@field public OPENGL_FORWARD_COMPAT number
---@field public OPENGL_DEBUG_CONTEXT number
---@field public OPENGL_PROFILE number
---@field public CONTEXT_RELEASE_BEHAVIOR number
---@field public CONTEXT_NO_ERROR number
---@field public CONTEXT_ROBUSTNESS number

---@type glfw_window_attribute
glfw.window_attribute = libglfw.window_attribute

---@class glfw_hint_value
---@field public TRUE number
---@field public FALSE number
---@field public DONT_CARE number
---@field public OPENGL_API number
---@field public OPENGL_ES_API number
---@field public NO_API number
---@field public NATIVE_CONTEXT_API number
---@field public EGL_CONTEXT_API number
---@field public OSMESA_CONTEXT_API number
---@field public NO_ROBUSTNESS number
---@field public NO_RESET_NOTIFICATION number
---@field public LOSE_CONTEXT_ON_RESET number
---@field public ANY_RELEASE_BEHAVIOR number
---@field public RELEASE_BEHAVIOR_FLUSH number
---@field public RELEASE_BEHAVIOR_NONE number
---@field public OPENGL_ANY_PROFILE number
---@field public OPENGL_COMPAT_PROFILE number
---@field public OPENGL_CORE_PROFILE number

---@type glfw_hint_value
glfw.hint_value = libglfw.hint_value

-- }======================================================

--[[
** {======================================================
** Function Pointers
** =======================================================
--]]

---@class glfw_func_ptr
---@field public MakeContextCurrent userdata
---@field public SwapBuffers userdata
---@field public SwapInterval userdata
---@field public GetProcAddress userdata
---@field public GetFramebufferSize userdata
---@field public GetTime userdata

---@type glfw_func_ptr
glfw.func_ptr = libglfw.func_ptr

-- }======================================================

return glfw
