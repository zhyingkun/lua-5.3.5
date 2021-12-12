local libbcfx = require("libbcfx")
local mbio = libbcfx.mbio
local image = libbcfx.image

local running = coroutine.running
local yield = coroutine.yield
local resume = coroutine.resume

local ASYNC_WAIT_MSG = "AsyncWait api must running in coroutine"

local bcfx = {}

function bcfx.writeFileAsync(fileName, mb, callback)
	local ptr = mbio.PackWriteFileParam(fileName, mb)
	libuv.queue_work(mbio.WriteFilePtr, ptr, function(status, result)
		local ret = mbio.UnpackWriteFileResult(result)
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
	return image.ImageEncode(mb, x, y, comp, type, sorq)
end
function bcfx.imageEncodeAsync(mb, x, y, comp, type, sorq, callback) -- stride or quality
	local ptr = image.PackImageEncodeParam(mb, x, y, comp, type, sorq);
	libuv.queue_work(image.ImageEncodePtr, ptr, function(status, result)
		local mb = image.UnpackImageEncodeResult(result)
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
