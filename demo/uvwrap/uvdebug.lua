package.cpath = "/Users/zyk/workspace/Lang/Lua/lua-5.3.5/buildXcode/cmod/uvwrap/Debug/?.so;" .. package.cpath

print("Hello World")

local libuv = require("libuvwrap")
local loop = libuv.loop_new()

-- libuv.fs.scandir(loop, "/Users/zyk/Tmp/TestLibuvScanDir", 0, function(result, files)
-- 	print(result, tostring(files, 1))
-- end)

-- local function GetFilesInDir(path)
-- 	local ret, files = libuv.fs.scandir(loop, path, 0);
-- 	print("sync", ret, tostring(files, 1))
-- 	return files
-- end
-- local path = "/Users/zyk/Tmp/TestLibuvScanDir"
-- local files = GetFilesInDir(path)
-- for name, type in pairs(files) do
-- 	if type == "dir" then
-- 		local files = GetFilesInDir(path .. "/" .. name)
-- 	end
-- end

local tcp = libuv.tcp
local ret, server = tcp.new(loop)
server:bind("0.0.0.0", 8080, false)
print("server", server)
server:listen(128, function(server, status)
	local ret, client = tcp.new(loop)
	local ret = server:accept(client)
	print(server, status, client)
	client:read_start(function(client, nread, data)
		print("on read_start:", client, nread, data)
		client:write("Hello World", function(status)
			print("on write:", status)
		end)
	end)
end)


libuv.run(loop)
libuv.loop_close(loop)
