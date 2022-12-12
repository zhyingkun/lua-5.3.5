# libuv 注入 Lua 的封装

## libuv 自身特性

1. libuv 自带的-fvisibility=hidden 编译参数需要配合 UV_EXTERN 使用，目前已去掉
2. 针对 fs 是 result 等于 0 意味着 EOF，针对其他 steams，nread 等于 UV_EOF 宏才是代表 EOF，因为此时 nread 等于 0 仅表示当前缓冲没有数据可读
3. 使用线程池来实现异步操作的接口都支持传入空的回调函数来进行阻塞同步操作，比如 fs、DNS 操作
4. bufs 和 path 等数据虽然在接口上是指针形式，但是实际的数据传输方式是值传递（也就是可以传递栈临时指针）
5. 回调函数传入的结构体指针要么有对应的 API 进行 free，要么就是直接指向 request 结构的一个字段，所以，需要深复制之后才能 free 掉 request
6. handle 是长期存在的，request 是针对一次异步调用期间存在的
7. 某个 request 异步过程对应的 handle 被 close 了的话，对应的异步回调也会调用并在 status 参数传入 UV_CANCLE
8. 传递给 uv_spawn 的 options 中，file 不能为 NULL，args 可以为 NULL

## 注入 Lua

1. 每一个异步过程会对应一个 request，该过程需要 hold 住的 lua 对象可以用 request 结构指针作为 key 来保存，异步结束直接清理掉
2. 对于 handle 触发的异步过程需要 hold 住 lua 对象的话，需要保证多个 hold 住的 lua 对象之间不会互相覆盖，handle 关闭的时候统一清理掉（用 handle 结构指针+固定用途的偏移 来作为 key）
3. 在 libuv 回调函数中调用 Lua 的函数必须要用 pcall，如果用 call，抛异常会破坏 libuv 的上下文
4. libuv 异步回调在将数据压入 Lua 栈之前都要先检查槽位是否足够
5. 函数名包含 Async 表示任务被异步处理，反之则同步，有些 Async 任务允许无回调，做完收工；函数名包含 AsyncWait 表示协程中等异步任务，做完自动唤醒协程

## 其他

1. 互联网字节序是大端序，字节内部的位序是小端序
2. udp 做一次 read，如果缓冲区大小不够，多余的部分数据会被丢弃（如果数据报没有丢失，那么一个 sendto 必定对应唯一一个 recvfrom）

## 没有注册到 Lua 的 API

1. uv_loop_fork、uv_tcp_open、uv_udp_open
2. poll、async、dlopen
3. 线程同步相关的操作：互斥锁、读写锁、信号量、条件变量、安全区、thread
4. uv_cancel、直接操作 uv_req_t 的 API

## Win 下获取 Release 版奔溃日志

```c
#include "dbghelp.h"
#pragma comment(lib, "dbghelp.lib")

LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS * pExceptionInfo) {
  HANDLE hFile = CreateFile(("project.dmp"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  MINIDUMP_EXCEPTION_INFORMATION stExceptionParam;
  stExceptionParam.ThreadId = GetCurrentThreadId();
  stExceptionParam.ExceptionPointers = pExceptionInfo;
  stExceptionParam.ClientPointers = FALSE;
  MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, &stExceptionParam, NULL, NULL);
  CloseHandle(hFile);

  return EXCEPTION_EXECUTE_HANDLER;
  //EXCEPTION_EXECUTE_HANDLER equ 1 means the exception has been deal with, so just exit the process
  //EXCEPTION_CONTINUE_SEARCH equ 0 means os must search for another handler, maybe show an error window
  //EXCEPTION_CONTINUE_EXECUTION equ -1 means the exception has been fixed, just run continue
}

int main(int argc, char** argv) {
  SetUnhandledExceptionFilter(TopLevelExceptionFilter);
  return 0;
}
```

将上述代码加入 main 函数中，即可在奔溃是生成`project.dmp`文件，用 VS 打开该文件可尝试还原奔溃堆栈
