# lua-5.3.5

----

## 基本介绍

1. 本工程是Lua及其C扩展的构建工程，用于运行调试
2. 所有Lua源码来自lua官方网站[http://www.lua.org/]
3. 支持Debug/Release编译模式
4. 整个工程PC端编译构建采用cmake来管理，支持跨平台（可以在树莓派上正常cmake+make）
5. 移动端iOS直接给出Xcode工程，Android则提供Android.md用于ndk-build（仅限luawithlib库）

----

## 如何编译

1. 在Mac上采用Xcode编译
```bash
cd lua-5.3.5/
mkdir buildXcode && cd buildXcode
cmake -G "Xcode" ..
```
此时已经在buildXcode文件夹下生成了Xcode工程，直接打开并编译即可

2. 直接命令行编译（支持Mac和Linux）
```bash
cd lua-5.3.5/
mkdir build && cd build
cmake .. # default is Debug
# for Debug: cmake -DCMAKE_BUILD_TYPE=Debug ..
# for Release: cmake -DCMAKE_BUILD_TYPE=Release ..
make
# for more details: make VERBOSE=1 
```
make命令会自动编译好各个模块

3. 在Windows上使用Cygwin + Visual Studio 2017进行编译
```bash
cd lua-5.3.5/
mkdir buildVS && cd buildVS
cmake -G "Visual Studio 15 2017 Win64" ..
```
此时已经在buildVS文件夹下生成了Visual Studio工程，双击打开并编译即可

4. 在Mac上编译出用于iOS的libluawithlib.a静态库（直接Xcode打开luawithlib工程文件）
```bash
cd lua-5.3.5/luawithlib/iOS
open luawithlib.xcodeproj
```
编译该iOS Xcode工程即可得到libluawithlib.a库

5. 使用NDK编译出用于Android的libluawithlib.so共享库
```bash
cd lua-5.1.5/luawithlib/Android/
ndk-build
```
编译之后便会有luawithlib/Android/libs/${APP_ABI}/libluawithlib.so共享库
使用ndk-build命令需要先安装AndroidStudio+AndroidSDK+NDK，然后将ndk-bundle路径加到系统PATH环境变量中
目前仅支持AndroidAPI21及以上的AndroidSDK，因为API20及以下不支持C标准库中的localeconv函数

----

## 文件夹说明

1. include：Lua5.3.5源码中的头文件单独放到一个文件夹中
2. lualib：Lua编译器和虚拟机的源码，称为Lua库
3. lua：Lua官方命令行工具，带编译器和虚拟机，依赖Lua库
4. luac：Lua官方命令行编译器
5. cmod：Lua的C语言扩展模块
	>a. hello：helloworld
	>b. lpeg：Lua模式匹配库，代码来自[http://www.inf.puc-rio.br/~roberto/lpeg/lpeg.html]
	>c. luabitop：位操作，代码来自[http://bitop.luajit.org/]
	>d. luacjson：让lua支持json格式，代码来自[https://github.com/mpx/lua-cjson]
	>e. luacstruct：让lua与C语言结构体交互，代码来自[http://www.inf.puc-rio.br/~roberto/struct/]
	>f. luasocket：封装了socket接口，代码来自[https://github.com/diegonehab/luasocket]
	>g. protobuf：序列化，代码来自[https://github.com/topameng/protoc-gen-lua]
	>h. create-lua-cmod.sh 和 delete-lua-cmod.sh：shell脚本用于快速构建Lua C模块
6. etc：工程杂项
	>a. fromlua：来自lua官方的相关文件，包括lua文档，留着方便查看
	>b. vscodeMac：Mac上VSCode的配置文件
7. luawithlib：包含lua和luaC模块的运行时动态库（iOS下是静态库）
8. demo：用于测试的Demo
	>a. c-call-lua：C作为宿主，调用Lua来完成操作
	>b. c-lang：C语言特性示例
9. 所有CMakeLists.txt：用于构建整个工程
