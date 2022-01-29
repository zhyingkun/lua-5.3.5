local function copyfile(from, to)
	local libuv = require("libuv")
	local fs = libuv.fs
	local open_flag = fs.open_flag
	local fd_from = fs.open(from, open_flag.RDONLY, 0)
	local fd_to = fs.open(to, open_flag.WRONLY | open_flag.CREAT, 6 * 64 + 4 * 8 + 4) -- 0o644
	local str, len = fs.read(fd_from, -1) -- -1 means ignore offset
	fs.close(fd_from)
	local len = fs.write(fd_to, str, -1)
	fs.close(fd_to)
end
