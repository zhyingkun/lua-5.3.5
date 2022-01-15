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

---@class nk_font:lightuserdata

---@class nk_draw_null_texture:userdata

---@class nk_font_atlas:userdata
---@field begin fun(self:nk_font_atlas):void
---@field add fun(self:nk_font_atlas, mem:string, height:number, cfg:nk_font_config):nk_font
---@field addDefault fun(self:nk_font_atlas, height:number, cfg:nk_font_config):nk_font
---@field bake fun(self:nk_font_atlas, format:nuklear_atlas_format):lightuserdata, lightuserdata, lightuserdata, integer, integer
---@field endAtlas fun(self:nk_font_atlas, handleId:integer):nk_draw_null_texture
---@field cleanup fun(self:nk_font_atlas):void

---@return nk_font_atlas
function font.Atlas()
	return libfont.Atlas()
end

---@class nuklear_atlas_format
---@field public ALPHA8 integer
---@field public RGBA32 integer

---@type nuklear_atlas_format
font.atlas_format = libfont.atlas_format

---@class nk_font_config

---@class nk_font_config_table
---@field public mergeMode boolean
---@field public pixelSnap boolean
---@field public overSampleV integer
---@field public overSampleH integer
---@field public pixelHeight number
---@field public coordType nuklear_coord_type
---@field public spacing NuklearVec2
---@field public range nk_rune_ptr
---@field public fallbackGlyph integer

---@param cfgTable nk_font_config_table
---@return nk_font_config
function font.Config(cfgTable)
	return libfont.Config(cfgTable)
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

---@class nk_buffer
---@field public frontBufferPtr fun(self:nk_buffer):lightuserdata
---@field public frontBufferAllocated fun(self:nk_buffer):integer
---@field public getTotal fun(self:nk_buffer):integer
---@field public clear fun(self:nk_buffer):void

---@overload fun():nk_buffer
---@overload fun(size:integer):nk_buffer
---@param size integer
function nk.Buffer(size)
	return libnk.Buffer(size)
end

-- }======================================================

--[[
** {======================================================
** Context
** =======================================================
--]]

---@class nk_context

---@param font nk_font
---@return nk_context
function nk.Context(font)
	return libnk.Context(font)
end
---@type nk_context
local curCtx
---@param ctx nk_context
function nk.setContext(ctx)
	curCtx = ctx
end
function nk.clear()
	libnk.clear(curCtx)
end

-- }======================================================

--[[
** {======================================================
** ShapeOutlines
** =======================================================
--]]

---@param canvas nk_command_buffer
---@param x0 number
---@param y0 number
---@param x1 number
---@param y1 number
---@param lineThickness number
---@param color NuklearColor
function nk.strokeLine(canvas, x0, y0, x1, y1, lineThickness, color)
	libnk.stroke_line(canvas, x0, y0, x1, y1, lineThickness, color)
end
---@param canvas nk_command_buffer
---@param ax number
---@param ay number
---@param ctrl0x number
---@param ctrl0y number
---@param ctrl1x number
---@param ctrl1y number
---@param bx number
---@param by number
---@param lineThickness number
---@param color NuklearColor
function nk.strokeCurve(canvas, ax, ay, ctrl0x, ctrl0y, ctrl1x, ctrl1y, bx, by, lineThickness, color)
	libnk.stroke_curve(canvas, ax, ay, ctrl0x, ctrl0y, ctrl1x, ctrl1y, bx, by, lineThickness, color)
end
---@param canvas nk_command_buffer
---@param rect NuklearRect
---@param rounding number
---@param lineThickness number
---@param color NuklearColor
function nk.strokeRect(canvas, rect, rounding, lineThickness, color)
	libnk.stroke_rect(canvas, rect, rounding, lineThickness, color)
end
---@param canvas nk_command_buffer
---@param rect NuklearRect
---@param lineThickness number
---@param color NuklearColor
function nk.strokeCircle(canvas, rect, lineThickness, color)
	libnk.stroke_circle(canvas, rect, lineThickness, color)
end
---@param canvas nk_command_buffer
---@param cx number
---@param cy number
---@param radius number
---@param aMin number
---@param aMax number
---@param lineThickness number
---@param color NuklearColor
function nk.strokeRrc(canvas, cx, cy, radius, aMin, aMax, lineThickness, color)
	libnk.stroke_arc(canvas, cx, cy, radius, aMin, aMax, lineThickness, color)
end
---@param canvas nk_command_buffer
---@param x0 number
---@param y0 number
---@param x1 number
---@param y1 number
---@param x2 number
---@param y2 number
---@param lineThickness number
---@param color NuklearColor
function nk.strokeTriangle(canvas, x0, y0, x1, y1, x2, y2, lineThickness, color)
	libnk.stroke_triangle(canvas, x0, y0, x1, y1, x2, y2, lineThickness, color)
end
---@param canvas nk_command_buffer
---@param pointList number[]
---@param lineThickness number
---@param color NuklearColor
function nk.strokePolyLine(canvas, pointList, lineThickness, color)
	libnk.stroke_polyline(canvas, pointList, lineThickness, color)
end
---@param canvas nk_command_buffer
---@param pointList number[]
---@param lineThickness number
---@param color NuklearColor
function nk.strokePolygon(canvas, pointList, lineThickness, color)
	libnk.stroke_polygon(canvas, pointList, lineThickness, color)
end

-- }======================================================

--[[
** {======================================================
** FilledShapes
** =======================================================
--]]

---@param canvas nk_command_buffer
---@param rect NuklearRect
---@param rounding number
---@param color NuklearColor
function nk.fillRect(canvas, rect, rounding, color)
	libnk.fill_rect(canvas, rect, rounding, color)
end
---@param canvas nk_command_buffer
---@param rect NuklearRect
---@param left NuklearColor
---@param top NuklearColor
---@param right NuklearColor
---@param bottom NuklearColor
function nk.fillRectMultiColor(canvas, rect, left, top, right, bottom)
	libnk.fill_rect_multi_color(canvas, rect, left, top, right, bottom)
end
---@param canvas nk_command_buffer
---@param rect NuklearRect
---@param color NuklearColor
function nk.fillCircle(canvas, rect, color)
	libnk.fill_circle(canvas, rect, color)
end
---@param canvas nk_command_buffer
---@param cx number
---@param cy number
---@param radius number
---@param aMin number
---@param aMax number
---@param color NuklearColor
function nk.fillArc(canvas, cx, cy, radius, aMin, aMax, color)
	libnk.fill_arc(canvas, cx, cy, radius, aMin, aMax, color)
end
---@param canvas nk_command_buffer
---@param x0 number
---@param y0 number
---@param x1 number
---@param y1 number
---@param x2 number
---@param y2 number
---@param color NuklearColor
function nk.fillTriangle(canvas, x0, y0, x1, y1, x2, y2, color)
	libnk.fill_triangle(canvas, x0, y0, x1, y1, x2, y2, color)
end
---@param canvas nk_command_buffer
---@param pointList number[]
---@param color NuklearColor
function nk.fillPolygon(canvas, pointList, color)
	libnk.fill_polygon(canvas, pointList, color)
end

-- }======================================================

--[[
** {======================================================
** DrawList
** =======================================================
--]]

---@param canvas nk_command_buffer
---@param rect NuklearRect
---@param img nk_image
---@param color NuklearColor
function nk.drawImage(canvas, rect, img, color)
	libnk.draw_image(canvas, rect, img, color)
end
---@param canvas nk_command_buffer
---@param rect NuklearRect
---@param nineSlice nk_nine_slice
---@param color NuklearColor
function nk.drawNineSlice(canvas, rect, nineSlice, color)
	libnk.draw_nine_slice(canvas, rect, nineSlice, color)
end
---@param canvas nk_command_buffer
---@param rect NuklearRect
---@param text string
---@param font nk_font
---@param bg NuklearColor
---@param fg NuklearColor
function nk.drawText(canvas, rect, text, font, bg, fg)
	libnk.draw_text(canvas, rect, text, font, bg, fg)
end
---@param canvas nk_command_buffer
---@param rect NuklearRect
function nk.pushScissor(canvas, rect)
	libnk.push_scissor(canvas, rect)
end
---@param canvas nk_command_buffer
---@param rect NuklearRect
---@param callback fun(drawList:nk_draw_list, x:integer, y:integer, w:integer, h:integer):void
function nk.pushCustom(canvas, rect, callback)
	libnk.push_custom(canvas, rect, callback)
end

-- }======================================================

--[[
** {======================================================
** DrawList Path
** =======================================================
--]]

---@param drawList nk_draw_list
function nk.drawListPathClear(drawList)
	libnk.draw_list_path_clear(drawList)
end
---@param drawList nk_draw_list
---@param pos NuklearVec2
function nk.drawListPathLineTo(drawList, pos)
	libnk.draw_list_path_line_to(drawList, pos)
end
---@param drawList nk_draw_list
---@param center NuklearVec2
---@param radius number
---@param aMin integer
---@param aMax integer
function nk.drawListPathArcToFast(drawList, center, radius, aMin, aMax)
	libnk.draw_list_path_arc_to_fast(drawList, center, radius, aMin, aMax)
end
---@param drawList nk_draw_list
---@param center NuklearVec2
---@param radius number
---@param aMin number
---@param aMax number
---@param segments integer
function nk.drawListPathArcTo(drawList, center, radius, aMin, aMax, segments)
	libnk.draw_list_path_arc_to(drawList, center, radius, aMin, aMax, segments)
end
---@param drawList nk_draw_list
---@param a NuklearVec2
---@param b NuklearVec2
---@param rounding number
function nk.drawListPathRectTo(drawList, a, b, rounding)
	libnk.draw_list_path_rect_to(drawList, a, b, rounding)
end
---@param drawList nk_draw_list
---@param p2 NuklearVec2
---@param p3 NuklearVec2
---@param p4 NuklearVec2
---@param numSegments integer
function nk.drawListPathCurveTo(drawList, p2, p3, p4, numSegments)
	libnk.draw_list_path_curve_to(drawList, p2, p3, p4, numSegments)
end
---@param drawList nk_draw_list
---@param color NuklearColor
function nk.drawListPathFill(drawList, color)
	libnk.draw_list_path_fill(drawList, color)
end
---@param drawList nk_draw_list
---@param color NuklearColor
---@param closed boolean
---@param thickness number
function nk.drawListPathStroke(drawList, color, closed, thickness)
	libnk.draw_list_path_stroke(drawList, color, closed, thickness)
end

-- }======================================================

--[[
** {======================================================
** DrawList Stroke
** =======================================================
--]]

---@param drawList nk_draw_list
---@param a NuklearVec2
---@param b NuklearVec2
---@param color NuklearColor
---@param thickness number
function nk.drawListStrokeLine(drawList, a, b, color, thickness)
	libnk.draw_list_stroke_line(drawList, a, b, color, thickness)
end
---@param drawList nk_draw_list
---@param rect NuklearVec2
---@param color NuklearColor
---@param rounding number
---@param thickness number
function nk.drawListStrokeRect(drawList, rect, color, rounding, thickness)
	libnk.draw_list_stroke_rect(drawList, rect, color, rounding, thickness)
end
---@param drawList nk_draw_list
---@param a NuklearVec2
---@param b NuklearVec2
---@param c NuklearVec2
---@param color NuklearColor
---@param thickness number
function nk.drawListStrokeTriangle(drawList, a, b, c, color, thickness)
	libnk.draw_list_stroke_triangle(drawList, a, b, c, color, thickness)
end
---@param drawList nk_draw_list
---@param center NuklearVec2
---@param radius number
---@param color NuklearColor
---@param segs integer
---@param thickness number
function nk.drawListStrokeCircle(drawList, center, radius, color, segs, thickness)
	libnk.draw_list_stroke_circle(drawList, center, radius, color, segs, thickness)
end
---@param drawList nk_draw_list
---@param p0 NuklearVec2
---@param cp0 NuklearVec2
---@param cp1 NuklearVec2
---@param p1 NuklearVec2
---@param color NuklearColor
---@param segments integer
---@param thickness number
function nk.drawListStrokeCurve(drawList, p0, cp0, cp1, p1, color, segments, thickness)
	libnk.draw_list_stroke_curve(drawList, p0, cp0, cp1, p1, color, segments, thickness)
end
---@param drawList nk_draw_list
---@param pointList NuklearVec2[]
---@param color NuklearColor
---@param closed boolean
---@param thickness number
---@param aliasing nuklear_anti_aliasing
function nk.drawListStrokePolyLine(drawList, pointList, color, closed, thickness, aliasing)
	libnk.draw_list_stroke_poly_line(drawList, pointList, color, closed, thickness, aliasing)
end

-- }======================================================

--[[
** {======================================================
** DrawList Fill
** =======================================================
--]]

---@param drawList nk_draw_list
---@param rect NuklearRect
---@param c NuklearColor
---@param rounding number
function nk.drawListFillRect(drawList, rect, c, rounding)
	libnk.draw_list_fill_rect(drawList, rect, c, rounding)
end
---@param drawList nk_draw_list
---@param rect NuklearRect
---@param left NuklearColor
---@param top NuklearColor
---@param right NuklearColor
---@param bottom NuklearColor
function nk.drawListFillRectMultiColor(drawList, rect, left, top, right, bottom)
	libnk.draw_list_fill_rect_multi_color(drawList, rect, left, top, right, bottom)
end
---@param drawList nk_draw_list
---@param a NuklearVec2
---@param b NuklearVec2
---@param c NuklearVec2
---@param col NuklearColor
function nk.drawListFillTriangle(drawList, a, b, c, col)
	libnk.draw_list_fill_triangle(drawList, a, b, c, col)
end
---@param drawList nk_draw_list
---@param center NuklearVec2
---@param radius number
---@param col NuklearColor
---@param segs integer
function nk.drawListFillCircle(drawList, center, radius, col, segs)
	libnk.draw_list_fill_circle(drawList, center, radius, col, segs)
end
---@param drawList nk_draw_list
---@param pointList NuklearVec2
---@param color NuklearColor
---@param aliasing nuklear_anti_aliasing
function nk.drawListFillPolyConvex(drawList, pointList, color, aliasing)
	libnk.draw_list_fill_poly_convex(drawList, pointList, color, aliasing)
end

-- }======================================================

--[[
** {======================================================
** DrawList Misc
** =======================================================
--]]

---@param drawList nk_draw_list
---@param image nk_image
---@param rect NuklearRect
---@param color NuklearColor
function nk.drawListAddImage(drawList, image, rect, color)
	libnk.draw_list_add_image(drawList, image, rect, color)
end
---@param drawList nk_draw_list
---@param font nk_font
---@param rect NuklearRect
---@param text string
---@param fontHeight number
---@param color NuklearColor
function nk.drawListAddText(drawList, font, rect, text, fontHeight, color)
	libnk.draw_list_add_text(drawList, font, rect, text, fontHeight, color)
end

---@class nuklear_anti_aliasing
---@field public OFF integer
---@field public ON integer

---@type nuklear_anti_aliasing
nk.anti_aliasing = libnk.anti_aliasing

-- }======================================================

--[[
** {======================================================
** Group
** =======================================================
--]]

---@param title string
---@param flags nuklear_panel_flag
---@return boolean
function nk.groupBegin(title, flags)
	return libnk.groupBegin(curCtx, title, flags)
end
---@param name string
---@param title string
---@param flags nuklear_panel_flag
---@return boolean
function nk.groupBeginTitled(name, title, flags)
	return libnk.group_begin_titled(curCtx, name, title, flags)
end
function nk.groupEnd()
	libnk.group_end(curCtx)
end
---@param title string
---@param flags nuklear_panel_flag
---@return boolean, integer, integer @ret, xOffset, yOffset
function nk.groupScrolledOffsetBegin(title, flags)
	return libnk.group_scrolled_offset_begin(curCtx, title, flags)
end
function nk.groupScrolledEnd()
	libnk.group_scrolled_end(curCtx)
end
---@param name string
---@return integer, integer @xOffset, yOffset
function nk.groupGetScroll(name)
	return libnk.group_get_scroll(curCtx, name)
end
---@param name string
---@param xOffset integer
---@param yOffset integer
function nk.groupSetScroll(name, offsetX, offsetY)
	libnk.group_set_scroll(curCtx, name, offsetX, offsetY)
end

-- }======================================================

--[[
** {======================================================
** Image
** =======================================================
--]]

---@class nk_image
---@field isSubImage(self:isSubImage):boolean

---@param handle integer | lightuserdata
---@param w integer
---@param h integer
---@param subRegion NuklearRect
---@return nk_image
function nk.Image(handle, w, h, subRegion)
	return libnk.Image(handle, w, h, subRegion)
end

---@class nk_nine_slice
---@field isSubNineSlice(self:isSubImage):boolean

---@param handle integer | lightuserdata
---@param w integer
---@param h integer
---@param subRegion NuklearRect
---@param left integer
---@param top integer
---@param right integer
---@param bottom integer
---@return nk_nine_slice
function nk.NineSlice(handle, w, h, subRegion, left, top, right, bottom)
	return libnk.NineSlice(handle, w, h, subRegion, left, top, right, bottom)
end

-- }======================================================

--[[
** {======================================================
** Input
** =======================================================
--]]

---@type integer
nk.INPUT_MAX = libnk.INPUT_MAX

function nk.inputBegin()
	libnk.input_begin(curCtx)
end
---@param x integer
---@param y integer
function nk.inputMotion(x, y)
	libnk.input_motion(curCtx, x, y)
end
---@param key nuklear_keyboard
---@param down boolean
function nk.inputKey(key, down)
	libnk.input_key(curCtx, key, down)
end
---@param button nuklear_mouse_button
---@param x integer
---@param y integer
---@param down boolean
function nk.inputButton(button, x, y, down)
	libnk.input_button(curCtx, button, x, y, down)
end
---@param vec2 NuklearVec2
function nk.inputScroll(vec2)
	libnk.input_scroll(curCtx, vec2)
end
---@param c integer
function nk.inputChar(c)
	libnk.input_scroll(curCtx, c)
end
---@param codepoint integer
function nk.inputUnicode(codepoint)
	libnk.input_scroll(curCtx, codepoint)
end
function nk.inputEnd()
	libnk.input_end(curCtx)
end

-- }======================================================

--[[
** {======================================================
** Input Checking
** =======================================================
--]]

---@param mouseButton nuklear_mouse_button
---@return boolean
function nk.inputHasMouseClick(mouseButton)
	return libnk.input_has_mouse_click(curCtx, mouseButton)
end
---@param mouseButton nuklear_mouse_button
---@param rect NuklearRect
---@return boolean
function nk.inputHasMouseClickInRect(mouseButton, rect)
	return libnk.input_has_mouse_click_in_rect(curCtx, mouseButton, rect)
end
---@param mouseButton nuklear_mouse_button
---@param rect NuklearRect
---@param down boolean
---@return boolean
function nk.inputHasMouseClickDownInRect(mouseButton, rect, down)
	return libnk.input_has_mouse_click_down_in_rect(curCtx, mouseButton, rect, down)
end
---@param mouseButton nuklear_mouse_button
---@param rect NuklearRect
---@return boolean
function nk.inputIsMouseClickInRect(mouseButton, rect)
	return libnk.input_is_mouse_click_in_rect(curCtx, mouseButton, rect)
end
---@param mouseButton nuklear_mouse_button
---@param rect NuklearRect
---@param down boolean
---@return boolean
function nk.inputIsMouseClickDownInRect(mouseButton, rect, down)
	return libnk.input_is_mouse_click_down_in_rect(curCtx, mouseButton, rect, down)
end
---@param rect NuklearRect
---@return boolean
function nk.inputAnyMouseClickInRect(rect)
	return libnk.input_any_mouse_click_in_rect(curCtx, rect)
end
---@param rect NuklearRect
---@return boolean
function nk.inputIsMousePrevHoveringRect(rect)
	return libnk.input_is_mouse_prev_hovering_rect(curCtx, rect)
end
---@param rect NuklearRect
---@return boolean
function nk.inputIsMouseHoveringRect(bounds)
	return libnk.input_is_mouse_hovering_rect(curCtx, bounds)
end
---@param mouseButton nuklear_mouse_button
---@param rect NuklearRect
---@return boolean
function nk.inputMouseClicked(mouseButton, rect)
	return libnk.input_mouse_clicked(curCtx, mouseButton, rect)
end
---@param mouseButton nuklear_mouse_button
---@return boolean
function nk.inputIsMouseDown(mouseButton)
	return libnk.input_is_mouse_down(curCtx, mouseButton)
end
---@param mouseButton nuklear_mouse_button
---@return boolean
function nk.inputIsMousePressed(mouseButton)
	return libnk.input_is_mouse_pressed(curCtx, mouseButton)
end
---@param mouseButton nuklear_mouse_button
---@return boolean
function nk.inputIsMouseReleased(mouseButton)
	return libnk.input_is_mouse_released(curCtx, mouseButton)
end
---@param key nuklear_keyboard
---@return boolean
function nk.inputIsKeyPressed(key)
	return libnk.input_is_key_pressed(curCtx, key)
end
---@param key nuklear_keyboard
---@return boolean
function nk.inputIsKeyReleased(key)
	return libnk.input_is_key_released(curCtx, key)
end
---@param key nuklear_keyboard
---@return boolean
function nk.inputIsKeyDown(key)
	return libnk.input_is_key_down(curCtx, key)
end

---@class nuklear_keyboard
---@field public NONE integer
---@field public SHIFT integer
---@field public CTRL integer
---@field public DEL integer
---@field public ENTER integer
---@field public TAB integer
---@field public BACKSPACE integer
---@field public COPY integer
---@field public CUT integer
---@field public PASTE integer
---@field public UP integer
---@field public DOWN integer
---@field public LEFT integer
---@field public RIGHT integer
---@field public TEXT_INSERT_MODE integer
---@field public TEXT_REPLACE_MODE integer
---@field public TEXT_RESET_MODE integer
---@field public TEXT_LINE_START integer
---@field public TEXT_LINE_END integer
---@field public TEXT_START integer
---@field public TEXT_END integer
---@field public TEXT_UNDO integer
---@field public TEXT_REDO integer
---@field public TEXT_SELECT_ALL integer
---@field public TEXT_WORD_LEFT integer
---@field public TEXT_WORD_RIGHT integer
---@field public SCROLL_START integer
---@field public SCROLL_END integer
---@field public SCROLL_DOWN integer
---@field public SCROLL_UP integer
---@field public MAX integer

---@type nuklear_keyboard
nk.keyboard = libnk.keyboard

---@class nuklear_mouse_button
---@field public LEFT integer
---@field public MIDDLE integer
---@field public RIGHT integer
---@field public DOUBLE integer
---@field public MAX integer

---@type nuklear_mouse_button
nk.mouse_button = libnk.mouse_button

-- }======================================================

--[[
** {======================================================
** Layout
** =======================================================
--]]

---@param height number
function nk.layoutSetMinRowHeight(height)
	libnk.layout_set_min_row_height(curCtx, height)
end
function nk.layoutResetMinRowHeight()
	libnk.layout_reset_min_row_height(curCtx)
end
---@return NuklearRect
function nk.layoutWidgetBounds()
	return libnk.layout_widget_bounds(curCtx)
end
---@param pixelWidth number
---@return number
function nk.layoutRatioFromPixel(pixelWidth)
	return libnk.layout_ratio_from_pixel(curCtx, pixelWidth)
end
---@param height number
---@param cols integer
function nk.layoutRowDynamic(height, cols)
	libnk.layout_row_dynamic(curCtx, height, cols)
end
---@param height number
---@param itemWidth integer
---@param cols integer
function nk.layoutRowStatic(height, itemWidth, cols)
	libnk.layout_row_static(curCtx, height, itemWidth, cols)
end
---@param fmt nuklear_layout_format
---@param rowHeight number
---@param cols integer
function nk.layoutRowBegin(fmt, rowHeight, cols)
	libnk.layout_row_begin(curCtx, fmt, rowHeight, cols)
end
---@param value number
function nk.layoutRowPush(value)
	libnk.layout_row_push(curCtx, value)
end
function nk.layoutRowEnd()
	libnk.layout_row_end(curCtx)
end
---@param fmt nuklear_layout_format
---@param height number
---@param cols integer
---@param ratioList number[]
function nk.layoutRow(fmt, height, cols, ratioList)
	libnk.layout_row(curCtx, fmt, height, cols, ratioList)
end
---@param rowHeight number
function nk.layoutRowTemplateBegin(rowHeight)
	libnk.layout_row_template_begin(curCtx, rowHeight)
end
function nk.layoutRowTemplatePushDynamic()
	libnk.layout_row_template_push_dynamic(curCtx)
end
---@param minWidth number
function nk.layoutRowTemplatePushVariable(minWidth)
	libnk.layout_row_template_push_variable(curCtx, minWidth)
end
---@param width number
function nk.layoutRowTemplatePushStatic(width)
	libnk.layout_row_template_push_static(curCtx, width)
end
function nk.layoutRowTemplateEnd()
	libnk.layout_row_template_end(curCtx)
end
---@param fmt nuklear_layout_format
---@param height number
---@param widgetCount integer
function nk.layoutSpaceBegin(fmt, height, widgetCount)
	libnk.layout_space_begin(curCtx, fmt, height, widgetCount)
end
---@param bounds NuklearRect
function nk.layoutSpacePush(bounds)
	libnk.layout_space_push(curCtx, bounds)
end
function nk.layoutSpaceEnd()
	libnk.layout_space_end(curCtx)
end
---@return NuklearRect
function nk.layoutSpaceBounds()
	return libnk.layout_space_bounds(curCtx)
end
---@param vec2 NuklearVec2
---@return NuklearVec2
function nk.layoutSpaceToScreen(vec2)
	return libnk.layout_space_to_screen(curCtx, vec2)
end
---@param vec2 NuklearVec2
---@return NuklearVec2
function nk.layoutSpaceToLocal(vec2)
	return libnk.layout_space_to_local(curCtx, vec2)
end
---@param rect NuklearRect
---@return NuklearRect
function nk.layoutSpaceRectToScreen(rect)
	return libnk.layout_space_rect_to_screen(curCtx, rect)
end
---@param rect NuklearRect
---@return NuklearRect
function nk.layoutSpaceRectToLocal(rect)
	return libnk.layout_space_rect_to_local(curCtx, rect)
end
function nk.spacer()
	libnk.spacer(curCtx)
end

---@class nuklear_layout_format
---@field public NK_DYNAMIC integer
---@field public NK_STATIC integer

---@type nuklear_layout_format
nk.layout_format = libnk.layout_format

-- }======================================================

--[[
** {======================================================
** Vertex
** =======================================================
--]]

---@param cmds nk_buffer
---@param vbuf nk_buffer @vertices
---@param ebuf nk_buffer @elements
---@param nullTex nk_draw_null_texture
function nk.convert(cmds, vbuf, ebuf, nullTex)
	libnk.convert(curCtx, cmds, vbuf, ebuf, nullTex)
end
---@param cmds nk_buffer
---@param screenWidth integer
---@param screenHeight integer
---@param pixelWidth integer
---@param pixelHeight integer
---@param callback fun(offset:integer, elemCount:integer, textureHandle:integer, x:integer, y:integer, w:integer, h:integer):void
function nk.drawForEach(cmds, screenWidth, screenHeight, pixelWidth, pixelHeight, callback)
	libnk.drawForEach(curCtx, cmds, screenWidth, screenHeight, pixelWidth, pixelHeight, callback)
end

-- }======================================================

--[[
** {======================================================
** ListView
** =======================================================
--]]

---@param idStr string
---@param flag nuklear_panel_flag
---@param rowHeight integer
---@param rowCount integer
---@return boolean, integer, integer @ret, begin, count
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

---@param str string
---@param alignment nuklear_text_align
function nk.textWidget(str, alignment)
	libnk.text_widget(curCtx, str, alignment)
end
---@param str string
---@param alignment nuklear_text_align
---@param color NuklearColor
function nk.textColored(str, alignment, color)
	libnk.text_colored(curCtx, str, alignment, color)
end
---@param str string
function nk.textWrap(str)
	libnk.text_wrap(curCtx, str)
end
---@param str string
---@param color NuklearColor
function nk.textWrapColored(str, color)
	libnk.text_wrap_colored(curCtx, str, color)
end
---@param image nk_image
function nk.imageWidget(image)
	libnk.image_widget(curCtx, image)
end
---@param image nk_image
---@param color NuklearColor
function nk.imageColor(image, color)
	libnk.image_color(curCtx, image, color)
end
---@param prefix string
---@param b boolean
function nk.valueBool(prefix, b)
	libnk.value_bool(curCtx, prefix, b)
end
---@param prefix string
---@param value integer
function nk.valueInt(prefix, value)
	libnk.value_int(curCtx, prefix, value)
end
---@param prefix string
---@param value integer
function nk.valueUint(prefix, value)
	libnk.value_uint(curCtx, prefix, value)
end
---@param prefix string
---@param value integer
function nk.valueFloat(prefix, value)
	libnk.value_float(curCtx, prefix, value)
end
---@param prefix string
---@param color NuklearColor
function nk.valueColorByte(prefix, color)
	libnk.value_color_byte(curCtx, prefix, color)
end
---@param prefix string
---@param color NuklearColor
function nk.valueColorFloat(prefix, color)
	libnk.value_color_float(curCtx, prefix, color)
end
---@param prefix string
---@param color NuklearColor
function nk.valueColorHex(prefix, color)
	libnk.value_color_hex(curCtx, prefix, color)
end

---@class nuklear_text_align
---@field public LEFT number
---@field public CENTERED number
---@field public RIGHT number
---@field public TOP number
---@field public MIDDLE number
---@field public BOTTOM number

---@type nuklear_text_align
nk.text_align = libnk.text_align

---@class nuklear_text_alignment:nuklear_text_align
---@field public LEFT number
---@field public CENTERED number
---@field public RIGHT number

---@type nuklear_text_alignment
nk.text_alignment = libnk.text_alignment

-- }======================================================

--[[
** {======================================================
** Button
** =======================================================
--]]

---@param title string
---@return boolean
function nk.buttonText(title)
	return libnk.button_text(curCtx, title)
end
---@param color NuklearColor
---@return boolean
function nk.buttonColor(color)
	return libnk.button_color(curCtx, color)
end
---@param symbol nuklear_symbol_type
---@return boolean
function nk.buttonSymbol(symbol)
	return libnk.button_symbol(curCtx, symbol)
end
---@param image nk_image
---@return boolean
function nk.buttonImage(img)
	return libnk.button_image(curCtx, img)
end
---@param symbol nuklear_symbol_type
---@param str string
---@param textAlignment nuklear_text_align
---@return boolean
function nk.buttonSymbolText(symbol, str, textAlignment)
	return libnk.button_symbol_text(curCtx, symbol, str, textAlignment)
end
---@param img nk_image
---@param str string
---@param align nuklear_text_align
---@return boolean
function nk.buttonImageText(img, str, align)
	return libnk.button_image_text(curCtx, img, str, align)
end
---@param styleButton nk_style_button
---@param title string
---@return boolean
function nk.buttonTextStyled(styleButton, title)
	return libnk.button_text_styled(curCtx, styleButton, title)
end
---@param styleButton nk_style_button
---@param symbol nuklear_symbol_type
---@return boolean
function nk.buttonSymbolStyled(styleButton, symbol)
	return libnk.button_symbol_styled(curCtx, styleButton, symbol)
end
---@param styleButton nk_style_button
---@param image nk_image
---@return boolean
function nk.buttonImageStyled(styleButton, image)
	return libnk.button_image_styled(curCtx, styleButton, image)
end
---@param styleButton nk_style_button
---@param symbol nuklear_symbol_type
---@param str string
---@param textAlignment nuklear_text_align
---@return boolean
function nk.buttonSymbolTextStyled(styleButton, symbol, str, textAlignment)
	return libnk.button_symbol_text_styled(curCtx, styleButton, symbol, str, textAlignment)
end
---@param styleButton nk_style_button
---@param img nk_image
---@param str string
---@param align nuklear_text_align
---@return boolean
function nk.buttonImageTextStyled(styleButton, img, str, align)
	return libnk.button_image_text_styled(curCtx, styleButton, img, str, align)
end
---@param behavior nuklear_button_behavior
function nk.buttonSetBehavior(behavior)
	libnk.button_set_behavior(curCtx, behavior)
end
---@param behavior nuklear_button_behavior
---@return boolean
function nk.buttonPushBehavior(behavior)
	return libnk.button_push_behavior(curCtx, behavior)
end
---@return boolean
function nk.buttonPopBehavior()
	return libnk.button_pop_behavior(curCtx)
end

---@class nuklear_symbol_type
---@field public NONE integer
---@field public X integer
---@field public UNDERSCORE integer
---@field public CIRCLE_SOLID integer
---@field public CIRCLE_OUTLINE integer
---@field public RECT_SOLID integer
---@field public RECT_OUTLINE integer
---@field public TRIANGLE_UP integer
---@field public TRIANGLE_DOWN integer
---@field public TRIANGLE_LEFT integer
---@field public TRIANGLE_RIGHT integer
---@field public PLUS integer
---@field public MINUS integer
---@field public MAX integer

---@type nuklear_symbol_type
nk.symbol_type = libnk.symbol_type

---@class nuklear_button_behavior
---@field public DEFAULT integer
---@field public REPEATER integer

---@type nuklear_button_behavior
nk.button_behavior = libnk.button_behavior

-- }======================================================

--[[
** {======================================================
** CheckBox
** =======================================================
--]]

---@param text string
---@param active boolean
---@return boolean
function nk.checkText(text, active)
	return libnk.check_text(curCtx, text, active)
end
---@param text string
---@param flags integer
---@param value integer
---@return integer
function nk.checkFlagsText(text, flags, value)
	return libnk.check_flags_text(curCtx, text, flags, value)
end

-- }======================================================

--[[
** {======================================================
** RadioButton
** =======================================================
--]]

---@param text string
---@param active boolean
---@return boolean
function nk.optionText(text, active)
	return libnk.option_text(curCtx, text, active)
end

-- }======================================================

--[[
** {======================================================
** Selectable
** =======================================================
--]]

---@param text string
---@param align nuklear_text_align
---@param value boolean
---@return boolean
function nk.selectText(text, align, value)
	return libnk.select_text(curCtx, text, align, value)
end
---@param img nk_image
---@param text string
---@param align nuklear_text_align
---@param value boolean
---@return boolean
function nk.selectImageText(img, text, align, value)
	return libnk.select_image_text(curCtx, img, text, align, value)
end
---@param symbol nuklear_symbol_type
---@param text string
---@param align nuklear_text_align
---@param value boolean
---@return boolean
function nk.selectSymbolText(symbol, text, align, value)
	return libnk.select_symbol_text(curCtx, symbol, text, align, value)
end

-- }======================================================

--[[
** {======================================================
** Slider
** =======================================================
--]]

---@param min number
---@param val number
---@param max number
---@param step number
---@return number
function nk.slideFloat(min, val, max, step)
	return libnk.slide_float(curCtx, min, val, max, step)
end
---@param min integer
---@param val integer
---@param max integer
---@param step integer
---@return integer
function nk.slideInt(min, val, max, step)
	return libnk.slide_int(curCtx, min, val, max, step)
end

-- }======================================================

--[[
** {======================================================
** ProgressBar
** =======================================================
--]]

---@param cur integer
---@param max integer
---@param modifyable boolean
---@return integer
function nk.prog(cur, max, modifyable)
	return libnk.prog(curCtx, cur, max, modifyable)
end

-- }======================================================

--[[
** {======================================================
** ColorPicker
** =======================================================
--]]

---@param color NuklearColor
---@param format nuklear_color_format
---@return NuklearColor
function nk.colorPicker(color, format)
	return libnk.color_picker(curCtx, color, format)
end

---@class nuklear_color_format
---@field public RGB integer
---@field public RGBA integer

---@type nuklear_color_format
nk.color_format = libnk.color_format

-- }======================================================

--[[
** {======================================================
** Properties
** =======================================================
--]]

---@param name string
---@param min integer
---@param val integer
---@param max integer
---@param step integer
---@param incPerPixel number
---@return integer
function nk.propertyi(name, min, val, max, step, incPerPixel)
	return libnk.propertyi(curCtx, name, min, val, max, step, incPerPixel)
end
---@param name string
---@param min number
---@param val number
---@param max number
---@param step number
---@param incPerPixel number
---@return number
function nk.propertyd(name, min, val, max, step, incPerPixel)
	return libnk.propertyd(curCtx, name, min, val, max, step, incPerPixel)
end

-- }======================================================

--[[
** {======================================================
** TextEdit
** =======================================================
--]]

---@alias FilterSignature lightuserdata | fun(editor:nk_text_edit, codepoint:integer):boolean

---@overload fun(flag:nuklear_edit_flag, buffer:nk_buffer):integer
---@overload fun(flag:nuklear_edit_flag, buffer:nk_buffer, filter:FilterSignature):integer
---@param flag nuklear_edit_flag
---@param buffer nk_buffer
---@param filter FilterSignature
---@return nuklear_edit_event
function nk.editString(flag, buffer, filter)
	return libnk.edit_string(curCtx, flag, buffer, filter)
end
---@overload fun(flag:nuklear_edit_flag, editor:nk_text_edit):integer
---@overload fun(flag:nuklear_edit_flag, editor:nk_text_edit, filter:FilterSignature):integer
---@param flag nuklear_edit_flag
---@param editor nk_text_edit
---@param filter FilterSignature
---@return nuklear_edit_event
function nk.editBuffer(flag, editor, filter)
	return libnk.edit_buffer(curCtx, flag, editor, filter)
end
---@param flag nuklear_edit_flag
function nk.editFocus(flag)
	libnk.edit_focus(curCtx, flag)
end
function nk.editUnfocus()
	libnk.edit_unfocus(curCtx)
end

---@class nuklear_edit_flag
---@field public DEFAULT integer
---@field public READ_ONLY integer
---@field public AUTO_SELECT integer
---@field public SIG_ENTER integer
---@field public ALLOW_TAB integer
---@field public NO_CURSOR integer
---@field public SELECTABLE integer
---@field public CLIPBOARD integer
---@field public CTRL_ENTER_NEWLINE integer
---@field public NO_HORIZONTAL_SCROLL integer
---@field public ALWAYS_INSERT_MODE integer
---@field public MULTILINE integer
---@field public GOTO_END_ON_ACTIVATE integer

---@type nuklear_edit_flag
nk.edit_flag = libnk.edit_flag

---@class nuklear_edit_type
---@field public SIMPLE integer
---@field public FIELD integer
---@field public BOX integer
---@field public EDITOR integer

---@type nuklear_edit_type
nk.edit_type = libnk.edit_type

---@class nuklear_edit_event
---@field public ACTIVE integer
---@field public INACTIVE integer
---@field public ACTIVATED integer
---@field public DEACTIVATED integer
---@field public COMMITED integer

---@type nuklear_edit_event
nk.edit_event = libnk.edit_event

-- }======================================================

--[[
** {======================================================
** Chart
** =======================================================
--]]

---@param type nuklear_chart_type
---@param num integer
---@param min number
---@param max number
---@return boolean
function nk.chartBegin(type, num, min, max)
	return libnk.chart_begin(curCtx, type, num, min, max)
end
---@param type nuklear_chart_type
---@param color NuklearColor
---@param highLightColor NuklearColor
---@param num integer
---@param min number
---@param max number
---@return boolean
function nk.chartBeginColored(type, color, highLightColor, num, min, max)
	return libnk.chart_begin_colored(curCtx, type, color, highLightColor, num, min, max)
end
---@param type nuklear_chart_type
---@param count integer
---@param minValue number
---@param maxValue number
function nk.chartAddSlot(type, count, minValue, maxValue)
	libnk.chart_add_slot(curCtx, type, count, minValue, maxValue)
end
---@param type nuklear_chart_type
---@param color NuklearColor
---@param highLightColor NuklearColor
---@param count integer
---@param minValue number
---@param maxValue number
function nk.chartAddSlotColored(type, color, highLightColor, count, minValue, maxValue)
	libnk.chart_add_slot_colored(curCtx, type, color, highLightColor, count, minValue, maxValue)
end
---@overload fun(value:number):integer
---@overload fun(value:number, slot:integer):integer
---@param value number
---@param slot integer
---@return integer
function nk.chartPushSlot(value, slot)
	return libnk.chart_push_slot(curCtx, value, slot)
end
function nk.chartEnd()
	libnk.chart_end(curCtx)
end
---@param type nuklear_chart_type
---@param valueList number[]
---@param offset integer
function nk.plot(type, valueList, offset)
	libnk.plot(curCtx, type, valueList, offset)
end
---@param type nuklear_chart_type
---@param valueGetter fun(index:integer):number
---@param count integer
---@param offset integer
function nk.plotFunction(type, valueGetter, count, offset)
	libnk.plot_function(curCtx, type, valueGetter, count, offset)
end

---@class nuklear_chart_type
---@field public LINES integer
---@field public COLUMN integer
---@field public MAX integer

---@type nuklear_chart_type
nk.chart_type = libnk.chart_type

---@class nuklear_chart_event
---@field public HOVERING integer
---@field public CLICKED integer

---@type nuklear_chart_event
nk.chart_event = libnk.chart_event

-- }======================================================

--[[
** {======================================================
** Popup
** =======================================================
--]]

---@param type nuklear_popup_type
---@param title string
---@param flags nuklear_panel_flag
---@param rect NuklearRect
---@return boolean
function nk.popupBegin(type, title, flags, rect)
	return libnk.popup_begin(curCtx, type, title, flags, rect)
end
function nk.popupClose()
	libnk.popup_close(curCtx)
end
function nk.popupEnd()
	libnk.popup_end(curCtx)
end
---@return integer, integer @offsetX, offsetY
function nk.popupGetScroll()
	return libnk.popup_get_scroll(curCtx)
end
---@param offsetX integer
---@param offsetY integer
function nk.popupSetScroll(offsetX, offsetY)
	libnk.popup_set_scroll(curCtx, offsetX, offsetY)
end

---@class nuklear_popup_type
---@field public STATIC integer
---@field public DYNAMIC integer

---@type nuklear_popup_type
nk.popup_type = libnk.popup_type

-- }======================================================

--[[
** {======================================================
** ComboBox
** =======================================================
--]]

---@param itemList string[]
---@param selected integer
---@param itemHeight integer
---@param sizeVec2 NuklearVec2
---@return integer
function nk.combo(itemList, selected, itemHeight, sizeVec2)
	return libnk.combo(curCtx, itemList, selected, itemHeight, sizeVec2)
end
---@param itemStr string
---@param separator integer
---@param selected integer
---@param count integer
---@param itemHeight integer
---@param sizeVec2 NuklearVec2
---@return integer
function nk.comboSeparator(itemStr, separator, selected, count, itemHeight, sizeVec2)
	return libnk.combo_separator(curCtx, itemStr, separator, selected, count, itemHeight, sizeVec2)
end
---@param itemGetter fun(index:integer):string
---@param selected integer
---@param count integer
---@param itemHeight integer
---@param sizeVec2 NuklearVec2
---@return integer
function nk.comboCallback(itemGetter, selected, count, itemHeight, sizeVec2)
	return libnk.combo_callback(curCtx, itemGetter, selected, count, itemHeight, sizeVec2)
end

-- }======================================================

--[[
** {======================================================
** AbstractComboBox
** =======================================================
--]]

---@param selectedStr string
---@param sizeVec2 NuklearVec2
---@return boolean
function nk.comboBeginText(selectedStr, sizeVec2)
	return libnk.combo_begin_text(curCtx, selectedStr, sizeVec2)
end
---@param color NuklearColor
---@param sizeVec2 NuklearVec2
---@return boolean
function nk.comboBeginColor(color, sizeVec2)
	return libnk.combo_begin_color(curCtx, color, sizeVec2)
end
---@param symbol nuklear_symbol_type
---@param sizeVec2 NuklearVec2
---@return boolean
function nk.comboBeginSymbol(symbol, sizeVec2)
	return libnk.combo_begin_symbol(curCtx, symbol, sizeVec2)
end
---@param selectedStr string
---@param symbol nuklear_symbol_type
---@param sizeVec2 NuklearVec2
---@return boolean
function nk.comboBeginSymbolText(selectedStr, symbol, sizeVec2)
	return libnk.combo_begin_symbol_text(curCtx, selectedStr, symbol, sizeVec2)
end
---@param img nk_image
---@param sizeVec2 NuklearVec2
---@return boolean
function nk.comboBeginImage(img, sizeVec2)
	return libnk.combo_begin_image(curCtx, img, sizeVec2)
end
---@param selectedStr string
---@param img nk_image
---@param sizeVec2 NuklearVec2
---@return boolean
function nk.comboBeginImageText(selectedStr, img, sizeVec2)
	return libnk.combo_begin_image_text(curCtx, selectedStr, img, sizeVec2)
end
---@param text string
---@param alignment nuklear_text_align
---@return boolean
function nk.comboItemText(text, alignment)
	return libnk.combo_item_text(curCtx, text, alignment)
end
---@param img nk_image
---@param text string
---@param alignment nuklear_text_align
---@return boolean
function nk.comboItemImageText(img, text, alignment)
	return libnk.combo_item_image_text(curCtx, img, text, alignment)
end
---@param symbol nuklear_symbol_type
---@param text string
---@param alignment nuklear_text_align
---@return boolean
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

---@param flags nuklear_panel_flag
---@param size NuklearVec2
---@param triggleBounds NuklearRect
---@return boolean
function nk.contextualBegin(flags, size, triggleBounds)
	return libnk.contextual_begin(curCtx, flags, size, triggleBounds)
end
---@param text string
---@param alignment nuklear_text_align
---@return boolean
function nk.contextualItemText(text, alignment)
	return libnk.contextual_item_text(curCtx, text, alignment)
end
---@param img nk_image
---@param text string
---@param alignment nuklear_text_align
---@return boolean
function nk.contextualItemImageText(img, text, alignment)
	return libnk.contextual_item_image_text(curCtx, img, text, alignment)
end
---@param symbol nuklear_symbol_type
---@param text string
---@param alignment nuklear_text_align
---@return boolean
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

---@param str string
function nk.tooltip(str)
	libnk.tooltip(curCtx, str)
end
---@param width number
---@return boolean
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
---@param title string
---@param align nuklear_text_align
---@param sizeVec2 NuklearVec2
---@return boolean
function nk.menuBeginText(title, align, sizeVec2)
	return libnk.menu_begin_text(curCtx, title, align, sizeVec2)
end
---@param idStr string
---@param img nk_image
---@param sizeVec2 NuklearVec2
---@return boolean
function nk.menuBeginImage(idStr, img, sizeVec2)
	return libnk.menu_begin_image(curCtx, idStr, img, sizeVec2)
end
---@param title string
---@param align nuklear_text_align
---@param img nk_image
---@param sizeVec2 NuklearVec2
---@return boolean
function nk.menuBeginImageText(title, align, img, sizeVec2)
	return libnk.menu_begin_image_text(curCtx, title, align, img, sizeVec2)
end
---@param idStr string
---@param symbol nuklear_symbol_type
---@param sizeVec2 NuklearVec2
---@return boolean
function nk.menuBeginSymbol(idStr, symbol, sizeVec2)
	return libnk.menu_begin_symbol(curCtx, idStr, symbol, sizeVec2)
end
---@param title string
---@param align nuklear_text_align
---@param symbol nuklear_symbol_type
---@param sizeVec2 NuklearVec2
---@return boolean
function nk.menuBeginSymbolText(title, align, symbol, sizeVec2)
	return libnk.menu_begin_symbol_text(curCtx, title, align, symbol, sizeVec2)
end
---@param title string
---@param align nuklear_text_align
---@return boolean
function nk.menuItemText(title, align)
	return libnk.menu_item_text(curCtx, title, align)
end
---@param img nk_image
---@param title string
---@param align nuklear_text_align
---@return boolean
function nk.menuItemImageText(img, title, align)
	return libnk.menu_item_image_text(curCtx, img, title, align)
end
---@param symbol nuklear_symbol_type
---@param title string
---@param align nuklear_text_align
---@return boolean
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
---@param table NuklearColor[]
function nk.styleFromTable(table)
	libnk.style_from_table(curCtx, table)
end
---@param atlas nk_font_atlas
function nk.styleLoadAllCursors(atlas)
	libnk.style_load_all_cursors(curCtx, atlas)
end
---@param styleColor nuklear_style_color
---@return string @name
function nk.styleGetColorName(styleColor)
	return libnk.style_get_color_name(styleColor)
end
---@param font nk_font
function nk.styleSetFont(font)
	libnk.style_set_font(curCtx, font)
end
---@param styleCursor nuklear_style_cursor
---@return boolean
function nk.styleSetCursor(styleCursor)
	return libnk.style_set_cursor(curCtx, styleCursor)
end
function nk.styleShowCursor()
	libnk.style_show_cursor(curCtx)
end
function nk.styleHideCursor()
	libnk.style_hide_cursor(curCtx)
end
---@param font nk_font
---@return boolean
function nk.stylePushFont(font)
	return libnk.style_push_font(curCtx, font)
end
---@param ptr lightuserdata
---@param value number
---@return boolean
function nk.stylePushFloat(ptr, value)
	return libnk.style_push_float(curCtx, ptr, value)
end
---@param ptr lightuserdata
---@param value NuklearVec2
---@return boolean
function nk.stylePushVec2(ptr, value)
	return libnk.style_push_vec2(curCtx, ptr, value)
end
---@param ptr lightuserdata
---@param value nk_style_item
---@return boolean
function nk.stylePushStyleItem(ptr, value)
	return libnk.style_push_style_item(curCtx, ptr, value)
end
---@param ptr lightuserdata
---@param value integer
---@return boolean
function nk.stylePushFlags(ptr, value)
	return libnk.style_push_flags(curCtx, ptr, value)
end
---@param ptr lightuserdata
---@param value NuklearColor
---@return boolean
function nk.stylePushColor(ptr, value)
	return libnk.style_push_color(curCtx, ptr, value)
end
---@return boolean
function nk.stylePopFont()
	return libnk.style_pop_font(curCtx)
end
---@return boolean
function nk.stylePopFloat()
	return libnk.style_pop_float(curCtx)
end
---@return boolean
function nk.stylePopVec2()
	return libnk.style_pop_vec2(curCtx)
end
---@return boolean
function nk.stylePopStyleItem()
	return libnk.style_pop_style_item(curCtx)
end
---@return boolean
function nk.stylePopFlags()
	return libnk.style_pop_flags(curCtx)
end
---@return boolean
function nk.stylePopColor()
	return libnk.style_pop_color(curCtx)
end

---@class nuklear_style_color
---@field public TEXT integer
---@field public WINDOW integer
---@field public HEADER integer
---@field public BORDER integer
---@field public BUTTON integer
---@field public BUTTON_HOVER integer
---@field public BUTTON_ACTIVE integer
---@field public TOGGLE integer
---@field public TOGGLE_HOVER integer
---@field public TOGGLE_CURSOR integer
---@field public SELECT integer
---@field public SELECT_ACTIVE integer
---@field public SLIDER integer
---@field public SLIDER_CURSOR integer
---@field public SLIDER_CURSOR_HOVER integer
---@field public SLIDER_CURSOR_ACTIVE integer
---@field public PROPERTY integer
---@field public EDIT integer
---@field public EDIT_CURSOR integer
---@field public COMBO integer
---@field public CHART integer
---@field public CHART_COLOR integer
---@field public CHART_COLOR_HIGHLIGHT integer
---@field public SCROLLBAR integer
---@field public SCROLLBAR_CURSOR integer
---@field public SCROLLBAR_CURSOR_HOVER integer
---@field public SCROLLBAR_CURSOR_ACTIVE integer
---@field public TAB_HEADER integer
---@field public COUNT integer

---@type nuklear_style_color
nk.style_color = libnk.style_color

---@class nuklear_style_cursor
---@field public ARROW integer
---@field public TEXT integer
---@field public MOVE integer
---@field public RESIZE_VERTICAL integer
---@field public RESIZE_HORIZONTAL integer
---@field public RESIZE_TOP_LEFT_DOWN_RIGHT integer
---@field public RESIZE_TOP_RIGHT_DOWN_LEFT integer
---@field public COUNT integer

---@type nuklear_style_cursor
nk.style_cursor = libnk.style_cursor

-- }======================================================

--[[
** {======================================================
** StyleItem
** =======================================================
--]]

---@class nk_style_item

---@overload fun():nk_style_item
---@overload fun(value:NuklearColor | nk_image | nk_nine_slice):nk_style_item
---@param value NuklearColor | nk_image | nk_nine_slice | nil
---@return nk_style_item
function nk.StyleItem(value)
	return libnk.StyleItem(value)
end

-- }======================================================

--[[
** {======================================================
** StyleButton
** =======================================================
--]]

---@alias DrawEventSignature fun(cmds:nk_command_buffer):void

---@class nk_style_button
---@field public normal nk_style_item
---@field public hover nk_style_item
---@field public active nk_style_item
---@field public borderColor NuklearColor
---@field public textBackground NuklearColor
---@field public textNormal NuklearColor
---@field public textHover NuklearColor
---@field public textActive NuklearColor
---@field public textAlignment nuklear_text_align
---@field public border number
---@field public rounding number
---@field public padding NuklearVec2
---@field public imagePadding NuklearVec2
---@field public touchPadding NuklearVec2
---@field public drawBegin DrawEventSignature
---@field public drawEnd DrawEventSignature

---@param field nk_style_button @table<string, value>
---@return nk_style_button
function nk.StyleButton(fields)
	return libnk.StyleButton(fields)
end

-- }======================================================

--[[
** {======================================================
** TextEditor
** =======================================================
--]]

---@class nk_text_edit
---@field public text fun(self:nk_text_edit, str:string):void
---@field public delete fun(self:nk_text_edit, where:integer, len:integer):void
---@field public deleteSelection fun(self:nk_text_edit):void
---@field public selectAll fun(self:nk_text_edit):void
---@field public cut fun(self:nk_text_edit):boolean
---@field public paste fun(self:nk_text_edit, str:string):boolean
---@field public undo fun(self:nk_text_edit):void
---@field public redo fun(self:nk_text_edit):void

---@return nk_text_edit
function nk.TextEditor()
	return libnk.TextEditor()
end

---@type FilterSignature
nk.filterDefaultPtr = libnk.filterDefaultPtr
---@type FilterSignature
nk.filterAsciiPtr = libnk.filterAsciiPtr
---@type FilterSignature
nk.filterFloatPtr = libnk.filterFloatPtr
---@type FilterSignature
nk.filterDecimalPtr = libnk.filterDecimalPtr
---@type FilterSignature
nk.filterHexPtr = libnk.filterHexPtr
---@type FilterSignature
nk.filterOctPtr = libnk.filterOctPtr
---@type FilterSignature
nk.filterBinaryPtr = libnk.filterBinaryPtr

-- }======================================================

--[[
** {======================================================
** Tree
** =======================================================
--]]

---@alias nuklear_collapse_states boolean

---@param type nuklear_tree_type
---@param title string
---@param initialState nuklear_collapse_states
---@param hashStr string
---@param seed integer
---@return boolean
function nk.treePushHashed(type, title, initialState, hashStr, seed)
	return libnk.tree_push_hashed(curCtx, type, title, initialState, hashStr, seed)
end
---@param type nuklear_tree_type
---@param img nk_image
---@param title string
---@param initialState nuklear_collapse_states
---@param hashStr string
---@param seed integer
---@return boolean
function nk.treeImagePushHashed(type, img, title, initialState, hashStr, seed)
	return libnk.tree_image_push_hashed(curCtx, type, img, title, initialState, hashStr, seed)
end
function nk.treePop()
	libnk.tree_pop(curCtx)
end
---@param type nuklear_tree_type
---@param title string
---@param state nuklear_collapse_states
---@return boolean, nuklear_collapse_states @ret, state
function nk.treeStatePush(type, title, state)
	return libnk.tree_state_push(curCtx, type, title, state)
end
---@param type nuklear_tree_type
---@param img nk_image
---@param title string
---@param state nuklear_collapse_states
---@return boolean, nuklear_collapse_states
function nk.treeStateImagePush(type, img, title, state)
	return libnk.tree_state_image_push(curCtx, type, img, title, state)
end
function nk.treeStatePop()
	libnk.tree_state_pop(curCtx)
end
---@param type nuklear_tree_type
---@param title string
---@param initialState nuklear_collapse_states
---@param selected boolean
---@param hashStr string
---@param seed integer
---@return boolean, boolean @ret, selected
function nk.treeElementPushHashed(type, title, initialState, selected, hashStr, seed)
	return libnk.tree_element_push_hashed(curCtx, type, title, initialState, selected, hashStr, seed)
end
---@param type nuklear_tree_type
---@param img nk_image
---@param title string
---@param initialState nuklear_collapse_states
---@param selected boolean
---@param hashStr string
---@param seed integer
---@return boolean, boolean @ret, selected
function nk.treeElementImagePushHashed(type, img, title, initialState, selected, hashStr, seed)
	return libnk.tree_element_image_push_hashed(curCtx, type, img, title, initialState, selected, hashStr, seed)
end
function nk.treeElementPop()
	libnk.tree_element_pop(curCtx)
end

---@class nuklear_tree_type
---@field public NODE integer
---@field public TAB integer

---@type nuklear_tree_type
nk.tree_type = libnk.tree_type

-- }======================================================

--[[
** {======================================================
** Color
** =======================================================
--]]

---@class NuklearColor:integer

---@overload fun(r:number, g:number, b:number):NuklearColor
---@overload fun(r:number, g:number, b:number, a:number):NuklearColor
---@param r number
---@param g number
---@param b number
---@param a number
---@return NuklearColor
function nk.rgbaToColor(r, g, b, a)
	return libnk.rgbaToColor(r, g, b, a)
end
---@param color NuklearColor
---@return integer, integer, integer, integer @ [0, 255]
function nk.colorToRgba(color)
	return libnk.colorToRgba(color)
end
---@param color NuklearColor
---@return number, number, number, number @[0.0, 1.0]
function nk.colorToRgbaf(color)
	return libnk.colorToRgbaf(color)
end
---@overload fun(h:number, s:number, v:number):NuklearColor
---@overload fun(h:number, s:number, v:number, a:number):NuklearColor
---@param h number
---@param s number
---@param v number
---@param a number
---@return NuklearColor
function nk.hsvaToColor(h, s, v, a)
	return libnk.hsvaToColor(h, s, v, a)
end
---@param color NuklearColor
---@return integer, integer, integer, integer
function nk.colorToHsva(color)
	return libnk.colorToHsva(color)
end
---@param color NuklearColor
---@return number, number, number, number
function nk.colorToHsvaf(color)
	return libnk.colorToHsvaf(color)
end
---@param rgbHexStr string
---@return NuklearColor
function nk.rgbHexToColor(rgbHexStr)
	return libnk.rgbHexToColor(rgbHexStr)
end
---@param color NuklearColor
---@return string
function nk.colorToRgbHex(color)
	return libnk.colorToRgbHex(color)
end
---@param rgbaHexStr string
---@return NuklearColor
function nk.rgbaHexToColor(rgbaHexStr)
	return libnk.rgbaHexToColor(rgbaHexStr)
end
---@param color NuklearColor
---@return string
function nk.colorToRgbaHex(color)
	return libnk.colorToRgbaHex(color)
end

-- }======================================================

--[[
** {======================================================
** Math
** =======================================================
--]]

---@param keyStr string
---@param seed integer
---@return integer
function nk.murmurHash(keyStr, seed)
	return libnk.murmur_hash(keyStr, seed)
end
---@param rect NuklearRect
---@param padX number
---@param padY number
---@param direction nuklear_heading
---@return NuklearVec2, NuklearVec2, NuklearVec2
function nk.triangleFromDirection(rect, padX, padY, direction)
	return libnk.triangle_from_direction(rect, padX, padY, direction)
end
---@return NuklearRect
function nk.getNullRect()
	return libnk.get_null_rect()
end

---@class nuklear_heading
---@field public UP integer
---@field public RIGHT integer
---@field public DOWN integer
---@field public LEFT integer

---@type nuklear_heading
nk.heading = libnk.heading

-- }======================================================

--[[
** {======================================================
** Math
** =======================================================
--]]

---@class NuklearVec2:integer

---@param x number
---@param y number
---@return NuklearVec2
function nk.packVec2(x, y)
	return libnk.packVec2(x, y)
end
---@param vec2 NuklearVec2
---@return number, number
function nk.unpackVec2(vec2)
	return libnk.unpackVec2(vec2)
end

---@class NuklearRect:integer

---@param x integer
---@param y integer
---@param w integer
---@param h integer
---@return NuklearRect
function nk.packRect(x, y, w, h)
	return libnk.packRect(x, y, w, h)
end
---@param rect NuklearRect
---@return integer, integer, integer, integer
function nk.unpackRect(rect)
	return libnk.unpackRect(rect)
end

-- }======================================================

--[[
** {======================================================
** Widget
** =======================================================
--]]

---@return nuklear_widget_layout_state, NuklearRect
function nk.widget()
	return libnk.widget(curCtx)
end
---@param itemPadding NuklearVec2
---@return nuklear_widget_layout_state, NuklearRect
function nk.widgetFitting(itemPadding)
	return libnk.widget_fitting(curCtx, itemPadding)
end
---@return NuklearRect
function nk.widgetBounds()
	return libnk.widget_bounds(curCtx)
end
---@return NuklearVec2
function nk.widgetPosition()
	return libnk.widget_position(curCtx)
end
---@return NuklearVec2
function nk.widgetSize()
	return libnk.widget_size(curCtx)
end
---@return number
function nk.widgetWidth()
	return libnk.widget_width(curCtx)
end
---@return number
function nk.widgetHeight()
	return libnk.widget_height(curCtx)
end
---@return boolean
function nk.widgetIsHovered()
	return libnk.widget_is_hovered(curCtx)
end
---@param button nuklear_mouse_button
---@return boolean
function nk.widgetIsMouseClicked(button)
	return libnk.widget_is_mouse_clicked(curCtx, button)
end
---@param button nuklear_mouse_button
---@param down boolean
---@return boolean
function nk.widgetHasMouseClickDown(button, down)
	return libnk.widget_has_mouse_click_down(curCtx, button, down)
end
function nk.spacing(cols)
	libnk.spacing(curCtx, cols)
end

---@class nuklear_widget_layout_state
---@field public INVALID integer
---@field public VALID integer
---@field public ROM integer

---@type nuklear_widget_layout_state
nk.widget_layout_state = libnk.widget_layout_state

-- }======================================================

--[[
** {======================================================
** Window
** =======================================================
--]]

---@param title string
---@param bounds NuklearRect
---@param flags nuklear_panel_flag
---@return boolean
function nk.begin(title, bounds, flags)
	return libnk.begin(curCtx, title, bounds, flags)
end
---@param name string
---@param title string
---@param bounds NuklearRect
---@param flags nuklear_panel_flag
---@return boolean
function nk.beginTitled(name, title, bounds, flags)
	return libnk.begin_titled(curCtx, name, title, bounds, flags)
end
function nk.endWindow()
	libnk.end_window(curCtx)
end
---@return NuklearRect
function nk.windowGetBounds()
	return libnk.window_get_bounds(curCtx)
end
---@return NuklearVec2
function nk.windowGetPosition()
	return libnk.window_get_position(curCtx)
end
---@return NuklearVec2
function nk.windowGetSize()
	return libnk.window_get_size(curCtx)
end
---@return number
function nk.windowGetWidth()
	return libnk.window_get_width(curCtx)
end
---@return number
function nk.windowGetHeight()
	return libnk.window_get_height(curCtx)
end
---@return NuklearRect
function nk.windowGetContentRegion()
	return libnk.window_get_content_region(curCtx)
end
---@return NuklearVec2
function nk.windowGetContentRegionMin()
	return libnk.window_get_content_region_min(curCtx)
end
---@return NuklearVec2
function nk.windowGetContentRegionMax()
	return libnk.window_get_content_region_max(curCtx)
end
---@return NuklearVec2
function nk.windowGetContentRegionSize()
	return libnk.window_get_content_region_size(curCtx)
end

---@class nk_command_buffer

---@return nk_command_buffer
function nk.windowGetCanvas()
	return libnk.window_get_canvas(curCtx)
end
---@return integer, integer
function nk.windowGetScroll()
	return libnk.window_get_scroll(curCtx)
end
---@return boolean
function nk.windowHasFocus()
	return libnk.window_has_focus(curCtx)
end
---@return boolean
function nk.windowIsHovered()
	return libnk.window_is_hovered(curCtx)
end
---@param name string
---@return boolean
function nk.windowIsCollapsed(name)
	return libnk.window_is_collapsed(curCtx, name)
end
---@param name string
---@return boolean
function nk.windowIsClosed(name)
	return libnk.window_is_closed(curCtx, name)
end
---@param name string
---@return boolean
function nk.windowIsHidden(name)
	return libnk.window_is_hidden(curCtx, name)
end
---@param name string
---@return boolean
function nk.windowIsActive(name)
	return libnk.window_is_active(curCtx, name)
end
---@return boolean
function nk.windowIsAnyHovered()
	return libnk.window_is_any_hovered(curCtx)
end
---@return boolean
function nk.itemIsAnyActive()
	return libnk.item_is_any_active(curCtx)
end
---@param name string
---@param bounds NuklearRect
function nk.windowSetBounds(name, bounds)
	libnk.window_set_bounds(curCtx, name, bounds)
end
---@param name string
---@param pos NuklearVec2
function nk.windowSetPosition(name, pos)
	libnk.window_set_position(curCtx, name, pos)
end
---@param name string
---@param size NuklearVec2
function nk.windowSetSize(name, size)
	libnk.window_set_size(curCtx, name, size)
end
---@param name string
function nk.windowSetFocus(name)
	libnk.window_set_focus(curCtx, name)
end
---@param offsetX integer
---@param offsetY integer
function nk.windowSetScroll(offsetX, offsetY)
	libnk.window_set_scroll(curCtx, offsetX, offsetY)
end
---@param name string
function nk.windowClose(name)
	libnk.window_close(curCtx, name)
end
---@param name string
---@param state nuklear_collapse_states
function nk.windowCollapse(name, state)
	libnk.window_collapse(curCtx, name, state)
end
---@param name string
---@param state nuklear_collapse_states
---@param cond integer
function nk.windowCollapseIf(name, state, cond)
	libnk.window_collapse_if(curCtx, name, state, cond)
end

---@alias nuklear_show_states boolean

---@param name string
---@param state nuklear_show_states
function nk.windowShow(name, state)
	libnk.window_show(curCtx, name, state)
end
---@param name string
---@param state nuklear_show_states
---@param cond integer
function nk.windowShowIf(name, state, cond)
	libnk.window_show_if(curCtx, name, state, cond)
end

---@class nuklear_panel_flag
---@field public BORDER integer
---@field public MOVABLE integer
---@field public SCALABLE integer
---@field public CLOSABLE integer
---@field public MINIMIZABLE integer
---@field public NO_SCROLLBAR integer
---@field public TITLE integer
---@field public SCROLL_AUTO_HIDE integer
---@field public BACKGROUND integer
---@field public SCALE_LEFT integer
---@field public NO_INPUT integer

---@type nuklear_panel_flag
nk.panel_flag = libnk.panel_flag

-- }======================================================

return nk
