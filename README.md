#lua-5.3.5

##Description
Source Code are copy from http://www.lua.org/

##Directories

1. include : all header files from lua source code
2. lualib : the c files for lua library from lua source code, and the CMakeLists.txt that I made
3. lua : CMakeLists.txt and lua.c
4. luac : CMakeLists.txt and luac.c
5. cmod : C lang module for lua
6. src and doc : this dirs are come from lua source code(www.lua.org), and not use now

##Build System

using cmake, type the cmd below if you want to build the code

```bash
cd lua-5.3.5/
mkdir build && cd build && cmake ..
mkdir buildXcode && cd buildXcode && cmake -G "Xcode" ..
```
