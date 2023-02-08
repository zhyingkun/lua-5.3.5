local bcfx = require("bcfx")
local shader_type = bcfx.shader_type
local texture_format = bcfx.texture_format
local image = bcfx.image

local libuv = require("libuv")
local mbio = libuv.mbio

local watch = require("watch")

local loader = {}

local pathPrefix = ""
function loader.SetPathPrefix(prefix)
	pathPrefix = prefix
end

function loader.LoadProgram(name)
	local vsPath = pathPrefix .. "Resource/Shader/" .. name .. ".vert.glsl"
	local fsPath = pathPrefix .. "Resource/Shader/" .. name .. ".frag.glsl"
	local vs = bcfx.createShader(io.readfile(vsPath), shader_type.Vertex)
	local fs = bcfx.createShader(io.readfile(fsPath), shader_type.Fragment)
	local prog = bcfx.createProgram(vs, fs)
	watch.onFileChanged(vsPath, function()
		bcfx.destroy(vs)
		vs = bcfx.createShader(io.readfile(vsPath), shader_type.Vertex)
		bcfx.updateProgram(prog, vs, fs)
		printerr("Vertex shader reload completed!")
	end)
	watch.onFileChanged(fsPath, function()
		bcfx.destroy(fs)
		fs = bcfx.createShader(io.readfile(fsPath), shader_type.Fragment)
		bcfx.updateProgram(prog, vs, fs)
		printerr("Fragment shader reload completed!")
	end)
	return prog
end

function loader.LoadTexture(filename)
	local filePath = pathPrefix .. "Resource/Texture/" .. filename;
	local fileData = mbio.readFile(filePath)
	local parseMB, width, height = image.imageDecode(fileData, texture_format.RGBA8)
	return bcfx.createTexture2D(texture_format.RGBA8, parseMB, width, height)
end

function loader.loadMesh(fileName)
	local filePath = pathPrefix .. "Resource/Mesh/" .. fileName;
	local fileData = mbio.readFile(filePath)
	local meshs, materialLibraryNames = bcfx.mesh.meshParse(fileData)
	return meshs[1].vertex, meshs[1].index
end

local FaceNameList = {
	"right",  -- +x
	"left",   -- -x
	"bottom", -- +y
	"top",    -- -y
	"back",   -- +z
	"front",  -- -z
}
--[[
     top
left back right front
    bottom
]]
function loader.LoadTextureCubeMap(folderName)
	local filePrefix = (pathPrefix .. "Resource/Texture") / folderName
	local fileDataMB = {}
	local width, height
	for idx, faceName in ipairs(FaceNameList) do
		local filePath = filePrefix / (faceName .. ".jpg")
		local fileData, err, str = mbio.readFile(filePath)
		if not fileData then
			printerr("LoadTextureCubeMap error: ", err, str, filePath)
		end
		local parseMB, w, h = image.imageDecode(fileData, texture_format.RGB8)
		fileDataMB[idx] = parseMB
		if not width and not height then
			width, height = w, h
		else
			assert(width == w and height == h)
		end
	end
	return bcfx.createTextureCubeMap(texture_format.RGB8, fileDataMB, width, height, false)
end

return loader
