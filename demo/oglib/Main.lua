local glfw = require("glfw")
local bcfx = require("bcfx")

glfw.init()

glfw.windowHint(glfw.window_hint.CONTEXT_VERSION_MAJOR, 4)
glfw.windowHint(glfw.window_hint.CONTEXT_VERSION_MINOR, 1)
glfw.windowHint(glfw.window_hint.OPENGL_PROFILE, glfw.hint_value.OPENGL_CORE_PROFILE)
glfw.windowHint(glfw.window_hint.OPENGL_FORWARD_COMPAT, glfw.hint_value.TRUE)
local win = glfw.createWindow(400, 300, "GLFW win")

bcfx.init(win)

local example = require("examples/ClearWin")
local example = require("examples/Triangle")
local example = require("examples/RotateTriangle")
local example = require("examples/Texture")
local example = require("examples/Cube")
local example = require("examples/Instance")
local example = require("examples/ObjModel")
local example = require("examples/Framebuffer")
local example = require("examples/Capture")
local example = require("examples/ImGUI")
example.setup(glfw, bcfx, win)

while not glfw.windowShouldClose(win) do
    glfw.pollEvents()
    example.loop(bcfx)
    bcfx.apiFrame()
end

example.clear(bcfx)

bcfx.shutdown()
glfw.terminate()
