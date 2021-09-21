local MAX_OBJECT_COUNT = 8

local M = {}

local CurrentCount
local a, b

function M.init()
	CurrentCount = 0
	a = 43
	b = 56
end

function M.increase_a()
	a = a + 1
	return a
end
function M.increase_b()
	b = b + 1
	return b
end

function M.get_count_func()
	return function()
		return CurrentCount, MAX_OBJECT_COUNT
	end
end

local meta = {}
meta.__index = meta
function meta:show()
	print("OLD")
end
function M.new(...)
	CurrentCount = CurrentCount + 1
	return setmetatable({}, meta)
end

M.enum = {
	FILE = 0,
	DIR = 1,
}

return M
