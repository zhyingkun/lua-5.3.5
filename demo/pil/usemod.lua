print("======================================================================")
do
	local array = require("libboolarray")
	local a = array.new(34)
	local b = array.new(86)

	a[4] = true
	a[5] = true
	a[33] = true
	a[34] = true

	b[5] = true
	b[33] = true
	b[65] = true
	b[77] = true

	local function showarray(str, a)
		local tbl = {}
		for k in pairs(a) do tbl[#tbl + 1] = k end
		print(str, table.unpack(tbl))
	end

	showarray("      a:", a)
	showarray("      b:", b)
	showarray("a and b:", a * b)
	showarray("a or  b:", a + b)
end
print("======================================================================")
do
	local dir = require("libdir")
	local tbl = {}
	for name in dir.open(".") do  tbl[#tbl + 1] = name end
	print("ls:", table.unpack(tbl))
	print("cwd:", dir.cwd())
	print("mkdir:", dir.mkdir("_tmp_"))
	print("chdir:", dir.chdir("_tmp_"))
	print("cwd:", dir.cwd())
	print("chdir:", dir.chdir(".."))
	print("rmdir:", dir.rmdir("_tmp_"))
end
print("======================================================================")
do
	local hello = require("libhello")
	hello.hellomod()
	print("42.0 + 13.0  =", hello.add(42, 13))
	print("Average and sum: 13, 45, 74 ==>", hello.foo(13, 45, 74))
	world = {
		hi = "Hello World!",
		singSong = function(name)
			print("Who?", name)
			return 42
		end,
	}
	function lfunc(num) print("In lfunc:", num) end
	hello.cfunc()
	function lfuncyieldable(num)
		print(num)
		coroutine.yield("Middle Yield >>>>>>>>")
	end
	local co = coroutine.create(function()
		coroutine.yield("First  Yield >>>>>>>>")
		hello.cfuncyieldable()
		coroutine.yield("Last   Yield >>>>>>>>")
	end)
	while coroutine.status(co) ~= "dead" do print(coroutine.resume(co)) end
end
print("======================================================================")
do
end
print("======================================================================")

local lproc = nil
local loadlib = pcall(function()
	lproc = require("liblproc")
end)
if loadlib == false then
	print("Could not find liblproc, may be not implement, such as Windows!")
	return
end
local send = [===[
	print("send thread start...")
	local lproc = require("liblproc")
	lproc.send("usemod", "liblproc", 12.5, 42, false, {})
	print("send thread end.")
]===]
local receive = [===[
	print("receive thread start...")
	local lproc = require("liblproc")
	print(lproc.receive("usemod"))
	print(lproc.receive("main"))
	print("receive thread end.")
]===]
print("lproc: main start...")
lproc.start(receive)
lproc.start(send)
lproc.send("main", "liblproc", 15.5, nil)
print("lproc: main end.")
print("======================================================================")
lproc.exit() -- current is main thread, should wait for other thread
