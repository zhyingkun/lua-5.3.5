local libuv = require("libuv")
local glfw = require("glfw")

local bcfx = require("bcfx")
local clear_flag = bcfx.clear_flag
local vertex_attrib = bcfx.vertex_attrib
local attrib_type = bcfx.attrib_type
local shader_type = bcfx.shader_type
local color = bcfx.color


local vsStr = [[
#version 410 core
in vec2 a_position;
in vec3 a_color0;

out vec3 v_color0;

void main() {
	gl_Position = vec4(a_position, 0.0, 1.0);
	v_color0 = a_color0;
}
]]
local fsStr = [[
#version 410 core
in vec3 v_color0;

out vec4 FragColor;

void main() {
	FragColor = vec4(v_color0, 1.0);
	// FragColor = vec4(v_color0.b, 0.0, 0.0, 1.0);
}
]]

function GenerateColorCircle(radius)
	local ColorDefine = {
		"chartreuse", "yellow", "orange", "red",
		"fuchsia", "magenta", "purple", "blue",
		"indigo", "cyan", "turquoise", "green",
	}
	local vertex = {0.0, 0.0, 1.0, 1.0, 1.0}
	local indices = {}
	local insert = table.insert
	for i = 1, 12, 1 do
		local theta = math.rad((i - 1) * 30)
		local x = radius * math.cos(theta)
		local y = radius * math.sin(theta)
		insert(vertex, x)
		insert(vertex, y)
		local r, g, b = color.unpackf(color[ColorDefine[i]])
		insert(vertex, r)
		insert(vertex, g)
		insert(vertex, b)
		insert(indices, 0)
		insert(indices, i)
		insert(indices, i % 12 + 1)
	end
	local layout = bcfx.vertexlayout.new()
	layout:add(vertex_attrib.Position, 2, attrib_type.Float, false)
	layout:add(vertex_attrib.Color0, 3, attrib_type.Float, false)
	local layoutHandle = bcfx.createVertexLayout(layout)
	local ptr, sz = bcfx.MakeFloatArray(vertex, #vertex)
	local vertexHandle = bcfx.createVertexBuffer(ptr, sz, layoutHandle)

	local ptr, sz = bcfx.MakeUintArray(indices, #indices)
	local idxHandle = bcfx.createIndexBuffer(ptr, sz)

	local vertexShaderHandle = bcfx.createShader(vsStr, shader_type.Vertex)
	local fragmentShaderHandle = bcfx.createShader(fsStr, shader_type.Fragment)
	local shaderProgramHandle = bcfx.createProgram(vertexShaderHandle, fragmentShaderHandle)

	return vertexHandle, idxHandle, shaderProgramHandle
end


local vertexHandle
local idxHandle
local shaderProgramHandle
local viewID = 5

local function setup(mainWin)
	vertexHandle, idxHandle, shaderProgramHandle = GenerateColorCircle(0.7)

	local win = glfw.CreateWindow(600, 600, "ColorCircle", nil, mainWin)
	if not win then
		print("GLFW CreateWindow Error:", glfw.GetError())
		glfw.Terminate()
		return
	end

	bcfx.setViewWindow(viewID, win)
	bcfx.setViewRect(viewID, 0, 0, glfw.GetFramebufferSize(win))
	bcfx.setViewClear(viewID, clear_flag.COLOR, color.white, 0.0, 0)
	glfw.SetFramebufferSizeCallback(win, function(window, width, height)
		bcfx.setViewRect(viewID, 0, 0, width, height)
	end)
end

local function tick(delta)
	bcfx.setVertexBuffer(0, vertexHandle)
	bcfx.setIndexBuffer(idxHandle)

	bcfx.submit(viewID, shaderProgramHandle)
end

return {
	setup = setup,
	tick = tick,
}
