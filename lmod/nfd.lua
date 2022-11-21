local libnfd = require("libnfd")

---@class nfd:table
local nfd = {}

---@class libnfd_result
---@field public ERROR integer
---@field public OKAY integer
---@field public CANCEL integer

---@type libnfd_result
nfd.result = libnfd.result

---@overload fun():libnfd_result, string | nil
---@overload fun(filterList:string):libnfd_result, string | nil
---@overload fun(filterList:string, defaultPath:string):libnfd_result, string | nil
---@param filterList string
---@param defaultPath string
---@return libnfd_result, string | nil @result, path/errMsg/nothing
function nfd.openDialog(filterList, defaultPath)
	return libnfd.OpenDialog(filterList, defaultPath)
end

---@overload fun():libnfd_result, string[] | string | nil
---@overload fun(filterList:string):libnfd_result, string[] | string | nil
---@overload fun(filterList:string, defaultPath:string):libnfd_result, string[] | string | nil
---@param filterList string
---@param defaultPath string
---@return libnfd_result, string[] | string | nil @result, pathList/errMsg/nothing
function nfd.openDialogMultiple(filterList, defaultPath)
	return libnfd.OpenDialogMultiple(filterList, defaultPath)
end

---@overload fun():libnfd_result, string | nil
---@overload fun(filterList:string):libnfd_result, string | nil
---@overload fun(filterList:string, defaultPath:string):libnfd_result, string | nil
---@param filterList string
---@param defaultPath string
---@return libnfd_result, string | nil @result, path/errMsg/nothing
function nfd.saveDialog(filterList, defaultPath)
	return libnfd.SaveDialog(filterList, defaultPath)
end

---@overload fun():libnfd_result, string | nil
---@overload fun(defaultPath:string):libnfd_result, string | nil
---@param defaultPath string
---@return libnfd_result, string | nil @result, path/errMsg/nothing
function nfd.pickFolder(defaultPath)
	return libnfd.PickFolder(defaultPath)
end

return nfd
