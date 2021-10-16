# Xcode 下调试 Lua 代码

基本思路：在 IDE 中调试运行`lua`命令，并且加载 Debug 版的 cmod，进入 C 语言的调试环境！Lua 需要断点停下来的时候，在对应 Lua 代码位置调用一下某个无副作用的 C 函数（如`debug.gethook()`），在 C 语言对应函数中加个断点，用 C 语言的断点来调试 Lua 虚拟机的上下文

## 准备调试环境

1. 设置调试进程的`WorkingDirectory`为本工程的根目录
2. 相对于`WorkingDirectory`，将入口脚本设置为调试参数
3. 增加调试环境变量`LUA_INIT_5_3`，值为`@demo/scripts/debug.lua`（`LUA_INIT_5_3="@demo/scripts/debug.lua"`）

## 在命中 Xcode 断点的情况下，使用`call`指令调用如下函数来获取 LuaVM 信息

1. 将某个栈帧中的一个 value 复制到栈顶：`LUA_API int(lua_pushstackvalue)(lua_State* L, int level, int idx);`
2. 删除栈顶的一些内容：`LUA_API void(lua_settop)(lua_State* L, int idx);`
3. 打印堆栈信息：`LUALIB_API void(luaL_ptraceback)(lua_State* L);`
4. 打印某个栈帧：`LUALIB_API void(luaL_pstack)(lua_State* L, int level);`，`level`传入`-1`表示打印所有栈帧
5. 打印栈帧深度：`LUALIB_API void(luaL_pstackdepth)(lua_State* L);`
6. 打印某个栈帧的某个局部变量：`LUALIB_API void(luaL_plocal)(lua_State* L, int level);`
7. 打印某个栈帧中一个 function 的某个 Upvalue：`LUALIB_API void(luaL_pupvalue)(lua_State* L, int level, int idx);`，`idx`等于`-1`表示使用该栈帧对应的执行函数
8. 打印某个栈帧中一个 Lua 函数的原型信息：`LUALIB_API void(luaL_pfuncinfo)(lua_State* L, int level, int idx, int recursive);`，`idx`等于`-1`表示使用该栈帧对应的执行函数
9. 在某个栈帧的运行上下文中执行一段 Lua 代码：`LUALIB_API void(luaL_pinject)(lua_State* L, const char* source, int level);`
10. 使用专门的断点函数设置给 Lua 的 hook：`LUALIB_API void(luaL_breakpoint)(lua_State* L);`

## 调试流程

1. 在想要断点停下的 Lua 代码前加一句`debug.gethook()`
2. 在`lua_gethook`添加 Xcode 断点（如果想一开始就停下来，可以在`pmain`处设置断点）
3. 运行 Lua 命令执行脚本
4. 命中断点之后，执行指令：`call luaL_breakpoint(L)`
5. 在`hook_virtual`函数的对应类型位置添加 Xcode 断点
6. 此时继续运行程序，可实现类似单步调试 Lua 代码的功能
7. 在此期间也可以使用`call`指令调用其他函数进行 debug
