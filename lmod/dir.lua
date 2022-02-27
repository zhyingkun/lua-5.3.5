local libdir = require("libdir")

---@class dir:table
local dir = {}

---@alias DirIterSignature fun():string, boolean

---@param path string
---@return DirIterSignature
function dir.open(path)
	return libdir.open(path)
end
---@param path string
---@return DirIterSignature
function dir.dirs(path)
	return libdir.dirs(path)
end
---@overload fun(path:string):boolean, string, integer
---@overload fun(path:string, onlyOne:boolean):boolean, string, integer
---@param path string
---@param onlyOne boolean
---@return boolean, string, integer @errStr, errCode
function dir.mkdir(path, onlyOne)
	return libdir.mkdir(path, onlyOne)
end
---@param path string
---@return boolean, string, integer @errStr, errCode
function dir.rmdir(path)
	return libdir.rmdir(path)
end
---@return string | nil, nil | string, nil | integer @ path, errStr, errCode
function dir.cwd()
	return libdir.cwd()
end
---@param path string
---@return boolean, string, integer @errStr, errCode
function dir.chdir(path)
	return libdir.chdir(path)
end
---@param path string
---@return string
function dir.abspath(path)
	return libdir.abspath(path)
end
---@param path string
---@return string
function dir.dirname(path)
	return libdir.dirname(path)
end
---@param path string
---@return string
function dir.basename(path)
	return libdir.basename(path)
end

return dir
