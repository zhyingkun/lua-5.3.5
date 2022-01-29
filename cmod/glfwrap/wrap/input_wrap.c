/* Lua C Library */

#define input_wrap_c

#include <glfwrap.h>

/*
** {======================================================
** Input
** =======================================================
*/

static int GLFWRAP_FUNCTION(GetInputMode)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int mode = luaL_checkinteger(L, 2);
  int value = glfwGetInputMode(window, mode);
  lua_pushinteger(L, value);
  return 1;
}

static int GLFWRAP_FUNCTION(SetInputMode)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int mode = luaL_checkinteger(L, 2);
  int value = luaL_checkinteger(L, 3);
  glfwSetInputMode(window, mode, value);
  return 0;
}

static int GLFWRAP_FUNCTION(RawMouseMotionSupported)(lua_State* L) {
  int ret = glfwRawMouseMotionSupported();
  lua_pushboolean(L, ret);
  return 1;
}

static int GLFWRAP_FUNCTION(GetKeyName)(lua_State* L) {
  int key = luaL_checkinteger(L, 1);
  int scancode = luaL_checkinteger(L, 2);
  const char* name = glfwGetKeyName(key, scancode);
  lua_pushstring(L, name);
  return 1;
}

static int GLFWRAP_FUNCTION(GetKeyScancode)(lua_State* L) {
  int key = luaL_checkinteger(L, 1);
  int scancode = glfwGetKeyScancode(key);
  lua_pushinteger(L, scancode);
  return 1;
}

static int GLFWRAP_FUNCTION(GetKey)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int key = luaL_checkinteger(L, 2);
  int ret = glfwGetKey(window, key);
  lua_pushinteger(L, ret);
  return 1;
}

static int GLFWRAP_FUNCTION(GetMouseButton)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  int button = luaL_checkinteger(L, 2);
  int ret = glfwGetMouseButton(window, button);
  lua_pushinteger(L, ret);
  return 1;
}

static int GLFWRAP_FUNCTION(GetCursorPos)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  double xpos = 0.0;
  double ypos = 0.0;
  glfwGetCursorPos(window, &xpos, &ypos);
  lua_pushnumber(L, xpos);
  lua_pushnumber(L, ypos);
  return 2;
}

static int GLFWRAP_FUNCTION(SetCursorPos)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  double xpos = luaL_checknumber(L, 2);
  double ypos = luaL_checknumber(L, 3);
  glfwSetCursorPos(window, xpos, ypos);
  return 0;
}

static int GLFWRAP_FUNCTION(CreateCursor)(lua_State* L) {
  const GLFWimage* image = luaL_checkGLFWimage(L, 1);
  int xhot = luaL_checkinteger(L, 2);
  int yhot = luaL_checkinteger(L, 3);
  GLFWcursor* cursor = glfwCreateCursor(image, xhot, yhot);
  PUSH_LIGHTUSERDATA(L, (void*)cursor);
  return 1;
}

static int GLFWRAP_FUNCTION(CreateStandardCursor)(lua_State* L) {
  int shape = luaL_checkinteger(L, 1);
  GLFWcursor* cursor = glfwCreateStandardCursor(shape);
  PUSH_LIGHTUSERDATA(L, (void*)cursor);
  return 1;
}

static int GLFWRAP_FUNCTION(DestroyCursor)(lua_State* L) {
  GLFWcursor* cursor = luaL_checkGLFWcursor(L, 1);
  glfwDestroyCursor(cursor);
  return 0;
}

static int GLFWRAP_FUNCTION(SetCursor)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  GLFWcursor* cursor = luaL_checkGLFWcursor(L, 2);
  glfwSetCursor(window, cursor);
  return 0;
}

static void GLFWRAP_CALLBACK(SetKeyCallback)(GLFWwindow* window, int key, int scancode, int action, int mods) {
  WINDOW_CALLBACK_BEGIN(SetKeyCallback, key)
  PUSH_LIGHTUSERDATA(L, window);
  lua_pushinteger(L, key);
  lua_pushinteger(L, scancode);
  lua_pushinteger(L, action);
  lua_pushinteger(L, mods);
  CALL_LUA_FUNCTION(L, 5);
  WINDOW_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetKeyCallback)(lua_State* L) {
  SET_WINDOW_CALLBACK(SetKeyCallback, key);
  return 0;
}

static void GLFWRAP_CALLBACK(SetCharCallback)(GLFWwindow* window, unsigned int codepoint) {
  WINDOW_CALLBACK_BEGIN(SetCharCallback, char)
  PUSH_LIGHTUSERDATA(L, window);
  lua_pushinteger(L, codepoint);
  CALL_LUA_FUNCTION(L, 2);
  WINDOW_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetCharCallback)(lua_State* L) {
  SET_WINDOW_CALLBACK(SetCharCallback, char);
  return 0;
}

static void GLFWRAP_CALLBACK(SetCharModsCallback)(GLFWwindow* window, unsigned int codepoint, int mods) {
  WINDOW_CALLBACK_BEGIN(SetCharModsCallback, charmods)
  PUSH_LIGHTUSERDATA(L, window);
  lua_pushinteger(L, codepoint);
  lua_pushinteger(L, mods);
  CALL_LUA_FUNCTION(L, 3);
  WINDOW_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetCharModsCallback)(lua_State* L) {
  SET_WINDOW_CALLBACK(SetCharModsCallback, charmods);
  return 0;
}

static void GLFWRAP_CALLBACK(SetMouseButtonCallback)(GLFWwindow* window, int button, int action, int mods) {
  WINDOW_CALLBACK_BEGIN(SetMouseButtonCallback, mousebutton)
  PUSH_LIGHTUSERDATA(L, window);
  lua_pushinteger(L, button);
  lua_pushinteger(L, action);
  lua_pushinteger(L, mods);
  CALL_LUA_FUNCTION(L, 4);
  WINDOW_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetMouseButtonCallback)(lua_State* L) {
  SET_WINDOW_CALLBACK(SetMouseButtonCallback, mousebutton);
  return 0;
}

static void GLFWRAP_CALLBACK(SetCursorPosCallback)(GLFWwindow* window, double xpos, double ypos) {
  WINDOW_CALLBACK_BEGIN(SetCursorPosCallback, cursorpos)
  PUSH_LIGHTUSERDATA(L, window);
  lua_pushnumber(L, xpos);
  lua_pushnumber(L, ypos);
  CALL_LUA_FUNCTION(L, 3);
  WINDOW_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetCursorPosCallback)(lua_State* L) {
  SET_WINDOW_CALLBACK(SetCursorPosCallback, cursorpos);
  return 0;
}

static void GLFWRAP_CALLBACK(SetCursorEnterCallback)(GLFWwindow* window, int entered) {
  WINDOW_CALLBACK_BEGIN(SetCursorEnterCallback, cursorenter)
  PUSH_LIGHTUSERDATA(L, window);
  lua_pushboolean(L, entered);
  CALL_LUA_FUNCTION(L, 2);
  WINDOW_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetCursorEnterCallback)(lua_State* L) {
  SET_WINDOW_CALLBACK(SetCursorEnterCallback, cursorenter);
  return 0;
}

static void GLFWRAP_CALLBACK(SetScrollCallback)(GLFWwindow* window, double xoffset, double yoffset) {
  WINDOW_CALLBACK_BEGIN(SetScrollCallback, scroll)
  PUSH_LIGHTUSERDATA(L, window);
  lua_pushnumber(L, xoffset);
  lua_pushnumber(L, yoffset);
  CALL_LUA_FUNCTION(L, 3);
  WINDOW_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetScrollCallback)(lua_State* L) {
  SET_WINDOW_CALLBACK(SetScrollCallback, scroll);
  return 0;
}

static void GLFWRAP_CALLBACK(SetDropCallback)(GLFWwindow* window, int path_count, const char* paths[]) {
  WINDOW_CALLBACK_BEGIN(SetDropCallback, drop)
  PUSH_LIGHTUSERDATA(L, window);
  lua_createtable(L, path_count, 0);
  for (int i = 0; i < path_count; i++) {
    lua_pushstring(L, paths[i]);
    lua_rawseti(L, -2, i + 1);
  }
  CALL_LUA_FUNCTION(L, 2);
  WINDOW_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetDropCallback)(lua_State* L) {
  SET_WINDOW_CALLBACK(SetDropCallback, drop);
  return 0;
}

static int GLFWRAP_FUNCTION(JoystickPresent)(lua_State* L) {
  int jid = luaL_checkinteger(L, 1);
  int ret = glfwJoystickPresent(jid);
  lua_pushboolean(L, ret);
  return 1;
}

static int GLFWRAP_FUNCTION(GetJoystickAxes)(lua_State* L) {
  int jid = luaL_checkinteger(L, 1);
  int count = 0;
  const float* axes = glfwGetJoystickAxes(jid, &count);
  if (axes == NULL) {
    lua_pushnil(L);
  } else {
    lua_createtable(L, count, 0);
    for (int i = 0; i < count; i++) {
      lua_pushnumber(L, axes[i]);
      lua_rawseti(L, -2, i + 1);
    }
  }
  return 1;
}

static int GLFWRAP_FUNCTION(GetJoystickButtons)(lua_State* L) {
  int jid = luaL_checkinteger(L, 1);
  int count = 0;
  const unsigned char* buttons = glfwGetJoystickButtons(jid, &count);
  if (buttons == NULL) {
    lua_pushnil(L);
  } else {
    lua_createtable(L, count, 0);
    for (int i = 0; i < count; i++) {
      lua_pushinteger(L, buttons[i]);
      lua_rawseti(L, -2, i + 1);
    }
  }
  return 1;
}

static int GLFWRAP_FUNCTION(GetJoystickHats)(lua_State* L) {
  int jid = luaL_checkinteger(L, 1);
  int count = 0;
  const unsigned char* hats = glfwGetJoystickHats(jid, &count);
  if (hats == NULL) {
    lua_pushnil(L);
  } else {
    lua_createtable(L, count, 0);
    for (int i = 0; i < count; i++) {
      lua_pushinteger(L, hats[i]);
      lua_rawseti(L, -2, i + 1);
    }
  }
  return 1;
}

static int GLFWRAP_FUNCTION(GetJoystickName)(lua_State* L) {
  int jid = luaL_checkinteger(L, 1);
  const char* name = glfwGetJoystickName(jid);
  lua_pushstring(L, name);
  return 1;
}

static int GLFWRAP_FUNCTION(GetJoystickGUID)(lua_State* L) {
  int jid = luaL_checkinteger(L, 1);
  const char* guid = glfwGetJoystickGUID(jid);
  lua_pushstring(L, guid);
  return 1;
}

static int GLFWRAP_FUNCTION(JoystickIsGamepad)(lua_State* L) {
  int jid = luaL_checkinteger(L, 1);
  int ret = glfwJoystickIsGamepad(jid);
  lua_pushboolean(L, ret);
  return 1;
}

static lua_State* StaticL;
static void GLFWRAP_CALLBACK(SetJoystickCallback)(int jid, int event) {
  lua_State* L = StaticL;
  GLFWRAP_CALLBACK_BEGIN(SetJoystickCallback)
  lua_pushinteger(L, jid);
  lua_pushinteger(L, event);
  CALL_LUA_FUNCTION(L, 2);
  GLFWRAP_CALLBACK_END()
}
static int GLFWRAP_FUNCTION(SetJoystickCallback)(lua_State* L) {
  StaticL = L;
  SET_GLFWRAP_CALLBACK(SetJoystickCallback, joystick, 1);
  glfwSetJoystickCallback(callback);
  return 0;
}

static int GLFWRAP_FUNCTION(UpdateGamepadMappings)(lua_State* L) {
  const char* string = luaL_checkstring(L, 1);
  int ret = glfwUpdateGamepadMappings(string);
  lua_pushboolean(L, ret);
  return 1;
}

static int GLFWRAP_FUNCTION(GetGamepadName)(lua_State* L) {
  int jid = luaL_checkinteger(L, 1);
  const char* name = glfwGetGamepadName(jid);
  lua_pushstring(L, name);
  return 1;
}

static int GLFWRAP_FUNCTION(GetGamepadState)(lua_State* L) {
  int jid = luaL_checkinteger(L, 1);
  GLFWgamepadstate state = {0};
  int ret = glfwGetGamepadState(jid, &state);
  if (ret == GLFW_FALSE) {
    lua_pushnil(L);
  } else {
    lua_createtable(L, 0, 2);
    lua_createtable(L, 15, 0);
    for (int i = 0; i < 15; i++) {
      lua_pushinteger(L, state.buttons[i]);
      lua_rawseti(L, -2, i + 1);
    }
    lua_setfield(L, -2, "buttons");
    lua_createtable(L, 6, 0);
    for (int i = 0; i < 6; i++) {
      lua_pushnumber(L, state.axes[i]);
      lua_rawseti(L, -2, i + 1);
    }
    lua_setfield(L, -2, "axes");
  }
  return 1;
}

static int GLFWRAP_FUNCTION(SetClipboardString)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  const char* string = luaL_checkstring(L, 2);
  glfwSetClipboardString(window, string);
  return 0;
}

static int GLFWRAP_FUNCTION(GetClipboardString)(lua_State* L) {
  GLFWwindow* window = luaL_checkGLFWwindow(L, 1);
  const char* string = glfwGetClipboardString(window);
  lua_pushstring(L, string);
  return 1;
}

static int GLFWRAP_FUNCTION(GetTime)(lua_State* L) {
  double time = glfwGetTime();
  lua_pushnumber(L, time);
  return 1;
}

static int GLFWRAP_FUNCTION(SetTime)(lua_State* L) {
  double time = luaL_checknumber(L, 1);
  glfwSetTime(time);
  return 0;
}

static int GLFWRAP_FUNCTION(GetTimerValue)(lua_State* L) {
  uint64_t freq = glfwGetTimerValue();
  lua_pushinteger(L, freq);
  return 1;
}

static int GLFWRAP_FUNCTION(GetTimerFrequency)(lua_State* L) {
  uint64_t freq = glfwGetTimerFrequency();
  lua_pushinteger(L, freq);
  return 1;
}

/* }====================================================== */

static const luaL_Reg GLFWRAP_FUNCTION(input_funcs)[] = {
    /* Input */
    EMPLACE_GLFWRAP_FUNCTION(GetInputMode),
    EMPLACE_GLFWRAP_FUNCTION(SetInputMode),
    EMPLACE_GLFWRAP_FUNCTION(RawMouseMotionSupported),
    EMPLACE_GLFWRAP_FUNCTION(GetKeyName),
    EMPLACE_GLFWRAP_FUNCTION(GetKeyScancode),
    EMPLACE_GLFWRAP_FUNCTION(GetKey),
    EMPLACE_GLFWRAP_FUNCTION(GetMouseButton),
    EMPLACE_GLFWRAP_FUNCTION(GetCursorPos),
    EMPLACE_GLFWRAP_FUNCTION(SetCursorPos),
    EMPLACE_GLFWRAP_FUNCTION(CreateCursor),
    EMPLACE_GLFWRAP_FUNCTION(CreateStandardCursor),
    EMPLACE_GLFWRAP_FUNCTION(DestroyCursor),
    EMPLACE_GLFWRAP_FUNCTION(SetCursor),
    EMPLACE_GLFWRAP_FUNCTION(SetKeyCallback),
    EMPLACE_GLFWRAP_FUNCTION(SetCharCallback),
    EMPLACE_GLFWRAP_FUNCTION(SetCharModsCallback),
    EMPLACE_GLFWRAP_FUNCTION(SetMouseButtonCallback),
    EMPLACE_GLFWRAP_FUNCTION(SetCursorPosCallback),
    EMPLACE_GLFWRAP_FUNCTION(SetCursorEnterCallback),
    EMPLACE_GLFWRAP_FUNCTION(SetScrollCallback),
    EMPLACE_GLFWRAP_FUNCTION(SetDropCallback),
    EMPLACE_GLFWRAP_FUNCTION(JoystickPresent),
    EMPLACE_GLFWRAP_FUNCTION(GetJoystickAxes),
    EMPLACE_GLFWRAP_FUNCTION(GetJoystickButtons),
    EMPLACE_GLFWRAP_FUNCTION(GetJoystickHats),
    EMPLACE_GLFWRAP_FUNCTION(GetJoystickName),
    EMPLACE_GLFWRAP_FUNCTION(GetJoystickGUID),
    EMPLACE_GLFWRAP_FUNCTION(JoystickIsGamepad),
    EMPLACE_GLFWRAP_FUNCTION(SetJoystickCallback),
    EMPLACE_GLFWRAP_FUNCTION(UpdateGamepadMappings),
    EMPLACE_GLFWRAP_FUNCTION(GetGamepadName),
    EMPLACE_GLFWRAP_FUNCTION(GetGamepadState),
    EMPLACE_GLFWRAP_FUNCTION(SetClipboardString),
    EMPLACE_GLFWRAP_FUNCTION(GetClipboardString),
    EMPLACE_GLFWRAP_FUNCTION(GetTime),
    EMPLACE_GLFWRAP_FUNCTION(SetTime),
    EMPLACE_GLFWRAP_FUNCTION(GetTimerValue),
    EMPLACE_GLFWRAP_FUNCTION(GetTimerFrequency),
    {NULL, NULL},
};

static const luaL_Enum GLFWRAP_ENUM(gamepad_axis)[] = {
    {"LEFT_X", GLFW_GAMEPAD_AXIS_LEFT_X},
    {"LEFT_Y", GLFW_GAMEPAD_AXIS_LEFT_Y},
    {"RIGHT_X", GLFW_GAMEPAD_AXIS_RIGHT_X},
    {"RIGHT_Y", GLFW_GAMEPAD_AXIS_RIGHT_Y},
    {"LEFT_TRIGGER", GLFW_GAMEPAD_AXIS_LEFT_TRIGGER},
    {"RIGHT_TRIGGER", GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER},
    {"LAST", GLFW_GAMEPAD_AXIS_LAST},
    {NULL, 0},
};
static const luaL_Enum GLFWRAP_ENUM(gamepad_button)[] = {
    {"A", GLFW_GAMEPAD_BUTTON_A},
    {"B", GLFW_GAMEPAD_BUTTON_B},
    {"X", GLFW_GAMEPAD_BUTTON_X},
    {"Y", GLFW_GAMEPAD_BUTTON_Y},
    {"LEFT_BUMPER", GLFW_GAMEPAD_BUTTON_LEFT_BUMPER},
    {"RIGHT_BUMPER", GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER},
    {"BACK", GLFW_GAMEPAD_BUTTON_BACK},
    {"START", GLFW_GAMEPAD_BUTTON_START},
    {"GUIDE", GLFW_GAMEPAD_BUTTON_GUIDE},
    {"LEFT_THUMB", GLFW_GAMEPAD_BUTTON_LEFT_THUMB},
    {"RIGHT_THUMB", GLFW_GAMEPAD_BUTTON_RIGHT_THUMB},
    {"DPAD_UP", GLFW_GAMEPAD_BUTTON_DPAD_UP},
    {"DPAD_RIGHT", GLFW_GAMEPAD_BUTTON_DPAD_RIGHT},
    {"DPAD_DOWN", GLFW_GAMEPAD_BUTTON_DPAD_DOWN},
    {"DPAD_LEFT", GLFW_GAMEPAD_BUTTON_DPAD_LEFT},
    {"LAST", GLFW_GAMEPAD_BUTTON_LAST},
    {"CROSS", GLFW_GAMEPAD_BUTTON_CROSS},
    {"CIRCLE", GLFW_GAMEPAD_BUTTON_CIRCLE},
    {"SQUARE", GLFW_GAMEPAD_BUTTON_SQUARE},
    {"TRIANGLE", GLFW_GAMEPAD_BUTTON_TRIANGLE},
    {NULL, 0},
};
static const luaL_Enum GLFWRAP_ENUM(joystick_hat)[] = {
    {"CENTERED", GLFW_HAT_CENTERED},
    {"UP", GLFW_HAT_UP},
    {"RIGHT", GLFW_HAT_RIGHT},
    {"DOWN", GLFW_HAT_DOWN},
    {"LEFT", GLFW_HAT_LEFT},
    {"RIGHT_UP", GLFW_HAT_RIGHT_UP},
    {"RIGHT_DOWN", GLFW_HAT_RIGHT_DOWN},
    {"LEFT_UP", GLFW_HAT_LEFT_UP},
    {"LEFT_DOWN", GLFW_HAT_LEFT_DOWN},
    {NULL, 0},
};
static const luaL_Enum GLFWRAP_ENUM(joystick)[] = {
    {"_1", GLFW_JOYSTICK_1},
    {"_2", GLFW_JOYSTICK_2},
    {"_3", GLFW_JOYSTICK_3},
    {"_4", GLFW_JOYSTICK_4},
    {"_5", GLFW_JOYSTICK_5},
    {"_6", GLFW_JOYSTICK_6},
    {"_7", GLFW_JOYSTICK_7},
    {"_8", GLFW_JOYSTICK_8},
    {"_9", GLFW_JOYSTICK_9},
    {"_10", GLFW_JOYSTICK_10},
    {"_11", GLFW_JOYSTICK_11},
    {"_12", GLFW_JOYSTICK_12},
    {"_13", GLFW_JOYSTICK_13},
    {"_14", GLFW_JOYSTICK_14},
    {"_15", GLFW_JOYSTICK_15},
    {"_16", GLFW_JOYSTICK_16},
    {"LAST", GLFW_JOYSTICK_LAST},
    {NULL, 0},
};
static const luaL_Enum GLFWRAP_ENUM(keyboard)[] = {
    {"UNKNOWN", GLFW_KEY_UNKNOWN},
    {"SPACE", GLFW_KEY_SPACE},
    {"APOSTROPHE", GLFW_KEY_APOSTROPHE},
    {"COMMA", GLFW_KEY_COMMA},
    {"MINUS", GLFW_KEY_MINUS},
    {"PERIOD", GLFW_KEY_PERIOD},
    {"SLASH", GLFW_KEY_SLASH},
    {"_0", GLFW_KEY_0},
    {"_1", GLFW_KEY_1},
    {"_2", GLFW_KEY_2},
    {"_3", GLFW_KEY_3},
    {"_4", GLFW_KEY_4},
    {"_5", GLFW_KEY_5},
    {"_6", GLFW_KEY_6},
    {"_7", GLFW_KEY_7},
    {"_8", GLFW_KEY_8},
    {"_9", GLFW_KEY_9},
    {"SEMICOLON", GLFW_KEY_SEMICOLON},
    {"EQUAL", GLFW_KEY_EQUAL},
    {"A", GLFW_KEY_A},
    {"B", GLFW_KEY_B},
    {"C", GLFW_KEY_C},
    {"D", GLFW_KEY_D},
    {"E", GLFW_KEY_E},
    {"F", GLFW_KEY_F},
    {"G", GLFW_KEY_G},
    {"H", GLFW_KEY_H},
    {"I", GLFW_KEY_I},
    {"J", GLFW_KEY_J},
    {"K", GLFW_KEY_K},
    {"L", GLFW_KEY_L},
    {"M", GLFW_KEY_M},
    {"N", GLFW_KEY_N},
    {"O", GLFW_KEY_O},
    {"P", GLFW_KEY_P},
    {"Q", GLFW_KEY_Q},
    {"R", GLFW_KEY_R},
    {"S", GLFW_KEY_S},
    {"T", GLFW_KEY_T},
    {"U", GLFW_KEY_U},
    {"V", GLFW_KEY_V},
    {"W", GLFW_KEY_W},
    {"X", GLFW_KEY_X},
    {"Y", GLFW_KEY_Y},
    {"Z", GLFW_KEY_Z},
    {"LEFT_BRACKET", GLFW_KEY_LEFT_BRACKET},
    {"BACKSLASH", GLFW_KEY_BACKSLASH},
    {"RIGHT_BRACKET", GLFW_KEY_RIGHT_BRACKET},
    {"GRAVE_ACCENT", GLFW_KEY_GRAVE_ACCENT},
    {"WORLD_1", GLFW_KEY_WORLD_1},
    {"WORLD_2", GLFW_KEY_WORLD_2},
    {"ESCAPE", GLFW_KEY_ESCAPE},
    {"ENTER", GLFW_KEY_ENTER},
    {"TAB", GLFW_KEY_TAB},
    {"BACKSPACE", GLFW_KEY_BACKSPACE},
    {"INSERT", GLFW_KEY_INSERT},
    {"DELETE", GLFW_KEY_DELETE},
    {"RIGHT", GLFW_KEY_RIGHT},
    {"LEFT", GLFW_KEY_LEFT},
    {"DOWN", GLFW_KEY_DOWN},
    {"UP", GLFW_KEY_UP},
    {"PAGE_UP", GLFW_KEY_PAGE_UP},
    {"PAGE_DOWN", GLFW_KEY_PAGE_DOWN},
    {"HOME", GLFW_KEY_HOME},
    {"END", GLFW_KEY_END},
    {"CAPS_LOCK", GLFW_KEY_CAPS_LOCK},
    {"SCROLL_LOCK", GLFW_KEY_SCROLL_LOCK},
    {"NUM_LOCK", GLFW_KEY_NUM_LOCK},
    {"PRINT_SCREEN", GLFW_KEY_PRINT_SCREEN},
    {"PAUSE", GLFW_KEY_PAUSE},
    {"F1", GLFW_KEY_F1},
    {"F2", GLFW_KEY_F2},
    {"F3", GLFW_KEY_F3},
    {"F4", GLFW_KEY_F4},
    {"F5", GLFW_KEY_F5},
    {"F6", GLFW_KEY_F6},
    {"F7", GLFW_KEY_F7},
    {"F8", GLFW_KEY_F8},
    {"F9", GLFW_KEY_F9},
    {"F10", GLFW_KEY_F10},
    {"F11", GLFW_KEY_F11},
    {"F12", GLFW_KEY_F12},
    {"F13", GLFW_KEY_F13},
    {"F14", GLFW_KEY_F14},
    {"F15", GLFW_KEY_F15},
    {"F16", GLFW_KEY_F16},
    {"F17", GLFW_KEY_F17},
    {"F18", GLFW_KEY_F18},
    {"F19", GLFW_KEY_F19},
    {"F20", GLFW_KEY_F20},
    {"F21", GLFW_KEY_F21},
    {"F22", GLFW_KEY_F22},
    {"F23", GLFW_KEY_F23},
    {"F24", GLFW_KEY_F24},
    {"F25", GLFW_KEY_F25},
    {"KP_0", GLFW_KEY_KP_0},
    {"KP_1", GLFW_KEY_KP_1},
    {"KP_2", GLFW_KEY_KP_2},
    {"KP_3", GLFW_KEY_KP_3},
    {"KP_4", GLFW_KEY_KP_4},
    {"KP_5", GLFW_KEY_KP_5},
    {"KP_6", GLFW_KEY_KP_6},
    {"KP_7", GLFW_KEY_KP_7},
    {"KP_8", GLFW_KEY_KP_8},
    {"KP_9", GLFW_KEY_KP_9},
    {"KP_DECIMAL", GLFW_KEY_KP_DECIMAL},
    {"KP_DIVIDE", GLFW_KEY_KP_DIVIDE},
    {"KP_MULTIPLY", GLFW_KEY_KP_MULTIPLY},
    {"KP_SUBTRACT", GLFW_KEY_KP_SUBTRACT},
    {"KP_ADD", GLFW_KEY_KP_ADD},
    {"KP_ENTER", GLFW_KEY_KP_ENTER},
    {"KP_EQUAL", GLFW_KEY_KP_EQUAL},
    {"LEFT_SHIFT", GLFW_KEY_LEFT_SHIFT},
    {"LEFT_CONTROL", GLFW_KEY_LEFT_CONTROL},
    {"LEFT_ALT", GLFW_KEY_LEFT_ALT},
    {"LEFT_SUPER", GLFW_KEY_LEFT_SUPER},
    {"RIGHT_SHIFT", GLFW_KEY_RIGHT_SHIFT},
    {"RIGHT_CONTROL", GLFW_KEY_RIGHT_CONTROL},
    {"RIGHT_ALT", GLFW_KEY_RIGHT_ALT},
    {"RIGHT_SUPER", GLFW_KEY_RIGHT_SUPER},
    {"MENU", GLFW_KEY_MENU},
    {"LAST", GLFW_KEY_LAST},
    {NULL, 0},
};
static const luaL_Enum GLFWRAP_ENUM(modifier_key)[] = {
    {"SHIFT", GLFW_MOD_SHIFT},
    {"CONTROL", GLFW_MOD_CONTROL},
    {"ALT", GLFW_MOD_ALT},
    {"SUPER", GLFW_MOD_SUPER},
    {"CAPS_LOCK", GLFW_MOD_CAPS_LOCK},
    {"NUM_LOCK", GLFW_MOD_NUM_LOCK},
    {NULL, 0},
};
static const luaL_Enum GLFWRAP_ENUM(mouse_button)[] = {
    {"_1", GLFW_MOUSE_BUTTON_1},
    {"_2", GLFW_MOUSE_BUTTON_2},
    {"_3", GLFW_MOUSE_BUTTON_3},
    {"_4", GLFW_MOUSE_BUTTON_4},
    {"_5", GLFW_MOUSE_BUTTON_5},
    {"_6", GLFW_MOUSE_BUTTON_6},
    {"_7", GLFW_MOUSE_BUTTON_7},
    {"_8", GLFW_MOUSE_BUTTON_8},
    {"LAST", GLFW_MOUSE_BUTTON_LAST},
    {"LEFT", GLFW_MOUSE_BUTTON_LEFT},
    {"RIGHT", GLFW_MOUSE_BUTTON_RIGHT},
    {"MIDDLE", GLFW_MOUSE_BUTTON_MIDDLE},
    {NULL, 0},
};
static const luaL_Enum GLFWRAP_ENUM(standard_cursor_shape)[] = {
    {"ARROW", GLFW_ARROW_CURSOR},
    {"IBEAM", GLFW_IBEAM_CURSOR},
    {"CROSSHAIR", GLFW_CROSSHAIR_CURSOR},
    {"HAND", GLFW_HAND_CURSOR},
    {"HRESIZE", GLFW_HRESIZE_CURSOR},
    {"VRESIZE", GLFW_VRESIZE_CURSOR},
    {NULL, 0},
};
static const luaL_Enum GLFWRAP_ENUM(input_mode)[] = {
    {"CURSOR", GLFW_CURSOR},
    {"STICKY_KEYS", GLFW_STICKY_KEYS},
    {"STICKY_MOUSE_BUTTONS", GLFW_STICKY_MOUSE_BUTTONS},
    {"LOCK_KEY_MODS", GLFW_LOCK_KEY_MODS},
    {"RAW_MOUSE_MOTION", GLFW_RAW_MOUSE_MOTION},
    {NULL, 0},
};
static const luaL_Enum GLFWRAP_ENUM(cursor_mode)[] = {
    {"NORMAL", GLFW_CURSOR_NORMAL},
    {"HIDDEN", GLFW_CURSOR_HIDDEN},
    {"DISABLED", GLFW_CURSOR_DISABLED},
    {NULL, 0},
};
static const luaL_Enum GLFWRAP_ENUM(input_state)[] = {
    {"PRESS", GLFW_PRESS},
    {"RELEASE", GLFW_RELEASE},
    {"REPEAT", GLFW_REPEAT},
    {NULL, 0},
};

DEFINE_REGISTE_FUNC_BEGIN(input)
REGISTE_ENUM_GLFWRAP(gamepad_axis);
REGISTE_ENUM_GLFWRAP(gamepad_button);
REGISTE_ENUM_GLFWRAP(joystick_hat);
REGISTE_ENUM_GLFWRAP(joystick);
REGISTE_ENUM_GLFWRAP(keyboard);
REGISTE_ENUM_GLFWRAP(modifier_key);
REGISTE_ENUM_GLFWRAP(mouse_button);
REGISTE_ENUM_GLFWRAP(standard_cursor_shape);
REGISTE_ENUM_GLFWRAP(input_mode);
REGISTE_ENUM_GLFWRAP(cursor_mode);
REGISTE_ENUM_GLFWRAP(input_state);
DEFINE_REGISTE_FUNC_END()
