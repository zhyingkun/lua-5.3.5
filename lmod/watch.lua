local libuv = require("libuv")
local fs_event = libuv.fs_event
local OK = libuv.err_code.OK
local CHANGE = fs_event.event_type.CHANGE
local RENAME = fs_event.event_type.RENAME
local RECURSIVE = fs_event.event_flag.RECURSIVE
local sys = libuv.sys

local reload = require("reload").reload

local watch = {}

local function error_msg(...)
	error(string.format(...))
end

local FilePath2Handle = {}
local function onFileChangedInternal(filePath, callback, onError)
	local lastTime = sys.hrtime()
	local function skip()
		local now = sys.hrtime()
		if now > lastTime and now - lastTime < 100000000 then
			return true
		end
		lastTime = now
		return false
	end
	local handle = fs_event.new()
	handle:start(function(fileName, events, status)
		if status == OK then
			if events & CHANGE ~= 0 then
				if not skip() then
					callback(filePath)
				end
			elseif events & RENAME ~= 0 then
				error_msg("Error RENAME filePath: %s, fileName: %s", filePath, fileName)
			end
		else
			local handleStr = tostring(handle)
			onError(filePath)
			error_msg(
				"Watch filePath error: %s, fileName: %s, handle: %s, status: %s",
				filePath,
				fileName,
				handleStr,
				tostring(status)
			)
		end
	end, filePath, RECURSIVE)
	return handle
end
local function onFileChanged(filePath, callback)
	if FilePath2Handle[filePath] then return end
	FilePath2Handle[filePath] = onFileChangedInternal(filePath, callback, function(filePath)
		local handle = FilePath2Handle[filePath]
		if handle then
			handle:close()
			FilePath2Handle[filePath] = nil
		end
	end)
end

watch.onFileChanged = onFileChanged

function watch.autoReload(modName)
	local filePath = package.searchpath(modName, package.path)
	if not filePath then
		error_msg("Error Could not find lua module: %s", modName)
	end
	onFileChanged(filePath, function(filePath)
		local ok, msg = reload(modName)
		if ok then
			print_err("reload succeed:", modName)
		else
			print_err("reload failed:", modName, msg)
		end
	end)
end

return watch
