# glfw

1. glfw 创建的每个窗口默认会创建并绑定一个 OpenGL 的 Context，可通过设置 GLFW_CONTEXT_CREATION_API 告诉 glfw 不创建 Context
2. 一个窗口对应一套双缓冲机制，通过 SwapBuffers 来交换缓冲
3. MakeCurrent 会做两件事：一是将窗口对应的双缓冲绑定给 OpenGLContext， 二是将窗口对应的 OpenGLContext 绑定到当前线程
