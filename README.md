# lua-5.3.5

[![Build Status](https://travis-ci.com/zhyingkun/lua-5.3.5.svg)](https://travis-ci.com/zhyingkun/lua-5.3.5)

---

## 基本介绍

1. 本工程是 Lua 及其 C 扩展的构建工程，用于运行调试
2. 所有 Lua 源码来自 lua 官方网站[https://www.lua.org/]
3. 增加了源码注释，扩展了一些功能，支持 Debug/Release 编译模式
4. 整个工程 PC 端编译构建采用 cmake 来管理，支持跨平台（可以在树莓派上正常 cmake+make）
5. 移动端 iOS 直接给出 Xcode 工程，Android 则提供 Android.md 用于 ndk-build（仅限 luawithlib 库）

---

## 如何编译

#### 1. 在 Mac 上采用 Xcode 编译

```bash
cd lua-5.3.5/
mkdir buildXcode && cd buildXcode
cmake -DCMAKE_INSTALL_PREFIX=./install -G "Xcode" ..
# cmake -DCMAKE_INSTALL_PREFIX=/usr/local/zyk -G "Xcode" ..
```

此时已经在 buildXcode 文件夹下生成了 Xcode 工程，直接打开并编译即可

#### 2. 直接命令行编译（支持 Mac 和 Linux）

```bash
cd lua-5.3.5/
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=./install .. # default is Debug
# for Debug: cmake -DCMAKE_BUILD_TYPE=Debug ..
# for Release: cmake -DCMAKE_BUILD_TYPE=Release ..
make
# for more details: make VERBOSE=1
make install
```

make 命令会自动编译好各个模块

#### 3. 在 Windows 上使用 Visual Studio 2017 进行编译

```bash
# In Cygwin
cd lua-5.3.5/
mkdir buildVS && cd buildVS
cmake -DCMAKE_INSTALL_PREFIX=./install -G "Visual Studio 15 2017 Win64" ..
```

非 Cygwin 可以使用 CMake 的 GUI 界面来生成 Visual Studio 工程
在 buildVS 文件夹下生成了 Visual Studio 工程后，双击打开并编译 ALL_BUILD 目标
Windows 环境下需要将 CMAKE_INSTALL_PREFIX 设置路径下 bin 文件夹加入系统 path 环境变量，以便在 cmd 命令行中能调用到
执行 INSTALL 目标后所有 exe、dll 和 .h 头文件 都会安装到 CMAKE_INSTALL_PREFIX 设置路径下的对应位置，lua 运行时可正常加载对应 dll，无需手动设置 package.cpath
目前在 Visual Studio 下无法直接调试运行链接了 dll 的 exe，执行 lua 等命令行需要执行上述命令行之后在 CMD 中运行，demo 下的 exe 则需要复制 liblua 编译出来的 lua.dll 到对应的 exe 目录下

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
目前仅支持 AndroidAPI21 及以上的 AndroidSDK，因为 API20 及以下不支持 C 标准库中的 localeconv 函数

---

## 文件夹说明

1. cmod：Lua 的 C 语言扩展模块
   - hello：helloworld
   - luanet：将 Lua 虚拟机嵌入 UnityC# 的中间层
   - luasocket：封装了 socket 接口，代码来自[LuaSocket](https://github.com/diegonehab/luasocket)
2. demo：用于测试的 Demo
   - c-call-lua：C 作为宿主，调用 Lua 来完成操作
   - c-lang：C 语言特性示例
   - stdlib：用 Lua 语言实现的部分 Lua 标准库函数
3. etc：工程杂项
   - fromlua：来自 lua 官方的相关文件，包括 lua 文档，留着方便查看
   - vscodeMac：Mac 上 VSCode 的配置文件
4. liblua：Lua 编译器和虚拟机的源码，称为 Lua 库
   - core：Lua 核心 C 代码和 C 头文件
   - include：Lua 源码中的外部使用头文件单独放到一个文件夹中
   - libraries：Lua 自带的官方标准库
   - test：扩展 Lua 功能的测试代码
5. lua：Lua 官方命令行工具，带编译器和虚拟机，依赖 Lua 库
6. luac：Lua 官方命令行编译器
7. luawithlib：包含 lua 和 luaC 模块的运行时动态库（iOS 下是静态库）
   - Android：安卓的 ndk-build 相关配置
   - iOS：Xcode 工程
   - MacOS：用于生成 MacOSX 下运行的 bundle
8. tools：相关工具
   - luatt：用于查看 Lua Tag Type 的整数类型对应的含义

---

## 扩展 Lua 功能

1. 非 Win 环境下，package.cpath 增加 "/usr/local/zyk/lib/lua/5.3/?.so" 搜索路径
2. tostring 方法支持第二个参数，传入一个大于 0 的整数，用于打印 table 中的字段，数字大小代表打印深度（通过 luaL_tolstringex 实现）
3. 增加 debug.getspecialkeys 函数，该函数无参数，返回一个 table，table 中记录了 CLIBS 等 Lua 用到的特殊 key
4. 增加 debug.sizeofstruct 函数，无参数，返回一个 table，table 中记录了底层实现结构体的内存占用大小
5. 增加 debug.tablemem 函数，接收一个 table 作为参数，返回四个字段：该 table 占用内存大小、数组部分长度、以 2 为底哈希表部分长度的对数、哈希表部分是否为假节点
6. 增加 debug.getgcstate 函数，用于获取当前 GC 状态

---

## 扩展 C API

1. 增加 luaL_tolstringex 方法，用于支持快速查看 table 中的字段
2. 增加 lua_getstackdepth 方法，用于获取当前函数调用嵌套深度（并非链表长度，链表长度记录于 L->nci）
