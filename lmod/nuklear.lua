local libnk = require("libnuklear")
local libfont = libnk.font

---@class nuklear
local nk = {}

--[[
** {======================================================
** Font
** =======================================================
--]]

---@class nuklear_font
local font = {}
nk.font = font

---@class nk_font_atlas:userdata
---@field begin fun():void

---@return nk_font_atlas
function font.Atlas()
	return libfont.Atlas()
end
function font.Config()
	return libfont.Config()
end

---@class nk_rune_ptr:lightuserdata

---@return nk_rune_ptr
function font.defaultGlyphRanges()
	return libfont.default_glyph_ranges()
end
---@return nk_rune_ptr
function font.chineseGlyphRanges()
	return libfont.chinese_glyph_ranges()
end
---@return nk_rune_ptr
function font.cyrillicGlyphRanges()
	return libfont.cyrillic_glyph_ranges()
end
---@return nk_rune_ptr
function font.koreanGlyphRanges()
	return libfont.korean_glyph_ranges()
end

-- }======================================================

--[[
** {======================================================
** Buffer
** =======================================================
--]]

function nk.Buffer(size)
	return libnk.Buffer(size)
end

-- }======================================================

--[[
** {======================================================
** Context
** =======================================================
--]]

function nk.Context(font)
	return libnk.Context(font)
end
local curCtx
function nk.setContext(ctx)
	curCtx = ctx
end
function nk.clear()
	libnk.clear(curCtx)
end

-- }======================================================

--[[
** {======================================================
** Drawing
** =======================================================
--]]

function nk.strokeLine(canvas, x0, y0, x1, y1, lineThickness, color)
	libnk.stroke_line(canvas, x0, y0, x1, y1, lineThickness, color)
end
function nk.strokeCurve(canvas, ax, ay, ctrl0x, ctrl0y, ctrl1x, ctrl1y, bx, by, lineThickness, color)
	libnk.stroke_curve(canvas, ax, ay, ctrl0x, ctrl0y, ctrl1x, ctrl1y, bx, by, lineThickness, color)
end
function nk.strokeRect(canvas, rect, rounding, lineThickness, color)
	libnk.stroke_rect(canvas, rect, rounding, lineThickness, color)
end
function nk.strokeCircle(canvas, rect, lineThickness, color)
	libnk.stroke_circle(canvas, rect, lineThickness, color)
end
function nk.strokeRrc(canvas, cx, cy, radius, aMin, aMax, lineThickness, color)
	libnk.stroke_arc(canvas, cx, cy, radius, aMin, aMax, lineThickness, color)
end
function nk.strokeTriangle(canvas, x0, y0, x1, y1, x2, y2, lineThickness, color)
	libnk.stroke_triangle(canvas, x0, y0, x1, y1, x2, y2, lineThickness, color)
end
function nk.strokePolyLine(canvas, pointList, lineThickness, color)
	libnk.stroke_polyline(canvas, pointList, lineThickness, color)
end
function nk.strokePolygon(canvas, pointList, lineThickness, color)
	libnk.stroke_polygon(canvas, pointList, lineThickness, color)
end

function nk.fillRect(canvas, rect, rounding, color)
	libnk.fill_rect(canvas, rect, rounding, color)
end
function nk.fillRectMultiColor(canvas, rect, left, top, right, bottom)
	libnk.fill_rect_multi_color(canvas, rect, left, top, right, bottom)
end
function nk.fillCircle(canvas, rect, color)
	libnk.fill_circle(canvas, rect, color)
end
function nk.fillArc(canvas, cx, cy, radius, aMin, aMax, color)
	libnk.fill_arc(canvas, cx, cy, radius, aMin, aMax, color)
end
function nk.fillTriangle(canvas, x0, y0, x1, y1, x2, y2, color)
	libnk.fill_triangle(canvas, x0, y0, x1, y1, x2, y2, color)
end
function nk.fillPolygon(canvas, pointList, color)
	libnk.fill_polygon(canvas, pointList, color)
end

function nk.drawImage(canvas, rect, img, color)
	libnk.draw_image(canvas, rect, img, color)
end
function nk.drawNineSlice(canvas, rect, nineSlice, color)
	libnk.draw_nine_slice(canvas, rect, nineSlice, color)
end
function nk.drawText(canvas, rect, text, font, bg, fg)
	libnk.draw_text(canvas, rect, text, font, bg, fg)
end
function nk.pushScissor(canvas, rect)
	libnk.push_scissor(canvas, rect)
end
function nk.pushCustom(canvas, rect, callback)
	libnk.push_custom(canvas, rect, callback)
end

-- }======================================================

--[[
** {======================================================
** DrawList
** =======================================================
--]]

function nk.drawListPathClear(drawList)
	libnk.draw_list_path_clear(drawList)
end
function nk.drawListPathLineTo(drawList, pos)
	libnk.draw_list_path_line_to(drawList, pos)
end
function nk.drawListPathArcToFast(drawList, center, radius, aMin, aMax)
	libnk.draw_list_path_arc_to_fast(drawList, center, radius, aMin, aMax)
end
function nk.drawListPathArcTo(drawList, center, radius, aMin, aMax, segments)
	libnk.draw_list_path_arc_to(drawList, center, radius, aMin, aMax, segments)
end
function nk.drawListPathRectTo(drawList, a, b, rounding)
	libnk.draw_list_path_rect_to(drawList, a, b, rounding)
end
function nk.drawListPathCurveTo(drawList, p2, p3, p4, numSegments)
	libnk.draw_list_path_curve_to(drawList, p2, p3, p4, numSegments)
end
function nk.drawListPathFill(drawList, color)
	libnk.draw_list_path_fill(drawList, color)
end
function nk.drawListPathStroke(drawList, color, closed, thickness)
	libnk.draw_list_path_stroke(drawList, color, closed, thickness)
end

function nk.drawListStrokeLine(drawList, a, b, color, thickness)
	libnk.draw_list_stroke_line(drawList, a, b, color, thickness)
end
function nk.drawListStrokeRect(drawList, rect, color, rounding, thickness)
	libnk.draw_list_stroke_rect(drawList, rect, color, rounding, thickness)
end
function nk.drawListStrokeTriangle(drawList, a, b, c, color, thickness)
	libnk.draw_list_stroke_triangle(drawList, a, b, c, color, thickness)
end
function nk.drawListStrokeCircle(drawList, center, radius, color, segs, thickness)
	libnk.draw_list_stroke_circle(drawList, center, radius, color, segs, thickness)
end
function nk.drawListStrokeCurve(drawList, p0, cp0, cp1, p1, color, segments, thickness)
	libnk.draw_list_stroke_curve(drawList, p0, cp0, cp1, p1, color, segments, thickness)
end
function nk.drawListStrokePolyLine(drawList, pointList, color, closed, thickness, aliasing)
	libnk.draw_list_stroke_poly_line(drawList, pointList, color, closed, thickness, aliasing)
end

function nk.drawListFillRect(drawList, rect, c, rounding)
	libnk.draw_list_fill_rect(drawList, rect, c, rounding)
end
function nk.drawListFillRectMultiColor(drawList, rect, left, top, right, bottom)
	libnk.draw_list_fill_rect_multi_color(drawList, rect, left, top, right, bottom)
end
function nk.drawListFillTriangle(drawList, a, b, c, col)
	libnk.draw_list_fill_triangle(drawList, a, b, c, col)
end
function nk.drawListFillCircle(drawList, center, radius, col, segs)
	libnk.draw_list_fill_circle(drawList, center, radius, col, segs)
end
function nk.drawListFillPolyConvex(drawList, pointList, color, aliasing)
	libnk.draw_list_fill_poly_convex(drawList, pointList, color, aliasing)
end

function nk.drawListAddImage(drawList, image, rect, color)
	libnk.draw_list_add_image(drawList, image, rect, color)
end
function nk.drawListAddText(drawList, font, rect, text, fontHeight, color)
	libnk.draw_list_add_text(drawList, font, rect, text, fontHeight, color)
end

-- }======================================================

--[[
** {======================================================
** Group
** =======================================================
--]]

function nk.groupBegin(title, flags)
	return libnk.groupBegin(curCtx, title, flags)
end
function nk.groupBeginTitled(name, title, flags)
	return libnk.group_begin_titled(curCtx, name, title, flags)
end
function nk.groupEnd()
	libnk.group_end(curCtx)
end
function nk.groupScrolledOffsetBegin(title, flags)
	return libnk.group_scrolled_offset_begin(curCtx, title, flags)
end
function nk.groupScrolledBegin(title, flags)
	return libnk.group_scrolled_begin(curCtx, title, flags)
end
function nk.groupScrolledEnd()
	libnk.group_scrolled_end(curCtx)
end
function nk.groupGetScroll(name)
	return libnk.group_get_scroll(curCtx, name)
end
function nk.groupSetScroll(name, offsetX, offsetY)
	libnk.group_set_scroll(curCtx, name, offsetX, offsetY)
end

-- }======================================================

--[[
** {======================================================
** Image
** =======================================================
--]]

function nk.Image(handle, w, h, subRegion)
	return libnk.Image(handle, w, h, subRegion)
end
function nk.NineSlice(handle, w, h, subRegion, left, top, right, bottom)
	return libnk.NineSlice(handle, w, h, subRegion, left, top, right, bottom)
end

-- }======================================================

--[[
** {======================================================
** Input
** =======================================================
--]]

function nk.inputBegin()
	libnk.input_begin(curCtx)
end
function nk.inputMotion(x, y)
	libnk.input_motion(curCtx, x, y)
end
function nk.inputKey(key, down)
	libnk.input_key(curCtx, key, down)
end
function nk.inputButton(button, x, y, down)
	libnk.input_button(curCtx, button, x, y, down)
end
function nk.inputScroll(vec2)
	libnk.input_scroll(curCtx, vec2)
end
function nk.inputChar(c)
	libnk.input_scroll(curCtx, c)
end
function nk.inputUnicode(codepoint)
	libnk.input_scroll(curCtx, codepoint)
end
function nk.inputEnd()
	libnk.input_end(curCtx)
end

function nk.inputHasMouseClick(mouseButton)
	return libnk.input_has_mouse_click(curCtx, mouseButton)
end
function nk.inputHasMouseClickInRect(mouseButton, rect)
	return libnk.input_has_mouse_click_in_rect(curCtx, mouseButton, rect)
end
function nk.inputHasMouseClickDownInRect(mouseButton, rect, down)
	return libnk.input_has_mouse_click_down_in_rect(curCtx, mouseButton, rect, down)
end

function nk.inputIsMouseClickInRect(mouseButton, rect)
	return libnk.input_is_mouse_click_in_rect(curCtx, mouseButton, rect)
end
function nk.inputIsMouseClickDownInRect(mouseButton, rect, down)
	return libnk.input_is_mouse_click_down_in_rect(curCtx, mouseButton, rect, down)
end

function nk.inputAnyMouseClickInRect(rect)
	return libnk.input_any_mouse_click_in_rect(curCtx, rect)
end
function nk.inputIsMousePrevHoveringRect(rect)
	return libnk.input_is_mouse_prev_hovering_rect(curCtx, rect)
end
function nk.inputIsMouseHoveringRect(bounds)
	return libnk.input_is_mouse_hovering_rect(curCtx, bounds)
end

function nk.inputMouseClicked(mouseButton, rect)
	return libnk.input_mouse_clicked(curCtx, mouseButton, rect)
end
function nk.inputIsMouseDown(mouseButton)
	return libnk.input_is_mouse_down(curCtx, mouseButton)
end
function nk.inputIsMousePressed(mouseButton)
	return libnk.input_is_mouse_pressed(curCtx, mouseButton)
end
function nk.inputIsMouseReleased(mouseButton)
	return libnk.input_is_mouse_released(curCtx, mouseButton)
end
function nk.inputIsKeyPressed(key)
	return libnk.input_is_key_pressed(curCtx, key)
end
function nk.inputIsKeyReleased(key)
	return libnk.input_is_key_released(curCtx, key)
end
function nk.inputIsKeyDown(key)
	return libnk.input_is_key_down(curCtx, key)
end

-- }======================================================

--[[
** {======================================================
** Layout
** =======================================================
--]]

function nk.layoutSetMinRowHeight(height)
	libnk.layout_set_min_row_height(curCtx, height)
end
function nk.layoutResetMinRowHeight()
	libnk.layout_reset_min_row_height(curCtx)
end
function nk.layoutWidgetBounds()
	return libnk.layout_widget_bounds(curCtx)
end
function nk.layoutRatioFromPixel(pixelWidth)
	return libnk.layout_ratio_from_pixel(curCtx, pixelWidth)
end
function nk.layoutRowDynamic(height, cols)
	libnk.layout_row_dynamic(curCtx, height, cols)
end
function nk.layoutRowStatic(height, itemWidth, cols)
	libnk.layout_row_static(curCtx, height, itemWidth, cols)
end
function nk.layoutRowBegin(fmt, rowHeight, cols)
	libnk.layout_row_begin(curCtx, fmt, rowHeight, cols)
end
function nk.layoutRowPush(value)
	libnk.layout_row_push(curCtx, value)
end
function nk.layoutRowEnd()
	libnk.layout_row_end(curCtx)
end
function nk.layoutRow(fmt, height, cols, ratioList)
	libnk.layout_row(curCtx, fmt, height, cols, ratioList)
end
function nk.layoutRowTemplateBegin(rowHeight)
	libnk.layout_row_template_begin(curCtx, rowHeight)
end
function nk.layoutRowTemplatePushDynamic()
	libnk.layout_row_template_push_dynamic(curCtx)
end
function nk.layoutRowTemplatePushVariable(minWidth)
	libnk.layout_row_template_push_variable(curCtx, minWidth)
end
function nk.layoutRowTemplatePushStatic(width)
	libnk.layout_row_template_push_static(curCtx, width)
end
function nk.layoutRowTemplateEnd()
	libnk.layout_row_template_end(curCtx)
end
function nk.layoutSpaceBegin(fmt, height, widgetCount)
	libnk.layout_space_begin(curCtx, fmt, height, widgetCount)
end
function nk.layoutSpacePush(bounds)
	libnk.layout_space_push(curCtx, bounds)
end
function nk.layoutSpaceEnd()
	libnk.layout_space_end(curCtx)
end
function nk.layoutSpaceBounds()
	return libnk.layout_space_bounds(curCtx)
end
function nk.layoutSpaceToScreen(vec2)
	return libnk.layout_space_to_screen(curCtx, vec2)
end
function nk.layoutSpaceToLocal(vec2)
	return libnk.layout_space_to_local(curCtx, vec2)
end
function nk.layoutSpaceRectToScreen(rect)
	return libnk.layout_space_rect_to_screen(curCtx, rect)
end
function nk.layoutSpaceRectToLocal(rect)
	return libnk.layout_space_rect_to_local(curCtx, rect)
end
function nk.spacer()
	libnk.spacer(curCtx)
end

-- }======================================================

--[[
** {======================================================
** Vertex
** =======================================================
--]]

function nk.convert(cmds, vbuf, ebuf, config)
	libnk.convert(curCtx, cmds, vbuf, ebuf, config)
end
function nk.drawForEach(cmds, screenWidth, screenHeight, pixelWidth, pixelHeight, callback)
	libnk.drawForEach(curCtx, cmds, screenWidth, screenHeight, pixelWidth, pixelHeight, callback)
end

-- }======================================================

--[[
** {======================================================
** ListView
** =======================================================
--]]

function nk.listViewBegin(idStr, flag, rowHeight, rowCount)
	return libnk.list_view_begin(curCtx, idStr, flag, rowHeight, rowCount)
end
function nk.listViewEnd()
	libnk.list_view_end()
end

-- }======================================================

--[[
** {======================================================
** Text
** =======================================================
--]]

function nk.textWidget(str, alignment)
	libnk.text_widget(curCtx, str, alignment)
end
function nk.textColored(str, alignment, color)
	libnk.text_colored(curCtx, str, alignment, color)
end
function nk.textWrap(str)
	libnk.text_wrap(curCtx, str)
end
function nk.textWrapColored(str, color)
	libnk.text_wrap_colored(curCtx, str, color)
end
function nk.imageWidget(image)
	libnk.image_widget(curCtx, image)
end
function nk.imageColor(image, color)
	libnk.image_color(curCtx, image, color)
end
function nk.valueBool(prefix, b)
	libnk.value_bool(curCtx, prefix, b)
end
function nk.valueInt(prefix, value)
	libnk.value_int(curCtx, prefix, value)
end
function nk.valueUint(prefix, value)
	libnk.value_uint(curCtx, prefix, value)
end
function nk.valueFloat(prefix, value)
	libnk.value_float(curCtx, prefix, value)
end
function nk.valueColorByte(prefix, color)
	libnk.value_color_byte(curCtx, prefix, color)
end
function nk.valueColorFloat(prefix, color)
	libnk.value_color_float(curCtx, prefix, color)
end
function nk.valueColorHex(prefix, color)
	libnk.value_color_hex(curCtx, prefix, color)
end

---@class nk_text_align
---@field public LEFT number @
---@field public CENTERED number
---@field public RIGHT number
---@field public TOP number
---@field public MIDDLE number
---@field public BOTTOM number
local nk_text_align
---@type nk_text_align
nk.text_align = libnk.text_align

---@class nk_text_alignment
---@field public LEFT number
---@field public CENTERED number
---@field public RIGHT number
local nk_text_alignment
---@type nk_text_alignment
nk.text_alignment = libnk.text_alignment

-- }======================================================

--[[
** {======================================================
** Button
** =======================================================
--]]

function nk.buttonText(title)
	return libnk.button_text(curCtx, title)
end
function nk.buttonColor(color)
	return libnk.button_color(curCtx, color)
end
function nk.buttonSymbol(symbol)
	return libnk.button_symbol(curCtx, symbol)
end
function nk.buttonImage(img)
	return libnk.button_image(curCtx, img)
end
function nk.buttonSymbolText(symbol, str, textAlignment)
	return libnk.button_symbol_text(curCtx, symbol, str, textAlignment)
end
function nk.buttonImageText(img, str, align)
	return libnk.button_image_text(curCtx, img, str, align)
end
function nk.buttonTextStyled(styleButton, title)
	return libnk.button_text_styled(curCtx, styleButton, title)
end
function nk.buttonSymbolStyled(styleButton, symbol)
	return libnk.button_symbol_styled(curCtx, styleButton, symbol)
end
function nk.buttonImageStyled(styleButton, image)
	return libnk.button_image_styled(curCtx, styleButton, image)
end
function nk.buttonSymbolTextStyled(styleButton, symbol, str, textAlignment)
	return libnk.button_symbol_text_styled(curCtx, styleButton, symbol, str, textAlignment)
end
function nk.buttonImageTextStyled(styleButton, img, str, align)
	return libnk.button_image_text_styled(curCtx, styleButton, img, str, align)
end
function nk.buttonSetBehavior(behavior)
	libnk.button_set_behavior(curCtx, behavior)
end
function nk.buttonPushBehavior(behavior)
	return libnk.button_push_behavior(curCtx, behavior)
end
function nk.buttonPopBehavior()
	return libnk.button_pop_behavior(curCtx)
end

nk.symbol_type = libnk.symbol_type
nk.button_behavior = libnk.button_behavior

-- }======================================================

--[[
** {======================================================
** CheckBox
** =======================================================
--]]

function nk.checkText(text, active)
	return libnk.check_text(curCtx, text, active)
end
function nk.checkFlagsText(text, flags, value)
	return libnk.check_flags_text(curCtx, text, flags, value)
end

-- }======================================================

--[[
** {======================================================
** RadioButton
** =======================================================
--]]

function nk.optionText(text, active)
	return libnk.option_text(curCtx, text, active)
end

-- }======================================================

--[[
** {======================================================
** Selectable
** =======================================================
--]]

function nk.selectText(text, align, value)
	return libnk.select_text(curCtx, text, align, value)
end
function nk.selectImageText(img, text, align, value)
	return libnk.select_image_text(curCtx, img, text, align, value)
end
function nk.selectSymbolText(symbol, text, align, value)
	return libnk.select_symbol_text(curCtx, symbol, text, align, value)
end


-- }======================================================

--[[
** {======================================================
** Slider
** =======================================================
--]]

function nk.slideFloat(min, val, max, step)
	return libnk.slide_float(curCtx, min, val, max, step)
end
function nk.slideInt(min, val, max, step)
	return libnk.slide_int(curCtx, min, val, max, step)
end

-- }======================================================

--[[
** {======================================================
** ProgressBar
** =======================================================
--]]

function nk.prog(cur, max, modifyable)
	return libnk.prog(curCtx, cur, max, modifyable)
end

-- }======================================================

--[[
** {======================================================
** ColorPicker
** =======================================================
--]]

function nk.colorPicker(color, format)
	return libnk.color_picker(curCtx, color, format)
end

nk.color_format = libnk.color_format

-- }======================================================

--[[
** {======================================================
** Properties
** =======================================================
--]]

function nk.propertyi(name, min, val, max, step, incPerPixel)
	return libnk.propertyi(curCtx, name, min, val, max, step, incPerPixel)
end
function nk.propertyd(name, min, val, max, step, incPerPixel)
	return libnk.propertyd(curCtx, name, min, val, max, step, incPerPixel)
end

-- }======================================================

--[[
** {======================================================
** TextEdit
** =======================================================
--]]

function nk.editString(flag, buffer)
	return libnk.edit_string(curCtx, flag, buffer)
end
function nk.editBuffer()
	return libnk.edit_buffer(curCtx)
end
function nk.editFocus(flag)
	libnk.edit_focus(curCtx, flag)
end
function nk.editUnfocus(flag)
	libnk.edit_unfocus(curCtx)
end

nk.edit_flag = libnk.edit_flag
nk.edit_type = libnk.edit_type
nk.edit_event = libnk.edit_event

-- }======================================================

--[[
** {======================================================
** Chart
** =======================================================
--]]

function nk.chartBegin(type, num, min, max)
	return libnk.chart_begin(curCtx, type, num, min, max)
end
function nk.chartBeginColored(type, color, highLightColor, num, min, max)
	return libnk.chart_begin_colored(curCtx, type, color, highLightColor, num, min, max)
end
function nk.chartAddSlot(type, count, minValue, maxValue)
	libnk.chart_add_slot(curCtx, type, count, minValue, maxValue)
end
function nk.chartAddSlotColored(type, color, highLightColor, count, minValue, maxValue)
	libnk.chart_add_slot_colored(curCtx, type, color, highLightColor, count, minValue, maxValue)
end
function nk.chartPush(value)
	return libnk.chart_push(curCtx, value)
end
function nk.chartPushSlot(value, slot)
	return libnk.chart_push_slot(curCtx, value, slot)
end
function nk.chartEnd()
	libnk.chart_end(curCtx)
end
function nk.plot(type, valueList, offset)
	libnk.plot(curCtx, type, valueList, offset)
end
function nk.plotFunction(type, valueGetter, count, offset)
	libnk.plot_function(curCtx, type, valueGetter, count, offset)
end

nk.chart_type = libnk.chart_type

-- }======================================================

--[[
** {======================================================
** Popup
** =======================================================
--]]

function nk.popupBegin(type, title, flags, rectX, rectY, rectW, rectH)
	return libnk.popup_begin(curCtx, type, title, flags, rectX, rectY, rectW, rectH)
end
function nk.popupClose()
	libnk.popup_close(curCtx)
end
function nk.popupEnd()
	libnk.popup_end(curCtx)
end
function nk.popupGetScroll()
	return libnk.popup_get_scroll(curCtx)
end
function nk.popupSetScroll(offsetX, offsetY)
	libnk.popup_set_scroll(curCtx, offsetX, offsetY)
end

nk.popup_type = libnk.popup_type

-- }======================================================

--[[
** {======================================================
** ComboBox
** =======================================================
--]]

function nk.combo(itemList, selected, itemHeight, sizeVec2)
	return libnk.combo(curCtx, itemList, selected, itemHeight, sizeVec2)
end
function nk.comboSeparator(itemStr, separator, selected, count, itemHeight, sizeVec2)
	return libnk.combo_separator(curCtx, itemStr, separator, selected, count, itemHeight, sizeVec2)
end
function nk.comboCallback(itemGetter, selected, count, itemHeight, sizeVec2)
	return libnk.combo_callback(curCtx, itemGetter, selected, count, itemHeight, sizeVec2)
end

-- }======================================================

--[[
** {======================================================
** AbstractComboBox
** =======================================================
--]]

function nk.comboBeginText(selectedStr, sizeVec2)
	return libnk.combo_begin_text(curCtx, selectedStr, sizeVec2)
end
function nk.comboBeginColor(color, sizeVec2)
	return libnk.combo_begin_color(curCtx, color, sizeVec2)
end
function nk.comboBeginSymbol(symbol, sizeVec2)
	return libnk.combo_begin_symbol(curCtx, symbol, sizeVec2)
end
function nk.comboBeginSymbolText(selectedStr, symbol, sizeVec2)
	return libnk.combo_begin_symbol_text(curCtx, selectedStr, symbol, sizeVec2)
end
function nk.comboBeginImage(img, sizeVec2)
	return libnk.combo_begin_image(curCtx, img, sizeVec2)
end
function nk.comboBeginImageText(selectedStr, img, sizeVec2)
	return libnk.combo_begin_image_text(curCtx, selectedStr, img, sizeVec2)
end
function nk.comboItemText(text, alignment)
	return libnk.combo_item_text(curCtx, text, alignment)
end
function nk.comboItemImageText(img, text, alignment)
	return libnk.combo_item_image_text(curCtx, img, text, alignment)
end
function nk.comboItemSymbolText(symbol, text, alignment)
	return libnk.combo_item_symbol_text(curCtx, symbol, text, alignment)
end
function nk.combo_close()
	libnk.combo_close(curCtx)
end
function nk.combo_end()
	libnk.combo_end(curCtx)
end

-- }======================================================

--[[
** {======================================================
** Contextual
** =======================================================
--]]


function nk.contextualBegin(flags, size, triggleBounds)
	return libnk.contextual_begin(curCtx, flags, size, triggleBounds)
end
function nk.contextualItemText(text, alignment)
	return libnk.contextual_item_text(curCtx, text, alignment)
end
function nk.contextualItemImageText(img, text, alignment)
	return libnk.contextual_item_image_text(curCtx, img, text, alignment)
end
function nk.contextualItemSymbolText(symbol, text, alignment)
	return libnk.contextual_item_symbol_text(curCtx, symbol, text, alignment)
end
function nk.contextualClose()
	libnk.contextual_close(curCtx)
end
function nk.contextualEnd()
	libnk.contextual_end(curCtx)
end

-- }======================================================

--[[
** {======================================================
** ToolTip
** =======================================================
--]]

function nk.tooltip(str)
	libnk.tooltip(curCtx, str)
end
function nk.tooltipBegin(width)
	return libnk.tooltip_begin(curCtx, width)
end
function nk.tooltipEnd()
	libnk.tooltip_end(curCtx)
end

-- }======================================================

--[[
** {======================================================
** Menu
** =======================================================
--]]

function nk.menubarBegin()
	libnk.menubar_begin(curCtx)
end
function nk.menubarEnd()
	libnk.menubar_end(curCtx)
end
function nk.menuBeginText(title, align, sizeVec2)
	return libnk.menu_begin_text(curCtx, title, align, sizeVec2)
end
function nk.menuBeginImage(idStr, img, sizeVec2)
	return libnk.menu_begin_image(curCtx, idStr, img, sizeVec2)
end
function nk.menuBeginImageText(title, align, img, sizeVec2)
	return libnk.menu_begin_image_text(curCtx, title, align, img, sizeVec2)
end
function nk.menuBeginSymbol(idStr, symbol, sizeVec2)
	return libnk.menu_begin_symbol(curCtx, idStr, symbol, sizeVec2)
end
function nk.menuBeginSymbolText(title, align, symbol, sizeVec2)
	return libnk.menu_begin_symbol_text(curCtx, title, align, symbol, sizeVec2)
end
function nk.menuItemText(title, align)
	return libnk.menu_item_text(curCtx, title, align)
end
function nk.menuItemImageText(img, title, align)
	return libnk.menu_item_image_text(curCtx, img, title, align)
end
function nk.menuItemSymbolText(symbol, title, align)
	return libnk.menu_item_symbol_text(curCtx, symbol, title, align)
end
function nk.menuClose()
	libnk.menu_close(curCtx)
end
function nk.menuEnd()
	libnk.menu_end(curCtx)
end

-- }======================================================

--[[
** {======================================================
** Style
** =======================================================
--]]

function nk.styleDefault()
	libnk.style_default(curCtx)
end
function nk.styleFromTable(table)
	libnk.style_from_table(curCtx, table)
end


function nk.styleGetColorName(styleColor)
	return libnk.style_get_color_name(curCtx, styleColor)
end
function nk.styleSetFont(font)
	libnk.style_set_font(curCtx, font)
end
function nk.styleSetCursor(styleCursor)
	return libnk.style_set_cursor(curCtx, styleCursor)
end
function nk.styleShowCursor()
	return libnk.style_show_cursor(curCtx)
end
function nk.styleHideCursor()
	return libnk.style_hide_cursor(curCtx)
end
function nk.stylePushFont(font)
	return libnk.style_push_font(curCtx, font)
end


function nk.stylePopFont()
	return libnk.style_pop_font(curCtx)
end
function nk.stylePopFloat()
	return libnk.style_pop_float(curCtx)
end
function nk.stylePopVec2()
	return libnk.style_pop_vec2(curCtx)
end
function nk.stylePopStyleItem()
	return libnk.style_pop_style_item(curCtx)
end
function nk.stylePopFlags()
	return libnk.style_pop_flags(curCtx)
end
function nk.stylePopColor()
	return libnk.style_pop_color(curCtx)
end

-- }======================================================

--[[
** {======================================================
** StyleItem
** =======================================================
--]]



-- }======================================================

--[[
** {======================================================
** StyleButton
** =======================================================
--]]

function nk.StyleButton()
	return libnk.StyleButton(curCtx)
end

-- }======================================================

--[[
** {======================================================
** TextEditor
** =======================================================
--]]

function nk.TextEditor()
	return libnk.TextEditor(curCtx)
end

-- }======================================================

--[[
** {======================================================
** Tree
** =======================================================
--]]

function nk.treePushHashed(type, title, initialState, hashStr, seed)
	return libnk.tree_push_hashed(curCtx, type, title, initialState, hashStr, seed)
end
function nk.treeImagePushHashed(type, img, title, initialState, hashStr, seed)
	return libnk.tree_image_push_hashed(curCtx, type, img, title, initialState, hashStr, seed)
end
function nk.treePop()
	libnk.tree_pop(curCtx)
end
function nk.treeStatePush(type, title, state)
	return libnk.tree_state_push(curCtx, type, title, state)
end
function nk.treeStateImagePush(type, img, title, state)
	return libnk.tree_state_image_push(curCtx, type, img, title, state)
end
function nk.treeStatePop()
	libnk.tree_state_pop(curCtx)
end
function nk.treeElementPushHashed(type, title, initialState, selected, hashStr, seed)
	return libnk.tree_element_push_hashed(curCtx, type, title, initialState, selected, hashStr, seed)
end
function nk.treeElementImagePushHashed(type, img, title, initialState, selected, hashStr, seed)
	return libnk.tree_element_image_push_hashed(curCtx, type, img, title, initialState, selected, hashStr, seed)
end
function nk.treeElementPop()
	libnk.tree_element_pop(curCtx)
end

-- }======================================================

--[[
** {======================================================
** Utils
** =======================================================
--]]

function nk.rgbaToColor(r, g, b, a)
	return libnk.rgbaToColor(r, g, b, a)
end
function nk.colorToRgba(color)
	return libnk.colorToRgba(color)
end
function nk.colorToRgbaf(color)
	return libnk.colorToRgbaf(color)
end
function nk.hsvaToColor(h, s, v, a)
	return libnk.hsvaToColor(h, s, v, a)
end
function nk.colorToHsva(color)
	return libnk.colorToHsva(color)
end
function nk.colorToHsvaf(color)
	return libnk.colorToHsvaf(color)
end
function nk.rgbHexToColor(rgbHexStr)
	return libnk.rgbHexToColor(rgbHexStr)
end
function nk.colorToRgbHex(color)
	return libnk.colorToRgbHex(color)
end
function nk.rgbaHexToColor(rgbHexStr)
	return libnk.rgbaHexToColor(rgbHexStr)
end
function nk.colorToRgbaHex(color)
	return libnk.colorToRgbaHex(color)
end

function nk.murmurHash(keyStr, seed)
	return libnk.murmur_hash(keyStr, seed)
end
function nk.triangleFromDirection(rect, padX, padY, direction)
	return libnk.triangle_from_direction(rect, padX, padY, direction)
end
function nk.getNullRect()
	return libnk.get_null_rect()
end

nk.heading = libnk.heading

function nk.packVec2(x, y)
	return libnk.packVec2(x, y)
end
function nk.unpackVec2(vec2)
	return libnk.unpackVec2(vec2)
end

function nk.packRect(x, y, w, h)
	return libnk.packRect(x, y, w, h)
end
function nk.unpackRect(rect)
	return libnk.unpackRect(rect)
end

-- }======================================================

--[[
** {======================================================
** Widget
** =======================================================
--]]

function nk.widget()
	return libnk.widget(curCtx)
end
function nk.widgetFitting(itemPadding)
	return libnk.widget_fitting(curCtx, itemPadding)
end
function nk.widgetBounds()
	return libnk.widget_bounds(curCtx)
end
function nk.widgetPosition()
	return libnk.widget_position(curCtx)
end
function nk.widgetSize()
	return libnk.widget_size(curCtx)
end
function nk.widgetWidth()
	return libnk.widget_width(curCtx)
end
function nk.widgetHeight()
	return libnk.widget_height(curCtx)
end
function nk.widgetIsHovered()
	return libnk.widget_is_hovered(curCtx)
end
function nk.widgetIsMouseClicked(button)
	return libnk.widget_is_mouse_clicked(curCtx, button)
end
function nk.widgetHasMouseClickDown(button, down)
	return libnk.widget_has_mouse_click_down(curCtx, button, down)
end
function nk.spacing(cols)
	libnk.spacing(curCtx, cols)
end

nk.widget_layout_state = libnk.widget_layout_state

-- }======================================================

--[[
** {======================================================
** Window
** =======================================================
--]]

function nk.begin(title, bounds, flags)
	return libnk.begin(curCtx, title, bounds, flags)
end
function nk.beginTitled(name, title, bounds, flags)
	return libnk.begin_titled(curCtx, name, title, bounds, flags)
end
function nk.endWindow()
	libnk.end_window(curCtx)
end
function nk.windowGetBounds()
	return libnk.window_get_bounds(curCtx)
end
function nk.windowGetPosition()
	return libnk.window_get_position(curCtx)
end
function nk.windowGetSize()
	return libnk.window_get_size(curCtx)
end
function nk.windowGetWidth()
	return libnk.window_get_width(curCtx)
end
function nk.windowGetHeight()
	return libnk.window_get_height(curCtx)
end
function nk.windowGetContentRegion()
	return libnk.window_get_content_region(curCtx)
end
function nk.windowGetContentRegionMin()
	return libnk.window_get_content_region_min(curCtx)
end
function nk.windowGetContentRegionMax()
	return libnk.window_get_content_region_max(curCtx)
end
function nk.windowGetContentRegionSize()
	return libnk.window_get_content_region_size(curCtx)
end
function nk.windowGetCanvas()
	return libnk.window_get_canvas(curCtx)
end
function nk.windowGetScroll()
	return libnk.window_get_scroll(curCtx)
end
function nk.windowHasFocus()
	return libnk.window_has_focus(curCtx)
end
function nk.windowIsHovered()
	return libnk.window_is_hovered(curCtx)
end
function nk.windowIsCollapsed(name)
	return libnk.window_is_collapsed(curCtx, name)
end
function nk.windowIsClosed(name)
	return libnk.window_is_closed(curCtx, name)
end
function nk.windowIsHidden(name)
	return libnk.window_is_hidden(curCtx, name)
end
function nk.windowIsActive(name)
	return libnk.window_is_active(curCtx, name)
end
function nk.windowIsAnyHovered()
	return libnk.window_is_any_hovered(curCtx)
end
function nk.itemIsAnyActive()
	return libnk.item_is_any_active(curCtx)
end
function nk.windowSetBounds(name, bounds)
	libnk.window_set_bounds(curCtx, name, bounds)
end
function nk.windowSetPosition(name, pos)
	libnk.window_set_position(curCtx, name, pos)
end
function nk.windowSetSize(name, size)
	libnk.window_set_size(curCtx, name, size)
end
function nk.windowSetFocus(name)
	libnk.window_set_focus(curCtx, name)
end
function nk.windowSetScroll(offsetX, offsetY)
	libnk.window_set_scroll(curCtx, offsetX, offsetY)
end
function nk.windowClose(name)
	libnk.window_close(curCtx, name)
end
function nk.windowCollapse(name, state)
	libnk.window_collapse(curCtx, name, state)
end
function nk.windowCollapseIf(name, state, cond)
	libnk.window_collapse_if(curCtx, name, state, cond)
end
function nk.windowShow(name, state)
	libnk.window_show(curCtx, name, state)
end
function nk.windowShowIf(name, state, cond)
	libnk.window_show_if(curCtx, name, state, cond)
end

nk.panel_flag = libnk.panel_flag

-- }======================================================

return nk
