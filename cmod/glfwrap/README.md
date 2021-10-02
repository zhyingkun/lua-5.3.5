# glfw

1. glfw 创建的每个窗口默认会创建并绑定一个 OpenGL 的 Context，可通过设置 GLFW_CONTEXT_CREATION_API 告诉 glfw 不创建 Context
2. 一个窗口对应一套双缓冲机制，通过 SwapBuffers 来交换缓冲
3. MakeCurrent 会做两件事：一是将窗口对应的双缓冲绑定给 OpenGLContext， 二是将窗口对应的 OpenGLContext 绑定到当前线程
4. PrimaryMonitor 是系统设置里头的主显示器，跟 Window 无关（没有接口直接获取 Window 所在的 Monitor，可以通过 Window 在 Monitor 中的覆盖面积大小来手动算出一个所在 Monitor）
5. VirtualMonitor 坐标是以 PrimaryMonitor 的左上角作为原点，X 正向右，Y 正向下。每个 Monitor 在这个坐标系下会有一个 Position（该 Monitor 的原点），Windows 的 Position 也是基于此坐标系！（因此，要获得 Windows 相对于其所在 Monitor 左上角的坐标系需要考虑 Monitor 的原点坐标）
6. glfwSetMonitorCallback 设置的回调函数会在显示器插拔的时候发出 CONNECTION 或 DISCONNECTION 事件

## 2D 坐标系

1. Monitor 坐标系（Monitor 左上角为原点，X 正向右，Y 正向下，单位与 ScreenCoordinate 一样，Monitor 就是显示器）
2. VirtualMonitor 坐标系（基于主 Monitor 的坐标系，扩展成四个象限。Monitor 和 Window 的 Position 都基于 VirtualMonitor 坐标系）
3. WindowContentArea 坐标系（窗口左上角为原点，X 正向右，Y 正向下，单位与 ScreenCoordinate 一样，鼠标光标位置使用的是这个坐标系）
4. ScreenCoordinate 跟 PixelCoordinate 有一个比例映射关系，一般是 1:1（苹果的 Retina 显示器就不是 1:1，WindowSize / FramebufferSize）
5. 一个单位是像素的坐标，范围最大值应该是 FramebufferSize
6. 一个单位是 ScreenCoordinate 的坐标，范围最大值应该是 WindowSize 或者 MonitorWorkarea

## 相关 Size

1. MonitorWorkarea: ScreenCoordinate 的单位，显示器的大小
2. WindowSize: ScreenCoordinate 的单位，ContentArea 的大小
3. WindowFrameSize: ScreenCoordinate 的单位，窗口两侧边缘的宽度或者上下边缘的高度，包括标题栏
4. VideoModeResolution: ScreenCoordinate 的单位
5. MonitorPhysicalSize: 单位是毫米
6. FramebufferSize: 单位是像素个数

## 关于 DPI

1. DPI: DotsPerInch，一英寸等于 25.4 毫米，
2. 计算屏幕 DPI: VideoModeSize / (MonitorPhysicalSize / 25.4)
3. MonitorContentScale/WindowContentScale: CurrentDPI / PlatformDefaultDPI

## Windows 下无法调试 dll 的问题

1. 参考博客：https://blog.csdn.net/u010797208/article/details/40452797
2. 关键点是：C/C++ 》常规 》调试信息格式 需要设置成 ‘编辑并继续’
3. 排查问题先调试找到具体出错位置，dll 可以考虑让 DllMain 打印出一些信息来帮助排查
4. 将字符串打印到文件的 C 语言实现：

```c
static int writefile(const char* filename, const char* str, size_t len) {
  FILE* f = fopen(filename, "wb");
  if (f == NULL) {
    return errno;
  }
  int ret = 0;
  if (fwrite(str, sizeof(char), len, f) != len) {
    ret = errno;
  }
  if (fclose(f)) {
    ret = errno;
  }
  return ret;
}
```
