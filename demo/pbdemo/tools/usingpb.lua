local protobuf = require("protobuf.protobuf")
local libdir = require("libdir")

for name, isdir in libdir.dirs("./pb") do
	if not isdir and name:find(".pb$") then
		-- print("zykTest:", name)
		protobuf.register_file("./pb/" .. name)
	end
end

local wdbin = protobuf.encode("zykTest.Simple", {
	name = "john",
	count = 1,
})
local obj = protobuf.decode("zykTest.Simple", wdbin)
print(tostring(obj, 1))
