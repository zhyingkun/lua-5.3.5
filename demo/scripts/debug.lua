local cmods = {
	bcfx = true,
	boolarray = true,
	dir = true,
	gb2312 = true,
	glfwrap = true,
	hello = true,
	json = true,
	lproc = true,
	luasocket = true,
	madwrap = true,
	nuklear = true,
	portaudio = true,
	protobuf = true,
	uvwrap = true,
}

local patterns = {
	MacOSX = "buildXcode/cmod/%s/Debug/?.so;",
	Windows = [[buildVS\cmod\%s\Debug\?.dll;]],
	Linux = "build/cmod/%s/?.so;",
}
local pattern = patterns[os.sysname]

local searchDirs = {}
for cmod in pairs(cmods) do
	table.insert(searchDirs, string.format(pattern, cmod))
end

package.cpath = table.concat(searchDirs) .. package.cpath

local lmodpath = "lmod"
local path1 = lmodpath / "?.lua"
local path2 = lmodpath / "?" / "init.lua"
package.path = path1 .. ";" .. path2 .. ";" .. package.path
