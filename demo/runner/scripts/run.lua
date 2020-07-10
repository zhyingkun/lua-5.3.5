local cpath
local isWin = os.sysname == "Windows"
if isWin then
	cpath = ".\\cmods\\?.dll;"
else
	cpath = "./cmods/?.so;"
end
package.cpath = cpath .. package.cpath

local exe = require("libdir").basename(arg[0])
if os.sysname == "Windows" then
	exe = exe:sub(1, -5)
end

local tasks = {
	runner = function()
		print("Good Good Study, Day Day Up!")
	end,
	task1 = function()
		require("scripts/task1")
	end,
}

local function run()
	local task = tasks[exe]
	if type(task) ~= "function" then
		task = loadfile("scripts/" .. exe .. ".lua")
	end
	if type(task) == "function" then
		task()
	else
		error("No task to run!")
	end
end

print("Start ...")
local ret, msg = pcall(run)
if not ret then
	print(debug.traceback(msg))
end
print("End.")

if isWin then
	os.execute("pause")
end
