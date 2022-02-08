local libmad = require("libmadwrap")

local libuv = require("libuv")
local queueWorkAsync = libuv.queueWorkAsync
local queueWorkAsyncWait = libuv.queueWorkAsyncWait

---@class mad:table
local mad = {}

--[[
** {======================================================
** MAD
** =======================================================
--]]

---@param mb luaL_MemBuffer
---@return luaL_MemBuffer | nil, integer
function mad.madDecode(mb)
	return libmad.madDecode(mb)
end
---@param mb luaL_MemBuffer
---@param callback fun(mb:luaL_MemBuffer | nil, errCode:integer):void
function mad.madDecodeAsync(mb, callback)
	local ptr = libmad.packMadDecodeParam(mb)
	queueWorkAsync(libmad.madDecodePtr, ptr, function(result, status)
		callback(libmad.unpackMadDecodeResult(result))
	end)
end
---@param mb luaL_MemBuffer
---@return luaL_MemBuffer | nil, integer
function mad.madDecodeAsyncWait(mb)
	local ptr = libmad.packMadDecodeParam(mb)
	local result = queueWorkAsyncWait(libmad.madDecodePtr, ptr)
	return libmad.unpackMadDecodeResult(result)
end

-- }======================================================

return mad
