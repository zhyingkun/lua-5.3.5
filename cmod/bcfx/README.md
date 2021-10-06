# bcfx

1. 抽象层次和抽象接口参考了[bgfx](https://github.com/bkaradzic/bgfx)
2. 独立的渲染线程异步执行抽象渲染指令

## 问题记录

1. Windows 下不支持空`struct`定义
2. Windows 下需要针对不同的`OpenGL Context`创建不同的`vertex array object`
3. MacOSX 仅支持`OpenGL 3.2`或之上版本的`forward-compatible core profile context`，该模式下必须使用`vertex array object`来进行渲染
