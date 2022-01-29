local glfw = require("glfw")
local nk = require("nuklear")

local input = {}

local bImGUI
local unicodeCount
local mainWin_
local ButtonInputs

function input.ImGUIInit(mainWin)
	bImGUI = false
	unicodeCount = 0
	mainWin_ = mainWin
	ButtonInputs = {}

	glfw.setCursorPosCallback(mainWin, function(window, xpos, ypos)
		if bImGUI then
			-- print("motion:", xpos, ypos)
			nk.inputMotion(math.floor(xpos), math.floor(ypos))
		end
	 end)
	 glfw.setKeyCallback(mainWin, function(window, key, scancode, action, mods)
		if bImGUI then
			nk.inputKey(KeyToNKKey(key), ActionToDown(action))
		end
	 end)
	 glfw.setMouseButtonCallback(mainWin, function(window, button, action, mods)
		if bImGUI then
			-- print("MouseButton:", button, action, mods)
			ButtonInputs[button] = action
		end
	 end)
	 glfw.setScrollCallback(mainWin, function(window, xoffset, yoffset)
		if bImGUI then
			nk.inputScroll(nk.packVec2(xoffset, yoffset))
		end
	 end)
	 glfw.setCharCallback(mainWin, function(window, codepoint)
		if bImGUI then
			unicodeCount = unicodeCount + 1
			if unicodeCount >= nk.INPUT_MAX then
				printerr("Error: too many input unicode in one frame, maybe low fps")
			else
				if codepoint < 128 then
					nk.inputChar(codepoint)
				else
					nk.inputUnicode(codepoint)
				end	
			end
		end
	 end)	 
end

function input.ImGUIPrePollEvent()
	nk.inputBegin()
	bImGUI = true
	unicodeCount = 0
end

local left = 0
local right = 1
local map = {
	[left] = 0,
	[right] = 2,
}
local function ToNKButton(btn)
	return map[btn]
end
function input.ImGUIPostPollEvent()
	bImGUI = false
	unicodeCount = 0
	local x, y = glfw.getCursorPos(mainWin_)
	if ButtonInputs[left] and ButtonInputs[right] then
		nk.inputButton(ToNKButton(leftAndRight), math.floor(x), math.floor(y), ButtonInputs[left] == 1)
	elseif ButtonInputs[left] then
		nk.inputButton(ToNKButton(left), math.floor(x), math.floor(y), ButtonInputs[left] == 1)
	elseif ButtonInputs[right] then
		nk.inputButton(ToNKButton(right), math.floor(x), math.floor(y), ButtonInputs[right] == 1)
	end
	-- if ButtonInputs[middle] then
	-- 	nk.inputButton(ToNKButton(middle), x, y, ButtonInputs[middle]))
	-- end
	ButtonInputs = {}
	nk.inputEnd()
end

return input
