local libnk = require("libnuklear")

local nk = {}

function nk.FontConfig()
	return libnk.font_config()
end

function nk.FontAtlas()
	return libnk.font_atlas()
end

function nk.Buffer()
	return libnk.buffer()
end

function nk.Context(font)
	return libnk.context(font)
end

function nk.Image(handle)
	return libnk.image(handle)
end

local curCtx
function nk.setContext(ctx)
	curCtx = ctx
end

function nk.inputBegin()
	libnk.input_begin(curCtx)
end
function nk.inputKey(key, down)
	libnk.input_key(curCtx, key, down)
end
function nk.inputMotion(x, y)
	libnk.input_motion(curCtx, x, y)
end
function nk.inputButton(button, x, y, down)
	libnk.input_button(curCtx, button, x, y, down)
end
function nk.inputEnd()
	libnk.input_end(curCtx)
end

function nk.styleSetFont(font)
	libnk.style_set_font(curCtx, font)
end

function nk.begin(title, x, y, w, h, flags)
	return libnk.begin(curCtx, title, x, y, w, h, flags)
end

function nk.endNk()
	libnk.endnk(curCtx)
end

function nk.layoutRowDynamic(height, cols)
	libnk.layout_row_dynamic(curCtx, height, cols)
end

function nk.layoutRowStatic(height, itemWidth, cols)
	libnk.layout_row_static(curCtx, height, itemWidth, cols)
end

function nk.label(str, alignment)
	libnk.label(curCtx, str, alignment)
end

function nk.layoutRow(fmt, height, cols, ratio)
	libnk.layout_row(curCtx, fmt, height, cols, ratio)
end

function nk.spacing(cols)
	libnk.spacing(curCtx, cols)
end

function nk.buttonLabel(title)
	return libnk.button_label(curCtx, title)
end

function nk.buttonImage(img)
	return libnk.button_image(curCtx, img)
end

function nk.buttonImageLabel(img, label, align)
	return libnk.button_image_label(curCtx, img, label, align)
end

function nk.imageWidget(img)
	libnk.image_widget(curCtx, img)
end

function nk.popupBegin(type, title, flags, rectX, rectY, rectW, rectH)
	return libnk.popup_begin(curCtx, type, title, flags, rectX, rectY, rectW, rectH)
end

function nk.popupClose()
	libnk.popup_close(curCtx)
end

function nk.popupEnd()
	libnk.popup_end(curCtx)
end


function nk.clear()
	libnk.clear(curCtx)
end

function nk.convert(cmds, vbuf, ebuf, config)
	libnk.convert(curCtx, cmds, vbuf, ebuf, config)
end

function nk.drawElements(cmds)
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
