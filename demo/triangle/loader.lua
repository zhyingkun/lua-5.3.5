local bcfx = require("bcfx")
local shader_type = bcfx.shader_type

local watch = require("watch")

local loader = {}

local pathPrefix = ""
function loader.SetPathPrefix(prefix)
	pathPrefix = prefix
end

function loader.LoadProgram(name)
	local vsPath = pathPrefix .. "Resource/Shader/" .. name .. "_vs.glsl"
	local fsPath = pathPrefix .. "Resource/Shader/" .. name .. "_fs.glsl"
	local vs = bcfx.createShader(io.readfile(vsPath), shader_type.Vertex)
	local fs = bcfx.createShader(io.readfile(fsPath), shader_type.Fragment)
	local prog = bcfx.createProgram(vs, fs)
	watch.onFileChanged(vsPath, function()
		bcfx.destroy(vs)
		vs = bcfx.createShader(io.readfile(vsPath), shader_type.Vertex)
		bcfx.updateProgram(prog, vs, fs)
	end)
	watch.onFileChanged(fsPath, function()
		bcfx.destroy(fs)
		fs = bcfx.createShader(io.readfile(fsPath), shader_type.Fragment)
		bcfx.updateProgram(prog, vs, fs)
	end)
	return prog
end

function loader.LoadTexture(filename)
	local filePath = pathPrefix .. "Resource/Texture/" .. filename;
	local fileData = bcfx.utils.ReadFile(filePath)
	local parseMB = bcfx.utils.ImageParse(fileData)
	return bcfx.createTexture(parseMB)
end

return loader
