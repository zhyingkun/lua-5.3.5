local exe = require("libdir").basename(arg[0])
local cpath
if os.sysname == "Windows" then
	cpath = ".\\cmods\\?.dll;"
	exe = exe:sub(1, -5)
else
	cpath = "./cmods/?.so;"
end
package.cpath = cpath .. package.cpath

if exe == "runner" then
	print("Hello runner")
elseif exe == "task1" then
	print("Hello task1")
end
