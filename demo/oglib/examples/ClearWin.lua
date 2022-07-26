local viewID = 0
local viewID1 = 1
local function setup(glfw, bcfx, win)
    glfw.setKeyCallback(win, function(win, key, scancode, action, mods)
        if key == glfw.keyboard.ESCAPE and action == glfw.input_state.PRESS then
            glfw.setWindowShouldClose(win, true)
        end
    end)
    glfw.setFramebufferSizeCallback(win, function(window, width, height)
        bcfx.setViewRect(viewID, 0, 0, width, height)
        bcfx.setViewRect(viewID1, 30, 30, width // 2, height // 2)
    end)
    local w, h = glfw.getFramebufferSize(win)
    bcfx.setViewRect(viewID, 0, 0, w, h)
    bcfx.setViewClear(viewID, bcfx.clear_flag_mask.Color, bcfx.color.cyan, 1.0, 0)
    bcfx.setViewRect(viewID1, 30, 30, w // 2, h // 2)
    bcfx.setViewClear(viewID1, bcfx.clear_flag_mask.Color, bcfx.color.red, 1.0, 0)
end
local function loop(bcfx)
    bcfx.touch(viewID)
    bcfx.touch(viewID1)
end
local function clear(bcfx)
end

return {
    setup = setup,
    loop = loop,
    clear = clear,
}
