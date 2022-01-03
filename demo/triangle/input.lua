local glfw = require("glfw")
local nk = require("nuklear")

local input = {}

local bImGUI
local mainWin_
local ButtonInputs

function input.ImGUIInit(mainWin)
	bImGUI = false
	mainWin_ = mainWin
	ButtonInputs = {}

	glfw.SetCursorPosCallback(mainWin, function(window, xpos, ypos)
		if bImGUI then
			-- print("motion:", xpos, ypos)
		   nk.inputMotion(math.floor(xpos), math.floor(ypos))
		end
	 end)
	 glfw.SetKeyCallback(mainWin, function(window, key, scancode, action, mods)
		if bImGUI then
		   nk.inputKey(KeyToNKKey(key), ActionToDown(action))
		end
	 end)
	 glfw.SetMouseButtonCallback(mainWin, function(window, button, action, mods)
		if bImGUI then
			print("MouseButton:", button, action, mods)
		   ButtonInputs[button] = action
		end
	 end)
	 glfw.SetScrollCallback(mainWin, function(window, xoffset, yoffset)
		if bImGUI then
		   nk.inputScroll(nk.packVec2(xoffset, yoffset))
		end
	 end)
	 glfw.SetCharCallback(mainWin, function(window, codepoint)
		if bImGUI then
		   if codepoint < 128 then
			  nk.inputChar(codepoint)
		   else
			  nk.inputUnicode(codepoint)
		   end
		end
	 end)	 
end

function input.ImGUIPrePollEvent()
	nk.inputBegin()
	bImGUI = true
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
   local x, y = glfw.GetCursorPos(mainWin_)
   if ButtonInputs[left] and ButtonInputs[right] then
      nk.inputButton(ToNKButton(leftAndRight), math.floor(x), math.floor(y), ButtonInputs[left] == 1)
   elseif ButtonInputs[left] then
      nk.inputButton(ToNKButton(left), math.floor(x), math.floor(y), ButtonInputs[left] == 1)
   elseif ButtonInputs[right] then
      nk.inputButton(ToNKButton(right), math.floor(x), math.floor(y), ButtonInputs[right] == 1)
   end
--    if ButtonInputs[middle] then
--       nk.inputButton(ToNKButton(middle), x, y, ButtonInputs[middle]))
--    end
   ButtonInputs = {}
   nk.inputEnd()
end

return input
