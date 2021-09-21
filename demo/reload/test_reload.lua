local function copy_file(from, to)
	io.writefile(to, io.readfile(from))
end

copy_file("./mod_old.lua", "./mymod.lua")

print("mymod file:", package.searchpath("mymod", package.path))
local mymod = require("mymod")

mymod.init()
local obj = mymod.new()
local count_func = mymod.get_count_func()

local tmp = {}
tmp[mymod.increase_a] = mymod.increase_a
global_b = mymod.increase_b

local thread = coroutine.wrap(function()
	local global_b = global_b
	print("In Thread:", global_b)
	coroutine.yield()
	print("In Thread Another:", global_b)
end)

local function test_mod()
	print("TEST START... ========================================================")
	print("Increase a:", mymod.increase_a())
	print("Increase b:", mymod.increase_b())
	print("Count:", count_func())
	obj:show()
	print("Enum:", tostring(mymod.enum, 1))
	print("Function as key:", tostring(tmp, 1), "mymod.increase_a:", mymod.increase_a)
	thread()
	print("global_b:", global_b, "mymod.increase_b:", mymod.increase_b)
	assert(tmp[mymod.increase_a] == mymod.increase_a)
	print("TEST END. ============================================================")
end

print("BEFORE Update:")
test_mod()

copy_file("./mod_new.lua", "./mymod.lua")

local reload = require("reload").reload
print("Execute reload:", reload("mymod"))

print("AFTER Update:")
test_mod()

os.remove("./mymod.lua")
