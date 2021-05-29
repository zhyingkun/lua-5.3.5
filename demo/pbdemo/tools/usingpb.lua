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

local tbl = {
	name = {
		"john",
	},
	count = 1,
}
for i = 1, 19, 1 do
	table.insert(tbl.name, tostring(i))
end
local wdbin = protobuf.encode("zykTest.Simple", tbl)
-- io.open("pb/err.wdbin", "wb"):write(wdbin):close()
local obj = protobuf.decode("zykTest.Simple", wdbin)
print(tostring(obj, 16))

-- local btbl = {}
-- for b in string.bytes(wdbin) do
-- 	table.insert(btbl, string.format("0x%X", b))
-- end
-- print(table.concat(btbl, ", "))
