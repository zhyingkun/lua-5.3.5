local pbdir = arg[1] -- maybe only one pb file
local protodir = arg[2]

if not pbdir then
	local usage = [[
	Usage: lua %s <pbdir> <protodir>
	Or: lua %s <pbfile>]]
	local script = arg[0]
	print(string.format(usage, script, script))
	return
end

local pb2proto = require("protobuf.pb2proto")
local function ReadBinaryFileAll(file)
	local fd = io.open(file, "rb")
	local binary = fd:read("a")
	fd:close()
	return binary
end

if not protodir then
	local pbbin = ReadBinaryFileAll(pbdir)
	local proto = pb2proto(pbbin)
	print(proto)
	return
end 

function decompilepb(pbPath, protoPath)
	local pbbin = ReadBinaryFileAll(pbPath)
	local proto = pb2proto(pbbin)
	io.open(protoPath, "wb"):write(proto):close()
end

local libdir = require("libdir")
local function walkpath(dir, func)
	local function walkinternal(prefix)
		for name, isdir in libdir.dirs(dir .. prefix) do
			if isdir then
				if name ~= "." and name ~= ".." then
					walkinternal(prefix .. name .. "/")
				end
			else
				if name:find(".pb$") then
					func(prefix, name)
				end
			end
		end
	end
	walkinternal("")
end

local function MakeHasSlash(dir)
	return dir:byte(-1) == ("/"):byte(1) and dir or dir .. "/"
end

pbdir = MakeHasSlash(pbdir)
protodir = MakeHasSlash(protodir)

walkpath(pbdir, function(prefix, name)
	-- print(pbdir .. prefix .. name, protodir .. prefix .. name:sub(1, -3) .. "proto")
	libdir.mkdir(protodir .. prefix, false)
	decompilepb(pbdir .. prefix .. name, protodir .. prefix .. name:sub(1, -3) .. "proto")
end)
