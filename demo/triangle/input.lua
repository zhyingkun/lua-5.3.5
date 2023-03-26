local glfw = require("glfw")

-- TODO: Joystick Monitor
local input_type = {
	Key = 0,
	Char = 1,
	MouseButton = 2,
	CursorPos = 3,
	CursorEnter = 4,
	Scroll = 5,
	Drop = 6,

	WindowPos = 7,
	WindowSize = 8,
	WindowClose = 9,
	WindowRefresh = 10,
	WindowFocus = 11,
	WindowIconify = 12,
	WindowMaximize = 13,
	WindowFramebufferSize = 14,
	WindowContentScale = 15,
}

local InputSetCallbackMap = {
	Key = glfw.setKeyCallback,
	Char = glfw.setCharCallback,
	MouseButton = glfw.setMouseButtonCallback,
	CursorPos = glfw.setCursorPosCallback,
	CursorEnter = glfw.setCursorEnterCallback,
	Scroll = glfw.setScrollCallback,
	Drop = glfw.setDropCallback,

	WindowPos = glfw.setWindowPosCallback,
	WindowSize = glfw.setWindowSizeCallback,
	WindowClose = glfw.setWindowCloseCallback,
	WindowRefresh = glfw.setWindowRefreshCallback,
	WindowFocus = glfw.setWindowFocusCallback,
	WindowIconify = glfw.setWindowIconifyCallback,
	WindowMaximize = glfw.setWindowMaximizeCallback,
	WindowFramebufferSize = glfw.setFramebufferSizeCallback,
	WindowContentScale = glfw.setWindowContentScaleCallback,
}

-- ActionName => List of input_type
local ActionInputTypeMap = {
	ImGUIKey = { input_type.Key },
	ImGUIChar = { input_type.Char },
	ImGUIMouseButton = { input_type.MouseButton },
	ImGUICursorPos = { input_type.CursorPos },
	ImGUIScroll = { input_type.Scroll },
	ImGUIWindowSize = { input_type.WindowSize },
	ImGUIWindowFramebufferSize = { input_type.WindowFramebufferSize },
}
-- ActionName => List of Callback Function
local ActionCallbackMap = {}
local function MakeCombineCallback(actionList)
	return function(window, ...)
		for _, actionName in ipairs(actionList) do
			for _, callback in ipairs(ActionCallbackMap[actionName] or {}) do
				callback(...)
			end
		end
	end
end

local function FindAllActionName(inputType)
	local ActionNameList = {}
	for ActionName, InputTypeList in pairs(ActionInputTypeMap) do
		if table.indexOf(InputTypeList, inputType) then
			table.insert(ActionNameList, ActionName)
		end
	end
	return ActionNameList
end

local input = {}

function input:init(win)
	for typeName, inputType in pairs(input_type) do
		local actionList = FindAllActionName(inputType)
		if next(actionList) then
			InputSetCallbackMap[typeName](win, MakeCombineCallback(actionList))
		end
	end
end

function input:bindAction(actionName, callback)
	local list = ActionCallbackMap[actionName]
	if list then
		list:insert(callback)
	else
		ActionCallbackMap[actionName] = setmetatable({ callback }, table)
	end
end

return input
