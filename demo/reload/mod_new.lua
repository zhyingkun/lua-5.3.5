local MAX_OBJECT_COUNT = 16 -- update config

local M = {}

local CurrentCount
local a, b
local c = "const" -- added

function M.init()
	CurrentCount = 0
	a = 43
	b = 56
end

function M.increase_a()
	a = a + 1
	return a, b -- one more b
end
function M.increase_b()
	b = b + 1
	return b
end

function M.get_count_func()
	print("c is:", c) -- added
	return function()
		return CurrentCount, MAX_OBJECT_COUNT
	end
end

local meta = {}
meta.__index = meta
function meta:show()
	print("NEW") -- updata string
	self:new_logic()
end
local tablesrc = require "common.tablesrc" -- added
print("tablesrc", tablesrc, require)
local insert = table.insert -- added
function meta.new_logic() -- added
	local tbl = {hello = "nice"}
	insert(tbl, "good")
	insert(tbl, MAX_OBJECT_COUNT)
	print(tablesrc.TableToLuaSource(tbl))
	print("new_logic", tostring(tbl, 1))
end
function M.new(...)
	CurrentCount = CurrentCount + 1
	return setmetatable({}, meta)
end

M.enum = {
	FILE = 1, -- update enum value
	DIR = 2, -- update enum value
	LINK = 3, -- added
}

return M
