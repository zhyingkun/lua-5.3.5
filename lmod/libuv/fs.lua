local uvwrap = require("libuvwrap")
local loop = uvwrap.default_loop()

local fs = uvwrap.fs
local fs_open = fs.open
local fs_close = fs.close
local fs_read = fs.read

local M = {}

function M.open(filename, flags, mode, cb)
	return fs_open(loop, filename, flags, mode, cb)
end

function M.close(fd, cb)
	return fs_close(loop, fd, cb)
end

function M.read(fd, offset, cb)
	return fs_read(loop, fd, offset, cb)
end

return M
