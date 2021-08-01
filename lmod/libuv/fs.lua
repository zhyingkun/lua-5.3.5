local uvwrap = require("libuvwrap")
local loop = uvwrap.loop.default()

local fs = uvwrap.fs

local M = setmetatable({}, {
	__index = function(self, name)
		local value = fs[name]
		if not value then return end
		local t = type(value)
		if t == "function" then
			local function func(...)
				return value(loop, ...)
			end
			self[name] = func
			return func
		else
			self[name] = value
			return value
		end
	end
})

return M
