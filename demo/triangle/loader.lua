local bcfx = require("bcfx")
local shader_type = bcfx.shader_type

local loader = {}

local pathPrefix = ""
function loader.SetPathPrefix(prefix)
	pathPrefix = prefix
end

function loader.LoadProgram(name)
	local vsStr = io.readfile(pathPrefix .. "Resource/Shader/" .. name .. "_vs.glsl")
	local fsStr = io.readfile(pathPrefix .. "Resource/Shader/" .. name .. "_fs.glsl")
	local vs = bcfx.createShader(vsStr, shader_type.Vertex)
	local fs = bcfx.createShader(fsStr, shader_type.Fragment)
	return bcfx.createProgram(vs, fs)
end

function loader.LoadTexture(filename)
	local filePath = pathPrefix .. "Resource/Texture/" .. filename;
	local fileData = bcfx.utils.ReadFile(filePath)
	local parseMB = bcfx.utils.ImageParse(fileData)
	return bcfx.createTexture(parseMB)
end

return loader
