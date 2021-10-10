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

local cache_handles = {}

local function unwatch(mod_name)
	local handle = cache_handles[mod_name]
	if handle then
		handle:close()
		cache_handles[mod_name] = nil
	end
end

local function watch(mod_name, callback)
	local filepath = package.searchpath(mod_name, package.path)
	if not filepath then
		error_msg("Error Could not find lua module: %s", mod_name)
	end
	if cache_handles[mod_name] then return end
	local handle = fs_event.new()
	handle:start(function(filename, events, status)
		if status == OK then
			if events & CHANGE ~= 0 then
				local ok, msg = reload(mod_name)
				if ok then
					print_err("reload succeed:", mod_name)
				else
					print_err("reload failed:", mod_name, msg)
				end
			elseif events & RENAME ~= 0 then
				error_msg("Error RENAME filepath: %s, filename: %s", filepath, filename)
			end
		else
			local handle_str = tostring(handle)
			unwatch(mod_name)
			error_msg(
				"Watch filepath error: %s, filename: %s, handle: %s, status: %s",
				filepath,
				filename,
				handle_str,
				tostring(status)
			)
		end
	end, filepath, RECURSIVE)
	cache_handles[mod_name] = handle
end

return {
	unwatch = unwatch,
	watch = watch,
}
