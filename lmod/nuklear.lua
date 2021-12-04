local libnk = require("libnuklear")

local nk = {}

function nk.font_config()
	return libnk.font_config()
end

function nk.font_atlas()
	return libnk.font_atlas()
end

function nk.buffer()
	return libnk.buffer()
end

function nk.context(font)
	return libnk.context(font)
end

local curCtx
function nk.SetContext(ctx)
	curCtx = ctx
end

function nk.input_begin()
	libnk.input_begin(curCtx)
end
function nk.input_key(key, down)
	libnk.input_key(curCtx, key, down)
end
function nk.input_motion(x, y)
	libnk.input_motion(curCtx, x, y)
end
function nk.input_button(button, x, y, down)
	libnk.input_button(curCtx, button, x, y, down)
end
function nk.input_end()
	libnk.input_end(curCtx)
end

function nk.style_set_font(font)
	libnk.style_set_font(curCtx, font)
end

function nk.begin(title, x, y, w, h, flags)
	return libnk.begin(curCtx, title, x, y, w, h, flags)
end

function nk.endnk()
	libnk.endnk(curCtx)
end

function nk.layout_row_dynamic(height, cols)
	libnk.layout_row_dynamic(curCtx, height, cols)
end

function nk.label(str, alignment)
	libnk.label(curCtx, str, alignment)
end

function nk.layout_row(fmt, height, cols, ratio)
	libnk.layout_row(curCtx, fmt, height, cols, ratio)
end

function nk.spacing(cols)
	libnk.spacing(curCtx, cols)
end

function nk.button_image_label(img, label, align)
	-- libnk.button_image_label(curCtx, img, label, align)
end

function nk.clear()
	libnk.clear(curCtx)
end

function nk.convert(cmds, vbuf, ebuf, config)
	libnk.convert(curCtx, cmds, vbuf, ebuf, config)
end

function nk.DrawElements(cmds)
	return function(cmds, cmd)
		if cmd == nil then
			cmd = libnk.draw_list_begin(curCtx, cmds)
		else
			cmd = libnk.draw_list_next(curCtx, cmds, cmd)
		end
		if cmd == NULL then return end
		return cmd
	end, cmds, nil
end

nk.draw_list_begin = libnk.draw_list_begin
nk.draw_list_next = libnk.draw_list_next
nk.UnpackDrawCommand = libnk.UnpackDrawCommand

nk.panel_flag = libnk.panel_flag
nk.text_alignment = libnk.text_alignment

return nk
