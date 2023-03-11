local libuv = require("libuv")
local fsevent = libuv.fsevent
local OK = libuv.err_code.OK
local CHANGE = fsevent.event_type.CHANGE
local RENAME = fsevent.event_type.RENAME
local RECURSIVE = fsevent.event_flag.RECURSIVE
local sys = libuv.sys

local reload = require("reload").reload

---@class watch:table
local watch = {}

local function error_msg(...)
	error(string.format(...))
end

---@type table<string, uv_fs_event_t>
local FilePath2Handle = {}
local function onFileChangedInternal(filePath, callback, onError)
	local lastTime = sys.hrTime()
	local function skip()
		local now = sys.hrTime()
		if now > lastTime and now - lastTime < 100000000 then
			return true
		end
		lastTime = now
		return false
	end
	local handle = fsevent.FsEvent()
	handle:startAsync(filePath, RECURSIVE, function(fileName, events, status, fsHandle)
		if status == OK then
			if events & CHANGE ~= 0 then
				if not skip() then
					callback(filePath)
				end
			elseif events & RENAME ~= 0 then
				error_msg("Error RENAME filePath: %s, fileName: %s", filePath, fileName)
			end
		else
			local handleStr = tostring(fsHandle)
			onError(filePath)
			error_msg(
				"Watch filePath error: %s, fileName: %s, handle: %s, status: %s",
				filePath,
				fileName,
				handleStr,
				tostring(status)
			)
		end
	end)
	return handle
end

---@param filePath string
---@param callback fun(filePath:string):void
local function onFileChanged(filePath, callback)
	if FilePath2Handle[filePath] then return end
	FilePath2Handle[filePath] = onFileChangedInternal(filePath, callback, function(path)
		local handle = FilePath2Handle[path]
		if handle then
			handle:closeAsync()
			FilePath2Handle[path] = nil
		end
	end)
end

watch.onFileChanged = onFileChanged

---@param modName string
function watch.autoReload(modName)
	local filePath = package.searchpath(modName, package.path)
	if not filePath then
		error_msg("Error Could not find lua module: %s", modName)
	end
	onFileChanged(filePath, function(path)
		local ok, msg = reload(modName)
		if ok then
			printerr("reload succeed:", modName, path)
		else
			printerr("reload failed:", modName, msg, path)
		end
	end)
end

return watch
