-- ����GB2312ģ��
local gb2312 = require("libgb2312")

local str = "��a�b��c�"

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

local dbg = "��"
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

--[=[
关于GB2312编码的一点故事：
1. x86 系列 CPU 支持软中断 INT 3 用于调试，其机器码为一个字节的 0xCC（故意设计成一个字节，方便调试器使用）
2. VisualStudio 的调试模式下，新分配的未初始化内存空间会被默认初始化为 0xCC（目的是程序异常跳转到此处就会触发调试中断）
3. Windows 默认使用 GB2312 编码（通过 cmd 中的 chcp 命令可得到活动代码页为 936， 对应的就是 GB2312）
4. GB2312 编码中，烫的机器码为 0xCCCC
5. 当使用 printf 之类的函数打印未初始化内存的内容时，就会得到“烫”字 
]=]
