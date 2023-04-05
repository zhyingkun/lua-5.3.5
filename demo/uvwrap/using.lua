local libuv = require("libuv")
local fs = libuv.fs
libuv.init()

local function copyfile(from, to)
	local open_flag = fs.open_flag
	local fd_from = fs.open(from, open_flag.RDONLY, 0)
	local fd_to = fs.open(to, open_flag.WRONLY | open_flag.CREAT, 6 * 64 + 4 * 8 + 4) -- 0o644
	local str, len = fs.read(fd_from, -1) -- -1 means ignore offset
	fs.close(fd_from)
	local len = fs.write(fd_to, str, -1)
	fs.close(fd_to)
end

local function ScanDirectory()
	local folder = "."
	local fd, err = fs.openDir(folder)
	if err ~= libuv.err_code.OK then
		print("Error:", folder, libuv.errName(err), libuv.strError(err))
		return
	end
	local pathList = setmetatable({}, table)
	repeat
		local relativePath, type, err = fs.readDir(fd)
		print(relativePath, type, err)
		if relativePath then
			pathList:insert(relativePath)
		end
	until relativePath == nil
	fs.closeDir(fd)
	return pathList
end

print(tostring(fs.scanDir("demo/pbdemo/", true), 1))
