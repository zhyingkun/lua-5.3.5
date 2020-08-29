-- Demo for GB2312
local gb2312 = require("libgb2312")

local function GB2312toUTF8(str)
	local next, str, key = gb2312.codes(str)
	return utf8.char(function()
		key, code = next(str, key)
		return code
	end)
end

local function UTF8toGB2312(str)
	local next, str, key = utf8.codes(str)
	return gb2312.char(function()
		key, code = next(str, key)
		return code
	end)
end

local str = UTF8toGB2312("苍a穹b浩c瀚")

print("#str: ", #str) --> 11
print(gb2312.offset(str, 5)) --> 7
print(gb2312.offset(str, 0, 8)) --> 7
print(gb2312.offset(str, -2, 7)) --> 4
print(gb2312.offset(str, 1, 7)) --> 7
print(gb2312.offset(str, 2, 7)) --> 9
print(gb2312.offset(str, -1, -2)) --> 9
print(gb2312.offset(str, 4, -8)) --> 9
print("Empty string: ", gb2312.offset("", 0)) --> 1

print(gb2312.codepoint(str, 1, -1)) --> 33485	97	31353	98	28009	99	28698

local str2 = gb2312.char(33485, 97, 31353, 98, 28009, 99, 28698)

local dbg = UTF8toGB2312("好")
local bytecode = string.format("0x%X%X", string.byte(dbg, 1, -1))

if os.sysname == "Windows" then
	print(str)
	print("codepoint to char: ", str2)
	print(dbg, "(GB2312) => ", bytecode)
else
	print(GB2312toUTF8(str))
	print("codepoint to char: ", GB2312toUTF8(str2))
	print(GB2312toUTF8(dbg), "(GB2312) => ", bytecode)
end
