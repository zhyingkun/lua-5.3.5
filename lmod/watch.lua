local libuv = require("libuv")
local fs_event = libuv.fs_event
local OK = libuv.err_code.OK
local CHANGE = fs_event.event_type.CHANGE
local RENAME = fs_event.event_type.RENAME
local RECURSIVE = fs_event.event_flag.RECURSIVE

local reload = require("reload").reload

local function error_msg(...)
	error(string.format(...))
end

local function watch(mod_name, callback)
	local filepath = package.searchpath(mod_name, package.path)
	if not filepath then
		error_msg("Error Could not find lua module: %s", mod_name)
	end
	local handle = fs_event.new()
	handle:start(function(filename, events, status)
		if status == OK then
			if events & CHANGE ~= 0 then
				local ok, msg = reload(mod_name)
				if ok then
					print("reload succeed:", mod_name)
				else
					print("reload failed:", mod_name, msg)
				end
			elseif events & RENAME ~= 0 then
				error_msg("Error RENAME filepath: %s, filename: %s", filepath, filename)
			end
		else
			error_msg(
				"Watch filepath error: %s, filename: %s, handle: %s, status: %s",
				filepath,
				filename,
				tostring(handle),
				tostring(status)
			)
		end
	end, filepath, RECURSIVE)
end

return {
	watch = watch,
}
