#include "nuklear.h"
#include "nuklear_internal.h"

/* ===============================================================
 *
 *                          INPUT
 *
 * ===============================================================*/
NK_API void nk_input_begin(nk_context* ctx) {
  int i;
  nk_input* in;
  NK_ASSERT(ctx);
  if (!ctx)
    return;
  in = &ctx->input;
  for (i = 0; i < NK_BUTTON_MAX; ++i)
    in->mouse.buttons[i].clicked = 0;

  in->keyboard.text_len = 0;
  in->mouse.scroll_delta = nk_make_vec2(0, 0);
  in->mouse.prev.x = in->mouse.pos.x;
  in->mouse.prev.y = in->mouse.pos.y;
  in->mouse.delta.x = 0;
  in->mouse.delta.y = 0;
  for (i = 0; i < NK_KEY_MAX; i++)
    in->keyboard.keys[i].clicked = 0;
}
NK_API void nk_input_end(nk_context* ctx) {
  nk_input* in;
  NK_ASSERT(ctx);
  if (!ctx)
    return;
  in = &ctx->input;
  if (in->mouse.grab)
    in->mouse.grab = 0;
  if (in->mouse.ungrab) {
    in->mouse.grabbed = 0;
    in->mouse.ungrab = 0;
    in->mouse.grab = 0;
  }
}
NK_API void nk_input_motion(nk_context* ctx, int x, int y) {
  nk_input* in;
  NK_ASSERT(ctx);
  if (!ctx)
    return;
  in = &ctx->input;
  in->mouse.pos.x = (float)x;
  in->mouse.pos.y = (float)y;
  in->mouse.delta.x = in->mouse.pos.x - in->mouse.prev.x;
  in->mouse.delta.y = in->mouse.pos.y - in->mouse.prev.y;
}
NK_API void nk_input_key(nk_context* ctx, nk_keys key, nk_bool down) {
  nk_input* in;
  NK_ASSERT(ctx);
  if (!ctx)
    return;
  in = &ctx->input;
#ifdef NK_KEYSTATE_BASED_INPUT
  if (in->keyboard.keys[key].down != down)
    in->keyboard.keys[key].clicked++;
#else
  in->keyboard.keys[key].clicked++;
#endif
  in->keyboard.keys[key].down = down;
}
NK_API void nk_input_button(nk_context* ctx, nk_buttons id, int x, int y, nk_bool down) {
  nk_mouse_button* btn;
  nk_input* in;
  NK_ASSERT(ctx);
  if (!ctx)
    return;
  in = &ctx->input;
  if (in->mouse.buttons[id].down == down)
    return;

  btn = &in->mouse.buttons[id];
  btn->clicked_pos.x = (float)x;
  btn->clicked_pos.y = (float)y;
  btn->down = down;
  btn->clicked++;
}
NK_API void nk_input_scroll(nk_context* ctx, nk_vec2 val) {
  NK_ASSERT(ctx);
  if (!ctx)
    return;
  ctx->input.mouse.scroll_delta.x += val.x;
  ctx->input.mouse.scroll_delta.y += val.y;
}
NK_API void nk_input_glyph(nk_context* ctx, const nk_glyph glyph) {
  int len = 0;
  nk_rune unicode;
  nk_input* in;

  NK_ASSERT(ctx);
  if (!ctx)
    return;
  in = &ctx->input;

  len = nk_utf_decode(glyph, &unicode, NK_UTF_SIZE);
  if (len && ((in->keyboard.text_len + len) < NK_INPUT_MAX)) {
    nk_utf_encode(unicode, &in->keyboard.text[in->keyboard.text_len], NK_INPUT_MAX - in->keyboard.text_len);
    in->keyboard.text_len += len;
  }
}
NK_API void nk_input_char(nk_context* ctx, char c) {
  nk_glyph glyph;
  NK_ASSERT(ctx);
  if (!ctx)
    return;
  glyph[0] = c;
  nk_input_glyph(ctx, glyph);
}
NK_API void nk_input_unicode(nk_context* ctx, nk_rune unicode) {
  nk_glyph rune;
  NK_ASSERT(ctx);
  if (!ctx)
    return;
  nk_utf_encode(unicode, rune, NK_UTF_SIZE);
  nk_input_glyph(ctx, rune);
}
NK_API nk_bool
nk_input_has_mouse_click(const nk_input* i, nk_buttons id) {
  const nk_mouse_button* btn;
  if (!i)
    return nk_false;
  btn = &i->mouse.buttons[id];
  return (btn->clicked && btn->down == nk_false) ? nk_true : nk_false;
}
NK_API nk_bool
nk_input_has_mouse_click_in_rect(const nk_input* i, nk_buttons id,
                                 nk_rect b) {
  const nk_mouse_button* btn;
  if (!i)
    return nk_false;
  btn = &i->mouse.buttons[id];
  if (!NK_INBOX(btn->clicked_pos.x, btn->clicked_pos.y, b.x, b.y, b.w, b.h))
    return nk_false;
  return nk_true;
}
NK_API nk_bool
nk_input_has_mouse_click_down_in_rect(const nk_input* i, nk_buttons id,
                                      nk_rect b, nk_bool down) {
  const nk_mouse_button* btn;
  if (!i)
    return nk_false;
  btn = &i->mouse.buttons[id];
  return nk_input_has_mouse_click_in_rect(i, id, b) && (btn->down == down);
}
NK_API nk_bool
nk_input_is_mouse_click_in_rect(const nk_input* i, nk_buttons id,
                                nk_rect b) {
  const nk_mouse_button* btn;
  if (!i)
    return nk_false;
  btn = &i->mouse.buttons[id];
  return (nk_input_has_mouse_click_down_in_rect(i, id, b, nk_false) &&
          btn->clicked) ?
             nk_true :
             nk_false;
}
NK_API nk_bool
nk_input_is_mouse_click_down_in_rect(const nk_input* i, nk_buttons id,
                                     nk_rect b, nk_bool down) {
  const nk_mouse_button* btn;
  if (!i)
    return nk_false;
  btn = &i->mouse.buttons[id];
  return (nk_input_has_mouse_click_down_in_rect(i, id, b, down) &&
          btn->clicked) ?
             nk_true :
             nk_false;
}
NK_API nk_bool
nk_input_any_mouse_click_in_rect(const nk_input* in, nk_rect b) {
  int i, down = 0;
  for (i = 0; i < NK_BUTTON_MAX; ++i)
    down = down || nk_input_is_mouse_click_in_rect(in, (nk_buttons)i, b);
  return down;
}
NK_API nk_bool
nk_input_is_mouse_hovering_rect(const nk_input* i, nk_rect rect) {
  if (!i)
    return nk_false;
  return NK_INBOX(i->mouse.pos.x, i->mouse.pos.y, rect.x, rect.y, rect.w, rect.h);
}
NK_API nk_bool
nk_input_is_mouse_prev_hovering_rect(const nk_input* i, nk_rect rect) {
  if (!i)
    return nk_false;
  return NK_INBOX(i->mouse.prev.x, i->mouse.prev.y, rect.x, rect.y, rect.w, rect.h);
}
NK_API nk_bool
nk_input_mouse_clicked(const nk_input* i, nk_buttons id, nk_rect rect) {
  if (!i)
    return nk_false;
  if (!nk_input_is_mouse_hovering_rect(i, rect))
    return nk_false;
  return nk_input_is_mouse_click_in_rect(i, id, rect);
}
NK_API nk_bool
nk_input_is_mouse_down(const nk_input* i, nk_buttons id) {
  if (!i)
    return nk_false;
  return i->mouse.buttons[id].down;
}
NK_API nk_bool
nk_input_is_mouse_pressed(const nk_input* i, nk_buttons id) {
  const nk_mouse_button* b;
  if (!i)
    return nk_false;
  b = &i->mouse.buttons[id];
  if (b->down && b->clicked)
    return nk_true;
  return nk_false;
}
NK_API nk_bool
nk_input_is_mouse_released(const nk_input* i, nk_buttons id) {
  if (!i)
    return nk_false;
  return (!i->mouse.buttons[id].down && i->mouse.buttons[id].clicked);
}
NK_API nk_bool
nk_input_is_key_pressed(const nk_input* i, nk_keys key) {
  const nk_key* k;
  if (!i)
    return nk_false;
  k = &i->keyboard.keys[key];
  if ((k->down && k->clicked) || (!k->down && k->clicked >= 2))
    return nk_true;
  return nk_false;
}
NK_API nk_bool
nk_input_is_key_released(const nk_input* i, nk_keys key) {
  const nk_key* k;
  if (!i)
    return nk_false;
  k = &i->keyboard.keys[key];
  if ((!k->down && k->clicked) || (k->down && k->clicked >= 2))
    return nk_true;
  return nk_false;
}
NK_API nk_bool
nk_input_is_key_down(const nk_input* i, nk_keys key) {
  const nk_key* k;
  if (!i)
    return nk_false;
  k = &i->keyboard.keys[key];
  if (k->down)
    return nk_true;
  return nk_false;
}
