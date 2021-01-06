print("Start ...")
local libuv = require("libuvwrap")
local loop = libuv.loop_new()

local fs = libuv.fs
local filename = "./word.txt"
local filerename = "./reword.txt"

-- async
fs.open(loop, filename, "r", 0, function(fd)
	print("In callback of fs.open:", fd)
	local function read_cb(result, str)
		print("In callback of fs.read:", result)
		if str ~= nil then
			print("Read from file:", str)
			fs.read(loop, fd, -1, read_cb)
		else
			fs.close(loop, fd, function()
				print("In callback of fs.close")
			end)
		end
	end
	fs.read(loop, fd, -1, read_cb)
end)

-- local ret = true
-- while ret do
-- 	print("Start while and run ...")
-- 	ret = libuv.run(loop, "nowait") ~= 0
-- end

local ret = libuv.run(loop, "default")
print("ret from libuv.run:", ret)

-- sync
local fd = fs.open(loop, filename, "r", 0)
print("after sync fs.open:", fd)
local result, str = fs.read(loop, fd, -1)
print("after sync fs.read:", result, str)
local ret = fs.close(loop, fd)
print("after sync fs.close:", ret)

local ret = fs.rename(loop, filename, filerename)
print("after sync fs.rename:", ret)

local ret = libuv.run(loop)
print("ret from libuv.run:", ret)

ret = libuv.loop_close(loop)
print("ret from libuv.loop_close:", ret)
loop = nil
print("End.")
