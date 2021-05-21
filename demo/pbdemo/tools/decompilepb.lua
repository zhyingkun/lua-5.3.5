local pbdir = arg[1]
local protodir = arg[2]

if not pbdir or not protodir then
	error("Usage: lua " .. arg[0] .. " <pbdir> <protodir>")
end

local pb2proto = require("protobuf.pb2proto")
function decompilepb(pbPath, protoPath)
	local fd = io.open(pbPath, "rb")
	local pbbin = fd:read("a")
	fd:close()
	fd = nil
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
