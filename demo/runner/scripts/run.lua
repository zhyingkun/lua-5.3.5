local cpath
if os.sysname == "Windows" then
	cpath = ".\\cmods\\?.dll;"
else
	cpath = "./cmods/?.so;"
end
package.cpath = cpath .. package.cpath

local exe = require("libdir").basename(arg[0])
if os.sysname == "Windows" then
	exe = exe:sub(1, -5)
end

if exe == "runner" then
	print("Hello runner")
elseif exe == "task1" then
	print("Hello task1")
end
