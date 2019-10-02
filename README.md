# lua-5.3.5

[![Build Status](https://travis-ci.com/zhyingkun/lua-5.3.5.svg)](https://travis-ci.com/zhyingkun/lua-5.3.5)

---

## 基本介绍

1. 本工程是 Lua 及其 C 扩展的构建工程，用于运行调试
2. 所有 Lua 源码来自 lua 官方网站[https://www.lua.org/]
3. 增加了源码注释，支持 Debug/Release 编译模式
4. 整个工程 PC 端编译构建采用 cmake 来管理，支持跨平台（可以在树莓派上正常 cmake+make）
5. 移动端 iOS 直接给出 Xcode 工程，Android 则提供 Android.md 用于 ndk-build（仅限 luawithlib 库）

---

## 如何编译

##### 1. 在 Mac 上采用 Xcode 编译

```bash
cd lua-5.3.5/
mkdir buildXcode && cd buildXcode
cmake -DCMAKE_INSTALL_PREFIX=./install -G "Xcode" ..
# cmake -DCMAKE_INSTALL_PREFIX=/usr/local/zyk -G "Xcode" ..
```

此时已经在 buildXcode 文件夹下生成了 Xcode 工程，直接打开并编译即可

##### 2. 直接命令行编译（支持 Mac 和 Linux）

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

##### 3. 在 Windows 上使用 Cygwin + Visual Studio 2017 进行编译

```bash
cd lua-5.3.5/
mkdir buildVS && cd buildVS
cmake -DCMAKE_INSTALL_PREFIX=./install -G "Visual Studio 15 2017 Win64" ..
```

此时已经在 buildVS 文件夹下生成了 Visual Studio 工程，双击打开并编译即可

##### 4. 在 Mac 上编译出用于 iOS 的 libluawithlib.a 静态库（直接 Xcode 打开 luawithlib 工程文件）

```bash
cd lua-5.3.5/luawithlib/iOS
open luawithlib.xcodeproj
```

编译该 iOS Xcode 工程即可得到 libluawithlib.a 库

##### 5. 使用 NDK 编译出用于 Android 的 libluawithlib.so 共享库

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

1. include：Lua5.3.5 源码中的头文件单独放到一个文件夹中
2. lualib：Lua 编译器和虚拟机的源码，称为 Lua 库
3. lua：Lua 官方命令行工具，带编译器和虚拟机，依赖 Lua 库
4. luac：Lua 官方命令行编译器
5. cmod：Lua 的 C 语言扩展模块
   - hello：helloworld
   - luanet：将 Lua 虚拟机嵌入 UnityC#的中间层
   - luasocket：封装了 socket 接口，代码来自[https://github.com/diegonehab/luasocket]
   - create-lua-cmod.sh 和 delete-lua-cmod.sh：shell 脚本用于快速构建 Lua C 模块
6. etc：工程杂项
   - fromlua：来自 lua 官方的相关文件，包括 lua 文档，留着方便查看
   - vscodeMac：Mac 上 VSCode 的配置文件
7. luawithlib：包含 lua 和 luaC 模块的运行时动态库（iOS 下是静态库）
8. demo：用于测试的 Demo
   - c-call-lua：C 作为宿主，调用 Lua 来完成操作
   - c-lang：C 语言特性示例
9. 所有 CMakeLists.txt：用于构建整个工程
