# lua-5.3.5

[![Build Status](https://travis-ci.com/zhyingkun/lua-5.3.5.svg)](https://travis-ci.com/zhyingkun/lua-5.3.5)

---

## 基本介绍

1. 本工程是 Lua 及其 C 扩展的构建工程，用于运行调试
2. 所有 Lua 源码来自[lua 官方网站](https://www.lua.org/)
3. 增加了源码注释，扩展了一些功能，支持 Debug/Release 编译模式
4. 封装了一些 C 扩展模块，所有模块都以源码形式存在于工程中，仅依赖系统库，不依赖第三方库
5. 整个工程 PC 端编译构建采用 cmake 来管理，支持跨平台（可以在树莓派上正常 cmake+make）
6. 移动端 iOS 直接给出 Xcode 工程，Android 则提供 Android.mk 用于 ndk-build（仅限 luawithlib 库）

---

## 如何编译

#### 1. 在 Mac 上采用 Xcode 编译

```bash
cd lua-5.3.5/
cmake -DCMAKE_INSTALL_PREFIX=./install -G "Xcode" -S . -B buildXcode
# cmake -DCMAKE_INSTALL_PREFIX=/usr/local/zyk/lua -G "Xcode" -S . -B buildXcode
# for some cmake version: -DCMAKE_OSX_ARCHITECTURES=x86_64
```

此时已经在 buildXcode 文件夹下生成了 Xcode 工程，直接打开并编译即可

#### 2. 直接命令行编译（支持 Mac 和 Linux）

```bash
cd lua-5.3.5/
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=./install .. # default is Debug
# for Debug: cmake -DCMAKE_BUILD_TYPE=Debug ..
# for Release: cmake -DCMAKE_BUILD_TYPE=Release ..
# cmake -DCMAKE_INSTALL_PREFIX=/usr/local/zyk/lua -DCMAKE_BUILD_TYPE=Release ..
make
# for more details: make VERBOSE=1
make install
```

make 命令会自动编译好各个模块

#### 3. 在 Windows 上使用 Visual Studio 2017 进行编译

```bash
# In Cygwin
cd lua-5.3.5/
cmake -DCMAKE_INSTALL_PREFIX=./install -G "Visual Studio 15 2017 Win64" -S . -B buildVS
# cmake -DCMAKE_INSTALL_PREFIX=D:/Applications/zyk/lua -G "Visual Studio 15 2017 Win64" -S . -B buildVS
```

非 Cygwin 可以使用 GUI 版本的 CMake 来生成 Visual Studio 工程

在 buildVS 文件夹下生成了 Visual Studio 工程后，双击打开并编译 ALL_BUILD 目标

Windows 环境下需要将 CMAKE_INSTALL_PREFIX 设置路径下 bin 文件夹加入系统 path 环境变量，以便在 cmd 命令行中能调用到

执行 INSTALL 目标后所有 exe、dll 和 .h 头文件 都会安装到 CMAKE_INSTALL_PREFIX 设置路径下的对应位置，lua 运行时可正常加载对应 dll，无需手动设置 package.cpath

#### 4. 在 Mac 上编译出用于 iOS 的 libluawithlib.a 静态库（直接 Xcode 打开 luawithlib 工程文件）

```bash
cd lua-5.3.5/luawithlib/iOS
open luawithlib.xcodeproj
```

编译该 iOS Xcode 工程即可得到 libluawithlib.a 库

#### 5. 使用 NDK 编译出用于 Android 的 libluawithlib.so 共享库

```bash
cd lua-5.3.5/luawithlib/Android/
ndk-build
ndk-build clean # clean project
ndk-build -B # rebuild project
```

编译之后便会有 luawithlib/Android/libs/\${APP_ABI}/libluawithlib.so 共享库

使用 ndk-build 命令需要先安装 AndroidStudio+AndroidSDK+NDK，然后将 ndk-bundle 路径加到系统 PATH 环境变量中

---

## 文件夹说明

1. cmod: Lua 的 C 语言扩展模块
    - bcfx: 异步渲染库，纯 C 实现，相关设计参考了[bgfx](https://github.com/bkaradzic/bgfx)
    - boolarray: 《Lua 程序设计》中的布尔数组，加了数组的交集和并集操作
    - dir: 《Lua 程序设计》中的遍历文件夹，加了 Win 版实现
    - gb2312: 类似 utf8 标准库实现的 gb2312 模块
    - glfwrap: 封装了 glfw 库，用于支持系统窗口操作，基础代码来自[glfw](https://www.glfw.org)
    - hello: helloworld
    - json: 封装了 JSON 模块，底层使用 cJSON，支持 Table 与 JSON 字符串互转
    - lproc: 《Lua 程序设计》中的多线程模块，使用 pthread
    - luasocket: 封装了 socket 接口，代码来自[LuaSocket](https://github.com/diegonehab/luasocket)
    - protobuf: C 语言实现的 protobuf，代码来自[pbc](https://github.com/cloudwu/pbc)
    - uvwrap: 封装了 libuv 库，用于支持事件驱动的异步 IO，基础代码来自[libuv](http://libuv.org)
2. demo: 用于测试的 Demo
    - c-call-lua: C 作为宿主，调用 Lua 来完成操作
    - c-lang: C 语言特性示例
    - glfwrap: glfw 特性示例 Lua 版
    - lpg: 《Lua Programming Gems》中的一部分案例代码
    - oglib: OpenGL 渲染的示例
    - pbdemo: 使用 ProtocolBuffer 的示例，包含协议示例
    - pil: 《Lua 程序设计》中的一部分示例代码
    - runner: Lua 环境的独立运行工具
    - scripts: 用于测试的 Lua 脚本
    - stdlib: 用 Lua 语言实现的部分 Lua 标准库函数
    - triangle: bcfx 渲染库示例
    - uvwrap: libuv 示例 Lua 版
3. etc: 工程杂项
    - fromlua: 来自 Lua 官方的相关文件，包括 Lua 文档，留着方便查看
    - logos: Lua 官方图标
    - vscodeMac: Mac 上 VSCode 的配置文件
    - win: 用于 Windows 上的系统运行时补丁
4. liblua: Lua 编译器和虚拟机的源码，称为 Lua 库
    - core: Lua 核心 C 代码和 C 头文件
    - include: Lua 源码中的外部使用头文件单独放到一个文件夹中
    - libraries: Lua 自带的官方标准库
    - test: 扩展 Lua 功能的测试代码
    - util: 功能杂项
5. lmod: Lua 语言实现的工具模块
    - common: 通用功能
    - protobuf: protobuf 模块的 Lua 封装
    - reload: reload 模块的 Lua 封装
6. lua: Lua 官方命令行工具，带编译器和虚拟机，依赖 Lua 库
7. luac: Lua 官方命令行编译器
8. luawithlib: 包含 lua 和 luaC 模块的运行时动态库（iOS 下是静态库）
    - Android: 安卓的 ndk-build 相关配置
    - iOS: Xcode 工程
    - MacOS: 用于生成 MacOSX 下运行的 bundle
9. tools: 相关工具
    - luatoken: 用于查看词法分析 Token 的整数类型对应的含义
    - luatt: 用于查看 Lua Tag Type 的整数类型对应的含义

---

## 让 luac 命令使用 liblua 动态库

luac 命令的官方实现中，使用了一些动态库没有导出的符号，因此只能通过静态链接的方式来编译 luac。不过实际上 luac 命令只是一个类似 lua 命令的应用，本工程通过如下修改让 luac 命令也使用 liblua 动态库：

1. liblua 动态库导出两个符号：luaP_opmodes、luaP_opnames
2. 用 lua_dump 函数代替 luaU_dump，其实底层都是使用 luaU_dump 函数
3. 去除 luac.c 中的宏定义：LUA_CORE， 作为一个使用 liblua 的应用程序，不需要此宏（Win 下不允许有此宏，否则有些符号找不到）
4. 修改 CMakeLists.txt 配置文件，使用 liblua 动态库，并为 Win 版本加入宏：LUA_BUILD_AS_DLL（任何使用 liblua 的应用都需要在 Win 版本中添加 LUA_BUILD_AS_DLL 宏）

其实在 MacOSX 中，不需要上述操作也可以直接将 luac 切换为使用动态库 liblua，做这些修改主要是针对 Linux 和 Windows 平台。

为了在 lua 命令的交互式模式中也能够查看 lua 函数原型的内容，我将 luac 中的相关函数移植到 lauxlib.c 中，定义 C API luaL_protoinfo，并使用此 API 实现 luac 中的相关功能。

---

## 扩展 Lua 功能

1. 非 Win 环境下，package.cpath 增加 "/usr/local/zyk/lua/cmods/5.3/?.so" 搜索路径
2. tostring 方法支持第二个参数，传入一个大于 0 的整数，用于打印 table 中的字段，数字大小代表打印深度（通过 luaL_tolstringex 实现）
3. 增加 debug.getspecialkeys 函数，该函数无参数，返回一个 table，table 中记录了 CLIBS 等 Lua 用到的特殊 key
4. 增加 debug.sizeofstruct 函数，无参数，返回一个 table，table 中记录了底层实现结构体的内存占用大小
5. 增加 debug.tablemem 函数，接收一个 table 作为参数，返回四个字段：该 table 占用内存大小、数组部分长度、以 2 为底哈希表部分长度的对数、哈希表部分是否为假节点
6. 增加 debug.getgcstate 函数，用于获取当前 GC 状态
7. 增加 \_G.typedetail 函数，用于获取数据类型细节，包括了 \_\_name 元字段
8. 针对函数 \_G.collectgarbage 添加新的选项 "onestep"， 用于仅走最最原始的一步 gc 操作
9. 增加 debug.protoinfo 函数，用于获取函数原型信息，接收三个参数：Lua 函数、是否递归、原型选项。其中，原型选项为字符串"hcklupz"，每一个字符代表打印一种信息，分别为：header、codes、constants、locals、upvalues、protos、zykstyle
10. 增加 os.sysname 字段，为一个预编译定义的字符串，用于获取当前系统类型
11. 针对函数 utf8.char 添加新的用法：第一个参数传入一个 Lua 闭包，每次执行该闭包返回一个 codepoint，返回 nil 表示结束
12. 针对函数 string.char 添加新的用法：第一个参数传入一个 Lua 闭包，每次执行该闭包传入一个索引并返回一个 ASCII 字符对应的 codepoint（其实支持 0 到 255 之间任意数字，就是 1 个字节的范围），返回 nil 表示结束
13. 增加 string.bytes 函数，是字节版的 utf.codes，用于遍历所有字节
14. 增加 file:fileno 函数，用于获取文件对象中的文件描述符
15. 整数字面量增加八进制和二进制的支持（0o70、0b10）
16. 增加 debug.upvalues 函数，接收一个闭包作为参数，配合泛型 for，用于遍历该闭包的所有 UpValue，每次返回一个索引、名称、值
17. 增加 debug.locals 函数，参数为(\[thread,\] func)，其中 func 可以为闭包，或者调用栈层次；配合泛型 for，用于遍历该闭包或调用栈的所有局部变量，每次返回一个索引、名称、值（包括固定参数、内部局部变量、可变参数）
18. 增加 string.escape 函数，接收一个字符串，返回一个转义之后的字符串，支持任意二进制数据的转义
19. 增加 string.isvar 函数，用于判断目标字符串是否为合法的 Lua 变量名
20. 全局表中增加 "NULL" 字段，值为 lightuserdata 类型的 NULL
21. 增加 io.readfile 和 io.writefile 两个函数，用于进行快速的全内容文件读写
22. 非 Win 环境下，package.path 增加 "/usr/local/zyk/lua/mod/lua/5.3/?.lua" 搜索路径；Win 环境下，package.path 增加 "(lua.exe 所在目录)/../mod/lua/5.3/?.lua" 搜索路径
23. 增加 debug.inject 函数，用于在运行时某个上下文中插入一段代码，底层通过 luaL_inject 来实现
24. Lua 命令的 arg 表增加 argc 和 argv 两个字段，值为 main 函数的参数
25. 增加 string 元表的\_\_div 元方法，用于方便路径拼接
26. 增加 atexit 函数，用于注册一个函数给 lua 命令退出之前调用
27. 增加 atrepl 函数，用于注册一个函数给 lua 交互式命令执行一句脚本之后调用
28. 扩展 math.tointeger，支持将 lightuserdata 强转为整数
29. 新增 \_G.printe 函数，用于输出到标准错误流
30. table 表中新增功能方法：create、copy、every、filter、indexOf、map、reduce、reverse、slice、splice
31. 新增 asm.closure 函数，支持直接编写 Lua 汇编构建闭包
32. Lua 命令新增'-a'参数，用于将后续所有参数传递给'-e'所执行的脚本，方便构建 lua 脚本命令行工具
33. 新增 \_G.cocall 函数，参数传递类似 pcall，用于在新的协程中调用传入的函数

---

## 扩展 C API

1. 增加 `luaL_tolstringex` 方法，用于支持快速查看 table 中的字段
2. 增加 `lua_getstackdepth` 方法，用于获取当前函数调用嵌套深度（并非链表长度，链表长度记录于 L->nci）
3. 导出 `lua_ident` 符号，以便在 Windows 下使用该字符串
4. 针对函数 `lua_gc` 的第二个参数(what)添加新的选项`LUA_GCONESTEP`，用于仅走最最原始的一步 gc 操作
5. 增加 `luaL_protoinfo` 方法，用于获取函数原型信息
6. 增加 `luaL_escape` 方法，用于转义字符串，支持任意二进制数据的转义
7. 增加 `luaL_isvar` 方法，用于判断目标字符串是否为合法的 Lua 变量名
8. 增加 `luaL_atexit` 方法，用于在 lua 命令执行完脚本之后自动执行一系列函数
9. 增加 `lua_pushstackvalue` 方法，用于支持访问当前 Lua 栈中的任意槽位
10. 增加 `luaL_ptraceback` 方法，用于调试时打印 Lua 调用栈
11. 增加 `luaL_pstack` 方法，用于调试时打印 Lua 栈槽内容
12. 增加 `luaL_inject` 方法，用于在运行时某个上下文中插入一段代码
13. 增加 `luaL_testudata_recursive` 方法，用于递归的测试某个对象是否对应的用户类型
14. 增加 `luaL_checkudata_recursive` 方法，递归测试用户类型失败则抛出异常
15. 增加 `luaL_atrepl` 方法，用于在 lua 交互式命令执行完一句脚本之后自动执行一系列函数
16. 增加 `luaL_Enum` 结构体、`luaL_newenum`、`luaL_newenum_r`，用于支持 C 模块快速注入枚举数值
17. 扩展 `lua_tointegerx` 函数，支持将 lightuserdata 强转为整数
18. 增加 `luaL_checklightuserdata`、`luaL_optlightuserdata`、`luaL_checkboolean`、`luaL_optboolean`，方便对函数参数进行检查
19. 增加 `lua_copytable` 方法，用来复制一个 Table，支持配置是否复制其键值对
20. 增加 `lua_reverse` 方法，翻转 Lua 栈中指定区间的内容顺序
21. 增加 `lua_newlclosure`、`lua_asmcode`、`lua_asmconstant`、`lua_asmupvalue`、`lua_asmproto`、`lua_opcodeint`，支持在 C 端直接编写 Lua 汇编，并构建对应的 Lua 闭包

---

## 问题记录

1. luawithlib 在 Android 平台上仅支持 AndroidAPI21 及以上的 SDK，因为 API20 及以下不支持 C 标准库中的 localeconv 函数。
2. Windows 平台不支持 RPATH，默认 CMake 将编译出来的 dll 和 exe 放到各自的目录，导致在 Visual Studio 下无法直接调试运行链接了 dll 的 exe，解决此问题有多种方法，包括配置注册表、PATH 环境变量、拷贝 dll 等，这里选择最简单的一种：配置 CMake 将编译出来的所有 dll 和 exe 放到统一的 Binaries 目录。
3. Xcode 中使用 Attach to process 进行调试会将阻塞于 readline 的目标进程(lua 命令)唤醒，没有字符输入，此时会被 Lua 识别成 EOF，该进程主动退出。因此，使用 attach 进行调试时需要先手动阻塞等待调试器的 attach（例如先调用一次 readline），另外，后续设置断点等操作也有可能造成进程的错误唤醒。比较彻底的做法是区分调试模式，在调试模式中遇到 EOF 不退出进程（lua 命令添加-d 参数，用于支持 debug attaching）。
