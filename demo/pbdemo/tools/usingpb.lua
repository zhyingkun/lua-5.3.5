local protobuf = require("protobuf.protobuf")
local libdir = require("libdir")

local EscapeFiles = {
	"decompile.pb",
}
local function ShouldEscape(file)
	for _, name in ipairs(EscapeFiles) do
		if name == file then return true end
	end
	return false
end

for name, isdir in libdir.dirs("./pb") do
	-- decompile.pb depend on other pb
	if not isdir and name:find(".pb$") and not ShouldEscape(name) then
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
