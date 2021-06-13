local zyk = require("protobuf.pbc")
local yunfeng = require("protobuf.protobuf")

local function RegisterAllPB(pbModule)
	local EscapeFiles = {
		"decompile.pb", -- decompile.pb depend on other pb
	}
	local function ShouldEscape(file)
		for _, name in ipairs(EscapeFiles) do
			if name == file then return true end
		end
		return false
	end
	for name, isdir in require("libdir").dirs("./pb") do
		if not isdir and name:find(".pb$") and not ShouldEscape(name) then
			-- print("zykTest:", name)
			pbModule.register_file("./pb/" .. name)
		end
	end
end
RegisterAllPB(zyk)
RegisterAllPB(yunfeng)

local function PrintDefaultValue(pbModule)
	local defVal = pbModule.decode("zykTest.DefaultValue", "")
	print("\tdefVal:", defVal.zigzag)
	print("\tdefVal:", defVal.str)
	print("\tdefVal:", defVal.number)
	print("\tdefVal:", defVal.fixint)
	print("\tdefVal:", defVal.negint)
end
print("Default Value from zyk wrap:")
PrintDefaultValue(zyk)
print("Default Value from yunfeng wrap:")
PrintDefaultValue(yunfeng)

local function PrintBinaryInHex(binary)
	local btbl = {}
	for b in string.bytes(binary) do
		table.insert(btbl, string.format("0x%X", b))
	end
	print(table.concat(btbl, ", "))
end

local function TestProtobuf(pbModule)
	local tbl = {
		name = {
			"john",
		},
		count = 1,
	}
	for i = 1, 19, 1 do
		table.insert(tbl.name, tostring(i))
	end
	local wdbin = pbModule.encode("zykTest.Simple", tbl)
	-- io.open("pb/err.wdbin", "wb"):write(wdbin):close()
	local obj = pbModule.decode("zykTest.Simple", wdbin)
	print("Print raw table:", tostring(obj, 16))

	local repnd = pbModule.decode("zykTest.Simple", "") -- repeated no default
	print("repnd:", tostring(repnd, 2))
	print("repnd.name:", repnd.name)
	print("repnd.count:", repnd.count)
	for i = 0, 3, 1 do print("repnd.name[" .. tostring(i) .. "]:", repnd.name[i]) end
	print("repnd.sub:", tostring(repnd.sub, 1), repnd.sub.len, repnd.sub.foo, "end")
end
print("Test protobuf with zyk wrap:")
TestProtobuf(zyk)
print("Test protobuf with yunfeng wrap:")
TestProtobuf(yunfeng)
