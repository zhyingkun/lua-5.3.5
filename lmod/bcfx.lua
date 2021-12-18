local libbcfx = require("libbcfx")
local mbio = libbcfx.mbio
local image = libbcfx.image

local running = coroutine.running
local yield = coroutine.yield
local resume = coroutine.resume

local ASYNC_WAIT_MSG = "AsyncWait api must running in coroutine"

local bcfx = {}

function bcfx.writeFileAsync(fileName, mb, callback)
	local ptr = mbio.packWriteFileParam(fileName, mb)
	libuv.queue_work(mbio.writeFilePtr, ptr, function(status, result)
		local ret = mbio.unpackWriteFileResult(result)
		callback(ret)
	end)
end
function bcfx.writeFileAsyncWait(fileName, mb)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	bcfx.writeFileAsync(fileName, mb, function(ret)
		resume(co, ret)
	end)
	return yield()
end

function bcfx.imageEncode(mb, x, y, comp, type, sorq)
	return image.imageEncode(mb, x, y, comp, type, sorq)
end
function bcfx.imageEncodeAsync(mb, x, y, comp, type, sorq, callback) -- stride or quality
	local ptr = image.packImageEncodeParam(mb, x, y, comp, type, sorq);
	libuv.queue_work(image.imageEncodePtr, ptr, function(status, result)
		local mb = image.unpackImageEncodeResult(result)
		callback(mb)
	end)
end
function bcfx.imageEncodeAsyncWait(mb, x, y, comp, type, sorq)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	bcfx.imageEncodeAsync(mb, x, y, comp, type, sorq, function(mb)
		resume(co, mb)
	end)
	return yield()
end

return libbcfx
