local uvwrap = require("libuvwrap")
local libloop = uvwrap.loop
local libpipe = uvwrap.pipe
local libtcp = uvwrap.tcp
local libtty = uvwrap.tty
local libasync = uvwrap.async
local libfsevent = uvwrap.fs_event
local libfspoll = uvwrap.fs_poll
local libprepare = uvwrap.prepare
local libcheck = uvwrap.check
local libidle = uvwrap.idle
local libprocess = uvwrap.process
local libsignal = uvwrap.signal
local libtimer = uvwrap.timer
local libudp = uvwrap.udp
local libdebug = uvwrap.debug
local libfs = uvwrap.fs
local libnetwork = uvwrap.network
local libos = uvwrap.os
local libsys = uvwrap.sys
local libthread = uvwrap.thread
local libmbio = uvwrap.mbio

local ASYNC_WAIT_MSG = "AsyncWait api must running in coroutine"
local queueWork = uvwrap.queue_work
local OK = uvwrap.err_code.OK

local running = coroutine.running
local yield = coroutine.yield
local resumeOrigin = coroutine.resume
local function resume(...)
	local status, msg = resumeOrigin(...)
	if not status then printerr("libuv lua module resume coroutine error: ", msg) end
end

---@alias StatusCallbackSignature fun(status:integer):void

---@class libuv:table
local libuv = {}

--[[
** {======================================================
** Delay Multi Frames
** =======================================================
--]]

local delayJob
local currentJob
local function delayFrameStart()
	delayJob, currentJob = {}, {}
end
local function delayFrameTick()
	for job, check in pairs(delayJob) do
		delayJob[job] = nil
		currentJob[job] = check
	end
	for job, check in pairs(currentJob) do
		if check() then
			currentJob[job] = nil
			job() -- maybe add more job to delayJob
		end
	end
end
local function delayFrameEnd()
	delayJob, currentJob = nil, nil
end

function libuv.delayFrameAsync(check, job)
	local t = type(check)
	if t ~= "function" then
		local count = t == "number" and check or 1
		check = function()
			count = count - 1
			return count <= 0
		end
	end
	delayJob[job] = check
end

---@param check integer
function libuv.delayFrameAsyncWait(check)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libuv.delayFrameAsync(check, function()
		resume(co)
	end)
	return yield()
end

-- }======================================================

---@type uv_loop_t
local loopCtx

---@param ctx uv_loop_t | nil
function libuv.setLoop(ctx)
	delayFrameStart()
	loopCtx = ctx or libloop.default()
end

local libloop_run = libloop.run
local libloop_close = libloop.close
local NOWAIT = libloop.run_mode.NOWAIT
local function run(ctx, mode)
	delayFrameTick()
	return libloop_run(ctx, mode)
end
local function close(ctx)
	delayFrameEnd()
	return libloop_close(ctx)
end

function libuv.run()
	return run(loopCtx)
end

function libuv.runNoWait()
	return run(loopCtx, NOWAIT)
end

function libuv.runDeferred()
	atexit(function()
		if not loopCtx then return end
		run(loopCtx)
		close(loopCtx)
		loopCtx = nil
		uvwrap.set_realloc_cb()
	end)
end

---@return integer
function libuv.close()
	return close(loopCtx)
end

--[[
** {======================================================
** Pipe
** =======================================================
--]]

---@class libuv_pipe:table
local pipe = {}
libuv.pipe = pipe

---@alias StatusPipeSignature fun(status:integer, handle:uv_pipe_t):void
---@alias PipeReadSignature fun(nread:integer, str:string | nil, handle:uv_pipe_t):void

---@class uv_pipe_t:uv_stream_t
---@field public open fun(self:uv_pipe_t, fd:integer):uv_pipe_t
---@field public bind fun(self:uv_pipe_t, name:string):uv_pipe_t
---@field public connectAsync fun(self:uv_pipe_t, name:string, callback:StatusPipeSignature):void
---@field public connectAsyncWait fun(self:uv_pipe_t, name:string):integer
---@field public getSockName fun(self:uv_pipe_t):string
---@field public getPeerName fun(self:uv_pipe_t):string
---@field public pendingInstances fun(self:uv_pipe_t, count:integer):void
---@field public pendingCount fun(self:uv_pipe_t):integer
---@field public pendingType fun(self:uv_pipe_t):libuv_handle_type
---@field public chmod fun(self:uv_pipe_t, mode:integer):void
---@field public closeAsync fun(self:uv_pipe_t, callback:fun(handle:uv_pipe_t):void):void @uv_handle_t
---@field public shutdownAsync fun(self:uv_pipe_t, callback:StatusPipeSignature):void @uv_stream_t
---@field public listenAsync fun(self:uv_pipe_t, backlog:integer, callback:StatusPipeSignature):void @uv_stream_t
---@field public accept fun(self:uv_pipe_t, client:uv_pipe_t | nil):integer, uv_pipe_t @uv_stream_t
---@field public readStartAsync fun(self:uv_pipe_t, callback:PipeReadSignature):void @uv_stream_t
---@field public writeAsync fun(self:uv_pipe_t, data:string, callback:StatusPipeSignature):void @uv_stream_t
---@field public write2Async fun(self:uv_pipe_t, data:string, sendHandle:uv_stream_t, callback:StatusPipeSignature):void @uv_stream_t

---@param ipc boolean
---@return uv_pipe_t
function pipe.Pipe(ipc)
	return libpipe.Pipe(loopCtx, ipc)
end

-- }======================================================

--[[
** {======================================================
** Stream
** =======================================================
--]]

---@alias ReadCallbackSignature fun(nread:integer, str:string | nil, handle:uv_stream_t):void
---@alias StatusStreamSignature fun(status:integer, handle:uv_stream_t):void

---@class uv_stream_t:uv_handle_t
---@field public shutdownAsync fun(self:uv_stream_t, callback:StatusStreamSignature | nil):void @callback version in child class
---@field public shutdownAsyncWait fun(self:uv_stream_t):integer
---@field public listenAsync fun(self:uv_stream_t, backlog:integer, callback:StatusStreamSignature):void
---@field public accept fun(self:uv_stream_t, client:uv_stream_t | nil):integer, uv_stream_t
---@field public readStartAsync fun(self:uv_stream_t, callback:ReadCallbackSignature):void
---@field public readStop fun(self:uv_stream_t):void
---@field public writeAsync fun(self:uv_stream_t, data:string, callback:StatusStreamSignature | nil):void @callback version in child class
---@field public writeAsyncWait fun(self:uv_stream_t, data:string):integer
---@field public write2Async fun(self:uv_stream_t, data:string, sendHandle:uv_stream_t, callback:StatusStreamSignature | nil):void @callback version in child class
---@field public write2AsyncWait fun(self:uv_stream_t, data:string, sendHandle:uv_stream_t):integer
---@field public tryWrite fun(self:uv_stream_t, data:string):integer
---@field public isReadable fun(self:uv_stream_t):boolean
---@field public isWritable fun(self:uv_stream_t):boolean
---@field public setBlocking fun(self:uv_stream_t, block:boolean):void
---@field public getWriteQueueSize fun(self:uv_stream_t):integer

-- }======================================================

--[[
** {======================================================
** TCP
** =======================================================
--]]

---@class libuv_tcp:table
local tcp = {}
libuv.tcp = tcp

---@alias StatusTcpSignature fun(status:integer, handle:uv_tcp_t):void
---@alias TcpReadSignature fun(nread:integer, str:string | nil, handle:uv_tcp_t):void

---@class uv_tcp_t:uv_stream_t
---@field public bind fun(self:uv_tcp_t, addr:sockaddr, flags:libuv_tcp_flag | nil):uv_tcp_t
---@field public connectAsync fun(self:uv_tcp_t, addr:sockaddr, callback:StatusTcpSignature):void
---@field public connectAsyncWait fun(self:uv_tcp_t, addr:sockaddr):integer
---@field public noDelay fun(self:uv_tcp_t, enable:boolean):void
---@field public keepAlive fun(self:uv_tcp_t, enable:boolean, delay:integer):void
---@field public simultaneousAccepts fun(self:uv_tcp_t, enable:boolean):void
---@field public getSockName fun(self:uv_tcp_t):sockaddr
---@field public getPeerName fun(self:uv_tcp_t):sockaddr
---@field public closeAsync fun(self:uv_tcp_t, callback:fun(handle:uv_tcp_t):void):void @uv_handle_t
---@field public shutdownAsync fun(self:uv_tcp_t, callback:StatusTcpSignature):void @uv_stream_t
---@field public listenAsync fun(self:uv_tcp_t, backlog:integer, callback:StatusTcpSignature):void @uv_stream_t
---@field public accept fun(self:uv_tcp_t, client:uv_tcp_t | nil):integer, uv_tcp_t @uv_stream_t
---@field public readStartAsync fun(self:uv_tcp_t, callback:TcpReadSignature):void @uv_stream_t
---@field public writeAsync fun(self:uv_tcp_t, data:string, callback:StatusTcpSignature):void @uv_stream_t
---@field public write2Async fun(self:uv_tcp_t, data:string, sendHandle:uv_stream_t, callback:StatusTcpSignature):void @uv_stream_t

---@overload fun():uv_tcp_t
---@overload fun(flags:libuv_address_family):uv_tcp_t
---@param flags libuv_address_family
---@return uv_tcp_t
function tcp.Tcp(flags)
	return libtcp.Tcp(loopCtx, flags)
end

---@class libuv_tcp_flag
---@field public IPV6ONLY integer

---@type libuv_tcp_flag
tcp.tcp_flag = libtcp.tcp_flag

-- }======================================================

--[[
** {======================================================
** TTY
** =======================================================
--]]

---@class libuv_tty:table
local tty = {}
libuv.tty = tty

---@alias TtyReadSignature fun(nread:integer, str:string | nil, handle:uv_tty_t):void
---@alias StatusTtySignature fun(status:integer, handle:uv_tty_t):void

---@class uv_tty_t:uv_stream_t
---@field public setMode fun(self:uv_tty_t, mode:libuv_tty_mode):void
---@field public getWinSize fun(self:uv_tty_t):integer, integer @width, height
---@field public closeAsync fun(self:uv_tty_t, callback:fun(handle:uv_tty_t):void):void @uv_handle_t
---@field public shutdownAsync fun(self:uv_tty_t, callback:StatusTtySignature):void @uv_stream_t
---@field public listenAsync fun(self:uv_tty_t, backlog:integer, callback:StatusTtySignature):void @uv_stream_t
---@field public accept fun(self:uv_tty_t, client:uv_tty_t | nil):integer, uv_tty_t @uv_stream_t
---@field public readStartAsync fun(self:uv_tty_t, callback:TtyReadSignature):void @uv_stream_t
---@field public writeAsync fun(self:uv_tty_t, data:string, callback:StatusTtySignature):void @uv_stream_t
---@field public write2Async fun(self:uv_tty_t, data:string, sendHandle:uv_stream_t, callback:StatusTtySignature):void @uv_stream_t

---@param fd integer
---@return uv_tty_t
function tty.Tty(fd)
	return libtty.Tty(loopCtx, fd)
end
function tty.resetMode()
	libtty.resetMode()
end

---@class libuv_tty_mode
---@field public NORMAL integer
---@field public RAW integer
---@field public IO integer

---@type libuv_tty_mode
tty.tty_mode = libtty.tty_mode

-- }======================================================

--[[
** {======================================================
** Async
** =======================================================
--]]

---@class libuv_async
local async = {}
libuv.async = async

---@class uv_async_t:uv_handle_t
---@field public closeAsync fun(self:uv_async_t, callback:fun(handle:uv_async_t):void):void @uv_handle_t

---@param callback fun():void
---@return uv_async_t
function async.AsyncEvent(callback)
	return libasync.AsyncEvent(loopCtx, callback)
end

---@type lightuserdata
async.fireAsyncPtr = libasync.fireAsyncPtr

-- }======================================================

--[[
** {======================================================
** FS Event
** =======================================================
--]]

---@class libuv_fs_event:table
local fsevent = {}
libuv.fsevent = fsevent

---@alias FsEventSignature fun(filename:string, event:libuv_event_type, status:integer, handle:uv_fs_event_t):void

---@class uv_fs_event_t:uv_handle_t
---@field public startAsync fun(self:uv_fs_event_t, callback:FsEventSignature, filePath:string, flags:libuv_event_flag):void
---@field public stop fun(self:uv_fs_event_t):void
---@field public getPath fun(self:uv_fs_event_t):string | nil, nil | integer @path, errCode
---@field public closeAsync fun(self:uv_fs_event_t, callback:fun(handle:uv_fs_event_t):void):void @uv_handle_t

---@return uv_fs_event_t
function fsevent.FsEvent()
	return libfsevent.FsEvent(loopCtx)
end

---@class libuv_event_type
---@field public RENAME integer
---@field public CHANGE integer

---@type libuv_event_type
fsevent.event_type = libfsevent.event_type

---@class libuv_event_flag
---@field public WATCH_ENTRY integer
---@field public STAT integer
---@field public RECURSIVE integer

---@type libuv_event_flag
fsevent.event_flag = libfsevent.event_flag

-- }======================================================

--[[
** {======================================================
** FS Poll
** =======================================================
--]]

---@class libuv_fs_poll:table
local fspoll = {}
libuv.fspoll = fspoll

---@class uv_timespec_t:table
---@field public sec integer
---@field public nsec integer

---@class uv_stat_t:table
---@field public dev integer
---@field public mode integer
---@field public nlink integer
---@field public uid integer
---@field public gid integer
---@field public rdev integer
---@field public ino integer
---@field public size integer
---@field public blksize integer
---@field public blocks integer
---@field public flags integer
---@field public gen integer
---@field public atim uv_timespec_t
---@field public mtim uv_timespec_t
---@field public ctim uv_timespec_t
---@field public birthtim uv_timespec_t

---@alias FsPollSignature fun(status:integer, prev:uv_stat_t, curr:uv_stat_t, handle:uv_fs_poll_t):void

---@class uv_fs_poll_t:uv_handle_t
---@field public startAsync fun(self:uv_fs_poll_t, callback:FsPollSignature, path:string, interval:integer):void
---@field public stop fun(self:uv_fs_poll_t):void
---@field public getPath fun(self:uv_fs_poll_t):string | nil, nil | integer @path, errCode
---@field public closeAsync fun(self:uv_fs_poll_t, callback:fun(handle:uv_fs_poll_t):void):void @uv_handle_t

---@return uv_fs_poll_t
function fspoll.FsPoll()
	return libfspoll.FsPoll(loopCtx)
end

-- }======================================================

--[[
** {======================================================
** Handle
** =======================================================
--]]

---@alias StatusHandleSignature fun(handle:uv_handle_t):void

---@class uv_handle_t:userdata
---@field public isActive fun(self:uv_handle_t):boolean
---@field public isClosing fun(self:uv_handle_t):boolean
---@field public closeAsync fun(self:uv_handle_t, callback:StatusHandleSignature | nil):void @callback version in child class
---@field public closeAsyncWait fun(self:uv_handle_t):void
---@field public ref fun(self:uv_handle_t):void
---@field public unref fun(self:uv_handle_t):void
---@field public hasRef fun(self:uv_handle_t):boolean
---@field public sendBufferSize fun(self:uv_handle_t, value:integer | nil):integer
---@field public recvBufferSize fun(self:uv_handle_t, value:integer | nil):integer
---@field public fileno fun(self:uv_handle_t):integer | nil, nil | integer @fd, errCode
---@field public getLoop fun(self:uv_handle_t):uv_loop_t
---@field public getType fun(self:uv_handle_t):libuv_handle_type

-- }======================================================

--[[
** {======================================================
** Misc
** =======================================================
--]]

---@alias PrepareCallbackSignature fun(handle:uv_prepare_t):void

---@class libuv_prepare:table
local prepare = {}
libuv.prepare = prepare

---@class uv_prepare_t:uv_handle_t
---@field public startAsync fun(self:uv_prepare_t, callback:PrepareCallbackSignature):void
---@field public stop fun(self:uv_prepare_t):void
---@field public closeAsync fun(self:uv_prepare_t, callback:fun(handle:uv_prepare_t):void):void @uv_handle_t

---@return uv_prepare_t
function prepare.Prepare()
	return libprepare.Prepare(loopCtx)
end

---@alias CheckCallbackSignature fun(handle:uv_check_t):void

---@class libuv_check:table
local check = {}
libuv.check = check

---@class uv_check_t:uv_handle_t
---@field public startAsync fun(self:uv_check_t, callback:CheckCallbackSignature):void
---@field public stop fun(self:uv_check_t):void
---@field public closeAsync fun(self:uv_check_t, callback:fun(handle:uv_check_t):void):void @uv_handle_t

---@return uv_check_t
function check.Check()
	return libcheck.Check(loopCtx)
end

---@alias IdleCallbackSignature fun(handle:uv_idle_t):void

---@class libuv_idle:table
local idle = {}
libuv.idle = idle

---@class uv_idle_t:uv_handle_t
---@field public startAsync fun(self:uv_idle_t, callback:IdleCallbackSignature):void
---@field public stop fun(self:uv_idle_t):void
---@field public closeAsync fun(self:uv_idle_t, callback:fun(handle:uv_idle_t):void):void @uv_handle_t

---@return uv_idle_t
function idle.Idle()
	return libidle.Idle(loopCtx)
end

-- }======================================================

--[[
** {======================================================
** Process
** =======================================================
--]]

---@class libuv_process:table
local process = {}
libuv.process = process

---@class uv_process_t:uv_handle_t
---@field public kill fun(self:uv_process_t, signum:integer):void
---@field public getPid fun(self:uv_process_t):integer
---@field public closeAsync fun(self:uv_process_t, callback:fun(handle:uv_process_t):void):void @uv_handle_t

---@class uv_process_config:table
---@field public file string
---@field public args string[]
---@field public env string[]
---@field public stdio uvwrap_stdio_container_t
---@field public exitCallback fun(exitStatus:integer, termSignal:integer):void
---@field public cwd string
---@field public flags libuv_process_flag
---@field public uid integer
---@field public gid integer

---@param config uv_process_config
---@return uv_process_t, integer @process, errCode
function process.Process(config)
	return libprocess.Process(loopCtx, config)
end
function process.disableStdioInheritance()
	libprocess.disable_stdio_inheritance()
end
---@param pid integer
---@param sigNum integer
function process.kill(pid, sigNum)
	libprocess.kill(pid, sigNum)
end
---@param argc integer
---@param argv lightuserdata
---@return lightuserdata
function process.setupArgs(argc, argv)
	return libprocess.setup_args(argc, argv)
end
---@return string
function process.getProcessTitle()
	return libprocess.get_process_title()
end
---@param title string
function process.setProcessTitle(title)
	libprocess.set_process_title(title)
end
---@return integer
function process.getPid()
	return libprocess.getpid()
end
---@return integer
function process.getPpid()
	return libprocess.getppid()
end
---@return string
function process.exePath()
	return libprocess.exepath()
end
---@return string
function process.cwd()
	return libprocess.cwd()
end
---@param dir string
function process.chdir(dir)
	libprocess.chdir(dir)
end

---@class uvwrap_stdio_container_t:userdata
---@field public add fun(self:uvwrap_stdio_container_t, flags:libuv_stdio_flag, file:integer | uv_stream_t):void

---@overload fun():uvwrap_stdio_container_t
---@overload fun(maxSize:integer):uvwrap_stdio_container_t
---@param maxSize integer
---@return uvwrap_stdio_container_t
function process.StdioContainer(maxSize)
	return libprocess.stdio_container(maxSize)
end

---@class libuv_process_flag
---@field public SETUID integer
---@field public SETGID integer
---@field public WINDOWS_VERBATIM_ARGUMENTS integer
---@field public DETACHED integer
---@field public WINDOWS_HIDE integer
---@field public WINDOWS_HIDE_CONSOLE integer
---@field public WINDOWS_HIDE_GUI integer

---@type libuv_process_flag
process.process_flag = libprocess.process_flag

---@class libuv_stdio_flag
---@field public IGNORE integer
---@field public CREATE_PIPE integer
---@field public INHERIT_FD integer
---@field public INHERIT_STREAM integer
---@field public READABLE_PIPE integer
---@field public WRITABLE_PIPE integer
---@field public OVERLAPPED_PIPE integer

---@type libuv_stdio_flag
process.stdio_flag = libprocess.stdio_flag

-- }======================================================

--[[
** {======================================================
** Signal
** =======================================================
--]]

---@class libuv_signal:table
local signal = {}
libuv.signal = signal

---@alias SignalCallbackSignature fun(sigNum:integer, handle:uv_signal_t):void

---@class uv_signal_t:uv_handle_t
---@field public startAsync fun(self:uv_signal_t, callback:SignalCallbackSignature, sigNum:integer):void
---@field public startOneShotAsync fun(self:uv_signal_t, callback:SignalCallbackSignature, sigNum:integer):void
---@field public stop fun(self:uv_signal_t):void
---@field public closeAsync fun(self:uv_signal_t, callback:fun(handle:uv_signal_t):void):void @uv_handle_t

---@return uv_signal_t
function signal.Signal()
	return libsignal.Signal(loopCtx)
end

---@class libuv_sig_num
---@field public SIGHUP integer
---@field public SIGINT integer
---@field public SIGQUIT integer @for MacOSX and Linux only
---@field public SIGILL integer
---@field public SIGTRAP integer @for MacOSX and Linux only
---@field public SIGABRT integer
---@field public SIGFPE integer
---@field public SIGKILL integer
---@field public SIGBUS integer @for MacOSX and Linux only
---@field public SIGSEGV integer
---@field public SIGSYS integer @for MacOSX and Linux only
---@field public SIGPIPE integer @for MacOSX and Linux only
---@field public SIGALRM integer @for MacOSX and Linux only
---@field public SIGTERM integer
---@field public SIGURG integer @for MacOSX and Linux only
---@field public SIGSTOP integer @for MacOSX and Linux only
---@field public SIGTSTP integer @for MacOSX and Linux only
---@field public SIGCONT integer @for MacOSX and Linux only
---@field public SIGCHLD integer @for MacOSX and Linux only
---@field public SIGTTIN integer @for MacOSX and Linux only
---@field public SIGTTOU integer @for MacOSX and Linux only
---@field public SIGIO integer @for Linux only
---@field public SIGXCPU integer @for MacOSX and Linux only
---@field public SIGXFSZ integer @for MacOSX and Linux only
---@field public SIGVTALRM integer @for MacOSX and Linux only
---@field public SIGPROF integer @for MacOSX and Linux only
---@field public SIGWINCH integer @for Windows and Linux only
---@field public SIGPWR integer @for Linux only
---@field public SIGUSR1 integer @for MacOSX and Linux only
---@field public SIGUSR2 integer @for MacOSX and Linux only

---@type libuv_sig_num
signal.sig_num = libsignal.sig_num

---@class libuv_sig_name

---@type libuv_sig_name
signal.sig_name = libsignal.sig_name

-- }======================================================

--[[
** {======================================================
** Timer
** =======================================================
--]]

---@class libuv_timer:table
local timer = {}
libuv.timer = timer

---@alias TimerCallbackSignature fun(handle:uv_timer_t):void

---@class uv_timer_t:uv_handle_t
---@field public startAsync fun(self:uv_timer_t, callback:TimerCallbackSignature, timeOut:integer, repeat:integer):void
---@field public startAsyncWait fun(self:uv_timer_t, timeOut:integer):void
---@field public stop fun(self:uv_timer_t):void
---@field public again fun(self:uv_timer_t):void
---@field public setRepeat fun(self:uv_timer_t, repeat:integer):void
---@field public getRepeat fun(self:uv_timer_t):integer
---@field public closeAsync fun(self:uv_timer_t, callback:fun(handle:uv_timer_t):void):void @uv_handle_t

---@return uv_timer_t
function timer.Timer()
	return libtimer.Timer(loopCtx)
end

-- }======================================================

--[[
** {======================================================
** UDP
** =======================================================
--]]

---@class libuv_udp:table
local udp = {}
libuv.udp = udp

---@alias SendCallbackSignature fun(status:integer, handle:uv_udp_t):void
---@alias RecvCallbackSignature fun(nread:integer, data:string | nil, addr:sockaddr | nil, flags:libuv_udp_flag, handle:uv_udp_t):void

---@class uv_udp_t:uv_handle_t
---@field public bind fun(self:uv_udp_t, addr:sockaddr, flags:libuv_udp_flag):uv_udp_t
---@field public connect fun(self:uv_udp_t, addr:sockaddr):integer
---@field public getSockName fun(self:uv_udp_t):sockaddr
---@field public getPeerName fun(self:uv_udp_t):sockaddr
---@field public setMembership fun(self:uv_udp_t, multicastAddr:string, interfaceAddr:string, membership:sockaddr):uv_udp_t
---@field public setMulticastLoop fun(self:uv_udp_t, on:boolean):uv_udp_t
---@field public setMulticastTtl fun(self:uv_udp_t, ttl:integer):uv_udp_t
---@field public setMulticastInterface fun(self:uv_udp_t, interfaceAddr:string):uv_udp_t
---@field public setBroadcast fun(self:uv_udp_t, on:boolean):uv_udp_t
---@field public setTtl fun(self:uv_udp_t, ttl:integer):uv_udp_t
---@field public sendAsync fun(self:uv_udp_t, data:string, addr:sockaddr, callback:SendCallbackSignature | nil):void
---@field public sendAsyncWait fun(self:uv_udp_t, data:string, addr:sockaddr):integer
---@field public trySend fun(self:uv_udp_t, data:string, addr:sockaddr):void
---@field public recvStartAsync fun(self:uv_udp_t, callback:RecvCallbackSignature):void
---@field public recvStop fun(self:uv_udp_t):void
---@field public getSendQueueSize fun(self:uv_udp_t):integer
---@field public getSendQueueCount fun(self:uv_udp_t):integer
---@field public closeAsync fun(self:uv_udp_t, callback:fun(handle:uv_udp_t):void):void @uv_handle_t

---@overload fun():uv_udp_t
---@overload fun(flags:libuv_address_family):uv_udp_t
---@param flags libuv_address_family
---@return uv_udp_t
function udp.Udp(flags)
	return libudp.Udp(loopCtx, flags)
end

---@class libuv_membership
---@field public LEAVE_GROUP integer
---@field public JOIN_GROUP integer

---@type libuv_membership
udp.membership = libudp.membership

---@class libuv_udp_flag
---@field public IPV6ONLY integer
---@field public PARTIAL integer
---@field public REUSEADDR integer

---@type libuv_udp_flag
udp.udp_flag = libudp.udp_flag

-- }======================================================

--[[
** {======================================================
** Debug
** =======================================================
--]]

---@class libuv_debug:table
local debug = {}
libuv.debug = debug

---@param fileStream file
function debug.printAllHandles(fileStream)
	libdebug.print_all_handles(loopCtx, fileStream)
end
---@param fileStream file
function debug.printActiveHandles(fileStream)
	libdebug.print_active_handles(loopCtx, fileStream)
end

-- }======================================================

--[[
** {======================================================
** FileSystem
** =======================================================
--]]

---@class libuv_fs:table
local fs = {}
libuv.fs = fs

---@param fd integer
---@return integer
function fs.close(fd)
	return libfs.close(loopCtx, fd)
end
---@param fd integer
---@param callback StatusCallbackSignature
function fs.closeAsync(fd, callback)
	libfs.close(loopCtx, fd, callback)
end
---@param fd integer
---@return integer
function fs.closeAsyncWait(fd)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.close(loopCtx, fd, function(err)
		resume(co, err)
	end)
	return yield()
end
---@param filePath string
---@param flags libuv_open_flag
---@param mode integer
---@return integer
function fs.open(filePath, flags, mode)
	return libfs.open(loopCtx, filePath, flags, mode)
end
---@param filePath string
---@param flags libuv_open_flag
---@param mode integer
---@param callback StatusCallbackSignature
function fs.openAsync(filePath, flags, mode, callback)
	libfs.open(loopCtx, filePath, flags, mode, callback)
end
---@param filePath string
---@param flags libuv_open_flag
---@param mode integer
---@return integer
function fs.openAsyncWait(filePath, flags, mode)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.open(loopCtx, filePath, flags, mode, function(fd)
		resume(co, fd)
	end)
	return yield()
end
---@param fd integer
---@param offset integer
---@return string, integer
function fs.read(fd, offset)
	return libfs.read(loopCtx, fd, offset)
end
---@param fd integer
---@param offset integer
---@param callback fun(str:string | nil, ret:integer):void
function fs.readAsync(fd, offset, callback)
	libfs.read(loopCtx, fd, offset, callback)
end
---@param fd integer
---@param offset integer
---@return string, integer
function fs.readAsyncWait(fd, offset)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.read(loopCtx, fd, offset, function(str, ret)
		resume(co, str, ret)
	end)
	return yield()
end
---@param filePath string
---@return integer
function fs.unlink(filePath)
	return libfs.unlink(loopCtx, filePath)
end
---@param filePath string
---@param callback StatusCallbackSignature
function fs.unlinkAsync(filePath, callback)
	libfs.unlink(loopCtx, filePath, callback)
end
---@param filePath string
---@return integer
function fs.unlinkAsyncWait(filePath)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.unlink(loopCtx, filePath, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param fd integer
---@param str string
---@param offset integer
---@return integer
function fs.write(fd, str, offset)
	return libfs.write(loopCtx, fd, str, offset)
end
---@param fd integer
---@param str string
---@param offset integer
---@param callback StatusCallbackSignature
function fs.writeAsync(fd, str, offset, callback)
	libfs.write(loopCtx, fd, str, offset, callback)
end
---@param fd integer
---@param str string
---@param offset integer
---@return integer
function fs.writeAsyncWait(fd, str, offset)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.write(loopCtx, fd, str, offset, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param filePath string
---@param mode integer
---@return integer
function fs.mkdir(filePath, mode)
	return libfs.mkdir(loopCtx, filePath, mode)
end
---@param filePath string
---@param mode integer
---@param callback StatusCallbackSignature
function fs.mkdirAsync(filePath, mode, callback)
	libfs.mkdir(loopCtx, filePath, mode, callback)
end
---@param filePath string
---@param mode integer
---@return integer
function fs.mkdirAsyncWait(filePath, mode)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.mkdir(loopCtx, filePath, mode, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param template string
---@return string | nil, integer
function fs.mkdtemp(template)
	return libfs.mkdtemp(loopCtx, template)
end
---@param template string
---@param callback fun(path:string | nil, ret:integer):void
function fs.mkdtempAsync(template, callback)
	libfs.mkdtemp(loopCtx, template, callback)
end
---@param template string
---@return string | nil, integer
function fs.mkdtempAsyncWait(template)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.mkdtemp(loopCtx, template, function(path, ret)
		resume(co, path, ret)
	end)
	return yield()
end
---@param path string
---@return integer
function fs.rmdir(path)
	return libfs.rmdir(loopCtx, path)
end
---@param path string
---@param callback StatusCallbackSignature
function fs.rmdirAsync(path, callback)
	libfs.rmdir(loopCtx, path, callback)
end
---@param path string
---@return integer
function fs.rmdirAsyncWait(path)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.rmdir(loopCtx, path, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param path string
---@return lightuserdata | nil, integer
function fs.opendir(path)
	return libfs.opendir(loopCtx, path)
end
---@param path string
---@param callback fun(ret:integer, str:string | nil):void
function fs.opendirAsync(path, callback)
	libfs.opendir(loopCtx, path, callback)
end
---@param path string
---@return lightuserdata | nil, integer
function fs.opendirAsyncWait(path)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.opendir(loopCtx, path, function(err, dir)
		resume(co, dir, err)
	end)
	return yield()
end
---@param dir lightuserdata
---@return integer
function fs.closedir(dir)
	return libfs.closedir(loopCtx, dir)
end
---@param dir lightuserdata
---@param callback StatusCallbackSignature
function fs.closedirAsync(dir, callback)
	libfs.closedir(loopCtx, dir, callback)
end
---@param dir lightuserdata
---@return integer
function fs.closedirAsyncWait(dir)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.closedir(loopCtx, dir, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param dir lightuserdata
---@return string | nil, libuv_dirent_type | nil, integer
function fs.readdir(dir)
	return libfs.readdir(loopCtx, dir)
end
---@param dir lightuserdata
---@param callback fun(name:string | nil, type:libuv_dirent_type | nil, ret:integer):void
function fs.readdirAsync(dir, callback)
	libfs.readdir(loopCtx, dir, callback)
end
---@param dir lightuserdata
---@return string | nil, libuv_dirent_type | nil, integer
function fs.readdirAsyncWait(dir)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.readdir(loopCtx, dir, function(name, type, ret)
		resume(co, name, type, ret)
	end)
	return yield()
end
---@param path string
---@param flags integer
---@return table<string, libuv_dirent_type> | nil, integer
function fs.scandir(path, flags)
	return libfs.scandir(loopCtx, path, flags)
end
---@param path string
---@param flags integer
---@param callback fun(ents:table<string, libuv_dirent_type> | nil, ret:integer):void
function fs.scandirAsync(path, flags, callback)
	libfs.scandir(loopCtx, path, flags, callback)
end
---@param path string
---@param flags integer
---@return table<string, libuv_dirent_type> | nil, integer
function fs.scandirAsyncWait(path, flags)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.scandir(loopCtx, path, flags, function(ents, ret)
		resume(co, ents, ret)
	end)
	return yield()
end

---@param path string
---@return uv_stat_t | nil, integer
function fs.stat(path)
	return libfs.stat(loopCtx, path)
end
---@param path string
---@param callback fun(stat:uv_stat_t | nil, ret:integer):void
function fs.statAsync(path, callback)
	libfs.stat(loopCtx, path, callback)
end
---@param path string
---@return uv_stat_t | nil, integer
function fs.statAsyncWait(path)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.stat(loopCtx, path, function(stat, ret)
		resume(co, stat, ret)
	end)
	return yield()
end
---@param fd integer
---@return uv_stat_t | nil, integer
function fs.fstat(fd)
	return libfs.fstat(loopCtx, fd)
end
---@param fd integer
---@param callback fun(stat:uv_stat_t | nil, ret:integer):void
function fs.fstatAsync(fd, callback)
	libfs.fstat(loopCtx, fd, callback)
end
---@param fd integer
---@return uv_stat_t | nil, integer
function fs.fstatAsyncWait(fd)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.fstat(loopCtx, fd, function(stat, ret)
		resume(co, stat, ret)
	end)
	return yield()
end
---@param path string
---@return uv_stat_t | nil, integer
function fs.lstat(path)
	return libfs.lstat(loopCtx, path)
end
---@param path string
---@param callback fun(stat:uv_stat_t | nil, ret:integer):void
function fs.lstatAsync(path, callback)
	libfs.lstat(loopCtx, path, callback)
end
---@param path string
---@return uv_stat_t | nil, integer
function fs.lstatAsyncWait(path)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.lstat(loopCtx, path, function(stat, ret)
		resume(co, stat, ret)
	end)
	return yield()
end

---@class uv_statfs_t:table
---@field public type integer
---@field public bsize integer
---@field public blocks integer
---@field public bfree integer
---@field public bavail integer
---@field public files integer
---@field public ffree integer
---@field public spare integer[]

---@param path string
---@return uv_statfs_t | nil, integer
function fs.statfs(path)
	return libfs.statfs(loopCtx, path)
end
---@param path string
---@param callback fun(stat:uv_statfs_t | nil, ret:integer):void
function fs.statfsAsync(path, callback)
	libfs.statfs(loopCtx, path, callback)
end
---@param path string
---@return uv_statfs_t | nil, integer
function fs.statfsAsyncWait(path)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.statfs(loopCtx, path, function(stat, ret)
		resume(co, stat, ret)
	end)
	return yield()
end
---@param path string
---@param newPath string
---@return integer
function fs.rename(path, newPath)
	return libfs.rename(loopCtx, path, newPath)
end
---@param path string
---@param newPath string
---@param callback StatusCallbackSignature
function fs.renameAsync(path, newPath, callback)
	libfs.rename(loopCtx, path, newPath, callback)
end
---@param path string
---@param newPath string
---@return integer
function fs.renameAsyncWait(path, newPath)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.rename(loopCtx, path, newPath, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param fd integer
---@return integer
function fs.fsync(fd)
	return libfs.fsync(loopCtx, fd)
end
---@param fd integer
---@param callback StatusCallbackSignature
function fs.fsyncAsync(fd, callback)
	libfs.fsync(loopCtx, fd, callback)
end
---@param fd integer
---@return integer
function fs.fsyncAsyncWait(fd)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.fsync(loopCtx, fd, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param fd integer
---@return integer
function fs.fdatasync(fd)
	return libfs.fdatasync(loopCtx, fd)
end
---@param fd integer
---@param callback StatusCallbackSignature
function fs.fdatasyncAsync(fd, callback)
	libfs.fdatasync(loopCtx, fd, callback)
end
---@param fd integer
---@return integer
function fs.fdatasyncAsyncWait(fd)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.fdatasync(loopCtx, fd, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param fd integer
---@param offset integer
---@return integer
function fs.ftruncate(fd, offset)
	return libfs.ftruncate(loopCtx, fd, offset)
end
---@param fd integer
---@param offset integer
---@param callback StatusCallbackSignature
function fs.ftruncateAsync(fd, offset, callback)
	libfs.ftruncate(loopCtx, fd, offset, callback)
end
---@param fd integer
---@param offset integer
---@return integer
function fs.ftruncateAsyncWait(fd, offset)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.ftruncate(loopCtx, fd, offset, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param path string
---@param newPath string
---@param flags integer
---@return integer
function fs.copyfile(path, newPath, flags)
	return libfs.copyfile(loopCtx, path, newPath, flags)
end
---@param path string
---@param newPath string
---@param flags integer
---@param callback StatusCallbackSignature
function fs.copyfileAsync(path, newPath, flags, callback)
	libfs.copyfile(loopCtx, path, newPath, flags, callback)
end
---@param path string
---@param newPath string
---@param flags integer
---@return integer
function fs.copyfileAsyncWait(path, newPath, flags)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.copyfile(loopCtx, path, newPath, flags, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param outFd integer
---@param inFd integer
---@param inOffset integer
---@param length integer
---@return integer
function fs.sendfile(outFd, inFd, inOffset, length)
	return libfs.sendfile(loopCtx, outFd, inFd, inOffset, length)
end
---@param outFd integer
---@param inFd integer
---@param inOffset integer
---@param length integer
---@param callback StatusCallbackSignature
function fs.sendfileAsync(outFd, inFd, inOffset, length, callback)
	libfs.sendfile(loopCtx, outFd, inFd, inOffset, length, callback)
end
---@param outFd integer
---@param inFd integer
---@param inOffset integer
---@param length integer
---@return integer
function fs.sendfileAsyncWait(outFd, inFd, inOffset, length)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.sendfile(loopCtx, outFd, inFd, inOffset, length, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param path string
---@param mode integer
---@return integer
function fs.access(path, mode)
	return libfs.access(loopCtx, path, mode)
end
---@param path string
---@param mode integer
---@param callback StatusCallbackSignature
function fs.accessAsync(path, mode, callback)
	libfs.access(loopCtx, path, mode, callback)
end
---@param path string
---@param mode integer
---@return integer
function fs.accessAsyncWait(path, mode)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.access(loopCtx, path, mode, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param path string
---@param mode integer
---@return integer
function fs.chmod(path, mode)
	return libfs.chmod(loopCtx, path, mode)
end
---@param path string
---@param mode integer
---@param callback StatusCallbackSignature
function fs.chmodAsync(path, mode, callback)
	libfs.chmod(loopCtx, path, mode, callback)
end
---@param path string
---@param mode integer
---@return integer
function fs.chmodAsyncWait(path, mode)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.chmod(loopCtx, path, mode, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param fd integer
---@param mode integer
---@return integer
function fs.fchmod(fd, mode)
	return libfs.fchmod(loopCtx, fd, mode)
end
---@param fd integer
---@param mode integer
---@param callback StatusCallbackSignature
function fs.fchmodAsync(fd, mode, callback)
	libfs.fchmod(loopCtx, fd, mode, callback)
end
---@param fd integer
---@param mode integer
---@return integer
function fs.fchmodAsyncWait(fd, mode)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.fchmod(loopCtx, fd, mode, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param path string
---@param atime number
---@param mtime number
---@return integer
function fs.utime(path, atime, mtime)
	return libfs.utime(loopCtx, path, atime, mtime)
end
---@param path string
---@param atime number
---@param mtime number
---@param callback StatusCallbackSignature
function fs.utimeAsync(path, atime, mtime, callback)
	libfs.utime(loopCtx, path, atime, mtime, callback)
end
---@param path string
---@param atime number
---@param mtime number
---@return integer
function fs.utimeAsyncWait(path, atime, mtime)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.utime(loopCtx, path, atime, mtime, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param fd integer
---@param atime number
---@param utime number
---@return integer
function fs.futime(fd, atime, utime)
	return libfs.futime(loopCtx, fd, atime, utime)
end
---@param fd integer
---@param atime number
---@param utime number
---@param callback StatusCallbackSignature
function fs.futimeAsync(fd, atime, utime, callback)
	libfs.futime(loopCtx, fd, atime, utime, callback)
end
---@param fd integer
---@param atime number
---@param utime number
---@return integer
function fs.futimeAsyncWait(fd, atime, utime)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.futime(loopCtx, fd, atime, utime, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param path string
---@param newPath string
---@return integer
function fs.link(path, newPath)
	return libfs.link(loopCtx, path, newPath)
end
---@param path string
---@param newPath string
---@param callback StatusCallbackSignature
function fs.linkAsync(path, newPath, callback)
	libfs.link(loopCtx, path, newPath, callback)
end
---@param path string
---@param newPath string
---@return integer
function fs.linkAsyncWait(path, newPath)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.link(loopCtx, path, newPath, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param path string
---@param newPath string
---@param flags integer
---@return integer
function fs.symlink(path, newPath, flags)
	return libfs.symlink(loopCtx, path, newPath, flags)
end
---@param path string
---@param newPath string
---@param flags integer
---@param callback StatusCallbackSignature
function fs.symlinkAsync(path, newPath, flags, callback)
	libfs.symlink(loopCtx, path, newPath, flags, callback)
end
---@param path string
---@param newPath string
---@param flags integer
---@return integer
function fs.symlinkAsyncWait(path, newPath, flags)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.symlink(loopCtx, path, newPath, flags, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param path string
---@return string | nil, integer
function fs.readlink(path)
	return libfs.readlink(loopCtx, path)
end
---@param path string
---@param callback fun(str:string | nil, ret:integer):void
function fs.readlinkAsync(path, callback)
	libfs.readlink(loopCtx, path, callback)
end
---@param path string
---@return string | nil, integer
function fs.readlinkAsyncWait(path)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.readlink(loopCtx, path, function(str, ret)
		resume(co, str, ret)
	end)
	return yield()
end
---@param path string
---@return string | nil, integer
function fs.realpath(path)
	return libfs.realpath(loopCtx, path)
end
---@param path string
---@param callback fun(str:string | nil, ret:integer):void
function fs.realpathAsync(path, callback)
	libfs.realpath(loopCtx, path, callback)
end
---@param path string
---@return string | nil, integer
function fs.realpathAsyncWait(path)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.realpath(loopCtx, path, function(str, ret)
		resume(co, str, ret)
	end)
	return yield()
end
---@param path string
---@param uid integer
---@param gid integer
---@return integer
function fs.chown(path, uid, gid)
	return libfs.chown(loopCtx, path, uid, gid)
end
---@param path string
---@param uid integer
---@param gid integer
---@param callback StatusCallbackSignature
function fs.chownAsync(path, uid, gid, callback)
	libfs.chown(loopCtx, path, uid, gid, callback)
end
---@param path string
---@param uid integer
---@param gid integer
---@return integer
function fs.chownAsyncWait(path, uid, gid)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.chown(loopCtx, path, uid, gid, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param fd integer
---@param uid integer
---@param gid integer
---@return integer
function fs.fchown(fd, uid, gid)
	return libfs.fchown(loopCtx, fd, uid, gid)
end
---@param fd integer
---@param uid integer
---@param gid integer
---@param callback StatusCallbackSignature
function fs.fchownAsync(fd, uid, gid, callback)
	libfs.fchown(loopCtx, fd, uid, gid, callback)
end
---@param fd integer
---@param uid integer
---@param gid integer
---@return integer
function fs.fchownAsyncWait(fd, uid, gid)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.fchown(loopCtx, fd, uid, gid, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param path string
---@param uid integer
---@param gid integer
---@return integer
function fs.lchown(path, uid, gid)
	return libfs.lchown(loopCtx, path, uid, gid)
end
---@param path string
---@param uid integer
---@param gid integer
---@param callback StatusCallbackSignature
function fs.lchownAsync(path, uid, gid, callback)
	libfs.lchown(loopCtx, path, uid, gid, callback)
end
---@param path string
---@param uid integer
---@param gid integer
---@return integer
function fs.lchownAsyncWait(path, uid, gid)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.lchown(loopCtx, path, uid, gid, function(ret)
		resume(co, ret)
	end)
	return yield()
end
---@param filePath string
---@return string | nil, integer
function fs.readfile(filePath)
	return libfs.readfile(loopCtx, filePath)
end
---@param filePath string
---@param callback fun(str:string | nil, ret:integer):void
function fs.readfileAsync(filePath, callback)
	libfs.readfile(loopCtx, filePath, callback)
end
---@param filePath string
---@return string | nil, integer
function fs.readfileAsyncWait(filePath)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.readfile(loopCtx, filePath, function(str, ret)
		resume(co, str, ret)
	end)
	return yield()
end
---@param filePath string
---@param data string
---@return integer
function fs.writefile(filePath, data)
	return libfs.writefile(loopCtx, filePath, data)
end
---@param filePath string
---@param data string
---@param callback StatusCallbackSignature
function fs.writefileAsync(filePath, data, callback)
	libfs.writefile(loopCtx, filePath, data, callback)
end
---@param filePath string
---@param data string
---@return integer
function fs.writefileAsyncWait(filePath, data)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libfs.writefile(loopCtx, filePath, data, function(ret)
		resume(co, ret)
	end)
	return yield()
end

---@class libuv_open_flag
---@field public APPEND integer
---@field public CREAT integer
---@field public DIRECT integer
---@field public DIRECTORY integer
---@field public DSYNC integer
---@field public EXCL integer
---@field public EXLOCK integer
---@field public FILEMAP integer
---@field public NOATIME integer
---@field public NOCTTY integer
---@field public NOFOLLOW integer
---@field public NONBLOCK integer
---@field public RANDOM integer
---@field public RDONLY integer
---@field public RDWR integer
---@field public SEQUENTIAL integer
---@field public SHORT_LIVED integer
---@field public SYMLINK integer
---@field public SYNC integer
---@field public TEMPORARY integer
---@field public TRUNC integer
---@field public WRONLY integer

---@type libuv_open_flag
fs.open_flag = libfs.open_flag

---@class libuv_dirent_type
---@field public UNKNOWN integer
---@field public FILE integer
---@field public DIR integer
---@field public LINK integer
---@field public FIFO integer
---@field public SOCKET integer
---@field public CHAR integer
---@field public BLOCK integer

---@type libuv_dirent_type
fs.dirent_type = libfs.dirent_type

---@class libuv_copyfile_flag
---@field public EXCL integer
---@field public FICLONE integer
---@field public FICLONE_FORCE integer

---@type libuv_copyfile_flag
fs.copyfile_flag = libfs.copyfile_flag

---@class libuv_symlink_flag
---@field public DIR integer
---@field public JUNCTION integer

---@type libuv_symlink_flag
fs.symlink_flag = libfs.symlink_flag

-- }======================================================

--[[
** {======================================================
** Loop
** =======================================================
--]]

---@class libuv_loop:table
local loop = {}
libuv.loop = loop

---@class uv_loop_t:lightuserdata

---@return uv_loop_t
function loop.default()
	return libloop.default()
end
---@return uv_loop_t
function loop.Loop()
	return libloop.Loop()
end
---@return integer
function loop.close()
	return close(loopCtx)
end
---@return boolean
function loop.alive()
	return libloop.alive(loopCtx)
end
---@param mode libuv_run_mode
---@return integer
function loop.run(mode)
	return run(loopCtx, mode)
end
function loop.stop()
	libloop.stop(loopCtx)
end
---@return integer
function loop.backendFd()
	return libloop.backend_fd(loopCtx)
end
---@return integer
function loop.backendTimeout()
	return libloop.backend_timeout(loopCtx)
end
---@return integer
function loop.now()
	return libloop.now(loopCtx)
end
function loop.updateTime()
	libloop.update_time(loopCtx)
end
---@param callback fun(handle:uv_handle_t, ptr:lightuserdata):void
---@return table<lightuserdata, uv_handle_t>
function loop.walk(callback)
	return libloop.walk(loopCtx, callback)
end
---@return lightuserdata
function loop.getData()
	return libloop.get_data(loopCtx)
end
---@param ptr lightuserdata
function loop.setData(ptr)
	libloop.set_data(loopCtx, ptr)
end
---@param sigNum integer
function loop.blockSignal(sigNum)
	libloop.block_signal(loopCtx, sigNum)
end

---@class libuv_run_mode
---@field public DEFAULT integer
---@field public ONCE integer
---@field public NOWAIT integer

---@type libuv_run_mode
loop.run_mode = libloop.run_mode

---@type integer
loop.loop_size = libloop.loop_size

-- }======================================================

--[[
** {======================================================
** Network
** =======================================================
--]]

---@class libuv_network:table
local network = {}
libuv.network = network

---@class sockaddr:userdata
---@field public len fun(self:sockaddr):integer
---@field public family fun(self:sockaddr):libuv_address_family
---@field public ip4Addr fun(self:sockaddr, ip:string, port:integer):sockaddr
---@field public ip6Addr fun(self:sockaddr, ip:string, port:integer):sockaddr
---@field public name fun(self:sockaddr):string
---@field public port fun(self:sockaddr):integer
---@field public ip6ScopeId fun(self:sockaddr):integer
---@field public ip6Flowinfo fun(self:sockaddr):integer

---@return sockaddr
function network.SockAddr()
	return libnetwork.SockAddr()
end

---@class addrinfo:table
---@field public flags libuv_addrinfo_flag
---@field public family libuv_address_family
---@field public socktype libuv_socktype
---@field public protocol libuv_protocol
---@field public canonname string
---@field public addr sockaddr

---@overload fun(node:string):integer, addrinfo
---@overload fun(node:string | nil, service:string):integer, addrinfo
---@overload fun(node:string | nil, service:string | nil, hints:addrinfo | nil):integer, addrinfo
---@param node string | nil
---@param service string | nil
---@param hints addrinfo | nil
---@return integer, addrinfo @errCode, addrinfo
function network.getAddrInfo(node, service, hints)
	return libnetwork.getaddrinfo(loopCtx, node, service, hints)
end
---@param node string | nil
---@param service string | nil
---@param hints addrinfo | nil
---@param callback fun(status:integer, res:addrinfo)
function network.getAddrInfoAsync(node, service, hints, callback)
	libnetwork.getaddrinfo(loopCtx, node, service, hints, callback)
end
---@overload fun(node:string):integer, addrinfo
---@overload fun(node:string | nil, service:string):integer, addrinfo
---@overload fun(node:string | nil, service:string | nil, hints:addrinfo | nil):integer, addrinfo
---@param node string | nil
---@param service string | nil
---@param hints addrinfo | nil
---@return integer, addrinfo @errCode, addrinfo
function network.getAddrInfoAsyncWait(node, service, hints)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libnetwork.getaddrinfo(loopCtx, node, service, hints, function(status, res)
		resume(co, status, res)
	end)
	return yield()
end
---@param addr sockaddr
---@param flags libuv_nameinfo_flag
---@return integer, string | nil, string | nil @errCode, host, service
function network.getNameInfo(addr, flags)
	return libnetwork.getnameinfo(loopCtx, addr, flags)
end
---@param addr sockaddr
---@param flags libuv_nameinfo_flag
---@param callback fun(status:integer, host:string | nil, service:string | nil):void
function network.getNameInfoAsync(addr, flags, callback)
	libnetwork.getnameinfo(loopCtx, addr, flags, callback)
end
---@param addr sockaddr
---@param flags libuv_nameinfo_flag
---@return integer, string | nil, string | nil @errCode, host, service
function network.getNameInfoAsyncWait(addr, flags)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	libnetwork.getnameinfo(loopCtx, addr, flags, function(status, hostname, service)
		resume(co, status, hostname, service)
	end)
	return yield()
end

---@class uv_interface_address_t:table
---@field public name string
---@field public phys_addr physaddr
---@field public is_internal boolean
---@field public address sockaddr
---@field public netmask sockaddr

---@return uv_interface_address_t[]
function network.interfaceAddresses()
	return libnetwork.interface_addresses()
end
---@param ifIndex integer
---@return string
function network.ifIndexToName(ifIndex)
	return libnetwork.if_indextoname(ifIndex)
end
---@param ifIndex integer
---@return string
function network.ifIndexToIid(ifIndex)
	return libnetwork.if_indextoiid(ifIndex)
end

---@class physaddr:userdata

---@class libuv_addrinfo_flag
---@field public ADDRCONFIG integer
---@field public ALL integer
---@field public CANONNAME integer
---@field public NUMERICHOST integer
---@field public PASSIVE integer
---@field public V4MAPPED integer
---@field public NUMERICSERV integer @for MacOSX and Linux only
---@field public V4MAPPED_CFG integer @for MacOSX only
---@field public DEFAULT integer @for MacOSX only
---@field public IDN integer @for Linux only
---@field public CANONIDN integer @for Linux only
---@field public IDN_ALLOW_UNASSIGNED integer @for Linux only
---@field public IDN_USE_STD3_ASCII_RULES integer @for Linux only
---@field public NON_AUTHORITATIVE integer @for Windows only
---@field public SECURE integer @for Windows only
---@field public RETURN_PREFERRED_NAMES integer @for Windows only
---@field public FQDN integer @for Windows only
---@field public FILESERVER integer @for Windows only

---@type libuv_addrinfo_flag
network.addrinfo_flag = libnetwork.addrinfo_flag

---@class libuv_nameinfo_flag
---@field public NOFQDN integer
---@field public NUMERICHOST integer
---@field public NAMEREQD integer
---@field public NUMERICSERV integer
---@field public DGRAM integer
---@field public IDN integer @for Linux only
---@field public IDN_ALLOW_UNASSIGNED integer @for Linux only
---@field public IDN_USE_STD3_ASCII_RULES integer @for Linux only

---@type libuv_nameinfo_flag
network.nameinfo_flag = libnetwork.nameinfo_flag

---@class libuv_address_family
---@field public UNSPEC integer
---@field public UNIX integer
---@field public INET integer
---@field public INET6 integer

---@type libuv_address_family
network.address_family = libnetwork.address_family

---@class libuv_protocol
---@field public IP integer
---@field public IPV6 integer
---@field public ICMP integer
---@field public RAW integer
---@field public TCP integer
---@field public UDP integer

---@type libuv_protocol
network.protocol = libnetwork.protocol

---@class libuv_socktype
---@field public STREAM integer
---@field public DGRAM integer
---@field public RAW integer
---@field public SEQPACKET integer

---@type libuv_socktype
network.socktype = libnetwork.socktype

-- }======================================================

--[[
** {======================================================
** OS
** =======================================================
--]]

---@class libuv_os:table
local os = {}
libuv.os = os

---@return string
function os.homeDir()
	return libos.homedir()
end
---@return string
function os.tmpDir()
	return libos.tmpdir()
end

---@class uv_passwd_t:table
---@field public username string
---@field public uid integer
---@field public gid integer
---@field public shell string
---@field public homedir string

---@return uv_passwd_t
function os.getPasswd()
	return libos.get_passwd()
end
---@return table<string, string>
function os.environ()
	return libos.environ()
end
---@param name string
---@return string
function os.getEnv(name)
	return libos.getenv(name)
end
---@param name string
---@param value string
function os.setEnv(name, value)
	libos.setenv(name, value)
end
---@param name string
function os.unsetEnv(name)
	libos.unsetenv(name)
end
---@return string
function os.getHostName()
	return libos.gethostname()
end
---@param pid integer
---@return integer
function os.getPriority(pid)
	return libos.getpriority(pid)
end
---@param pid integer
---@param priority integer
function os.setPriority(pid, priority)
	libos.setpriority(pid, priority)
end

---@class uv_utsname_t:table
---@field public sysname string
---@field public release string
---@field public version string
---@field public machine string

---@return string
function os.uname()
	return libos.uname()
end
---@return integer, integer @sec, usec
function os.getTimeOfDay()
	return libos.gettimeofday()
end

-- }======================================================

--[[
** {======================================================
** System
** =======================================================
--]]

---@class libuv_sys:table
local sys = {}
libuv.sys = sys

---@return integer @rss
function sys.residentSetMemory()
	return libsys.resident_set_memory()
end
---@return number
function sys.upTime()
	return libsys.uptime()
end

---@class uv_rusage_t_timeval:table
---@field public sec integer
---@field public usec integer

---@class uv_rusage_t:table
---@field public utime uv_rusage_t_timeval
---@field public stime uv_rusage_t_timeval
---@field public maxrss integer
---@field public ixrss integer
---@field public idrss integer
---@field public isrss integer
---@field public minflt integer
---@field public majflt integer
---@field public nswap integer
---@field public inblock integer
---@field public oublock integer
---@field public msgsnd integer
---@field public msgrcv integer
---@field public nsignals integer
---@field public nvcsw integer
---@field public nivcsw integer

---@return uv_rusage_t
function sys.getRusage()
	return libsys.getrusage()
end

---@class uv_cpu_info_t_cputimes:table
---@field public nivcsw integer

---@class uv_cpu_info_t:table
---@field public nivcsw integer

---@return uv_cpu_info_t[]
function sys.cpuInfo()
	return libsys.cpu_info()
end
function sys.loadAvg()
	return libsys.loadavg()
end
function sys.getFreeMemory()
	return libsys.get_free_memory()
end
function sys.getTotalMemory()
	return libsys.get_total_memory()
end
function sys.getConstrainedMemory()
	return libsys.get_constrained_memory()
end
function sys.hrTime()
	return libsys.hrtime()
end

-- }======================================================

--[[
** {======================================================
** Thread
** =======================================================
--]]

---@class libuv_thread:table
local thread = {}
libuv.thread = thread

---@type lightuserdata
thread.thread_create = libthread.thread_create
---@type lightuserdata
thread.thread_invalid = libthread.thread_invalid
---@type lightuserdata
thread.thread_self = libthread.thread_self
---@type lightuserdata
thread.thread_join = libthread.thread_join
---@type lightuserdata
thread.thread_equal = libthread.thread_equal

---@class libuv_thread_sem:table
---@field public sem_init lightuserdata
---@field public sem_destroy lightuserdata
---@field public sem_post lightuserdata
---@field public sem_wait lightuserdata
---@field public sem_trywait lightuserdata

---@type libuv_thread_sem
thread.sem = libthread.sem

-- }======================================================

--[[
** {======================================================
** Common
** =======================================================
--]]

---@param callback MemAllocCallback | "function(oldPtr, newPtr, newSize) end"
function libuv.setMemoryAllocatedCallback(callback)
	uvwrap.set_realloc_cb(callback)
end
---@param errCode integer
---@return string
function libuv.errName(errCode)
	return uvwrap.err_name(errCode)
end
---@param errCode integer
---@return string
function libuv.strError(errCode)
	return uvwrap.strerror(errCode)
end
---@param errCode integer
---@return integer
function libuv.translateSysError(errCode)
	return uvwrap.translate_sys_error(errCode)
end
---@param fd integer
---@return libuv_handle_type
function libuv.guessHandle(fd)
	return uvwrap.guess_handle(fd)
end
---@param worker lightuserdata
---@param arg nil | boolean | integer | lightuserdata
---@param callback (fun(result:lightuserdata, status:integer):void) | nil
function libuv.queueWorkAsync(worker, arg, callback)
	queueWork(loopCtx, worker, arg, callback and function(result, status)
		if status ~= OK then printerr("queueWorkAsync callback error: ", status) end
		callback(result, status)
	end or nil)
end
---@param worker lightuserdata
---@param arg nil | boolean | integer | lightuserdata
---@return lightuserdata
function libuv.queueWorkAsyncWait(worker, arg)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	queueWork(loopCtx, worker, arg, function(result, status)
		if status ~= OK then printerr("queueWorkAsyncWait callback error: ", status) end
		resume(co, result, status)
	end)
	return yield()
end

---@alias REPLEvalSignature fun(codeStr:string | nil, eof:boolean):boolean, string, string | nil @running, prompt, history

---@overload fun():void
---@overload fun(callback:REPLEvalSignature):void
---@param callback REPLEvalSignature
function libuv.replStartAsync(callback)
	uvwrap.replStart(loopCtx, false, callback)
end
---@param callback fun(codeStr:string | nil, eof:boolean):void
function libuv.replStartOneShotAsync(callback)
	uvwrap.replStart(loopCtx, true, callback)
end
function libuv.replShutdown()
	uvwrap.replShutdown()
end
---@overload fun(bRunning:boolean):void
---@overload fun(bRunning:boolean, prompt:string):void
---@overload fun(bRunning:boolean, prompt:string, history:string):void
---@param bRunning boolean
---@param prompt string
---@param history string
---@return boolean @running
function libuv.replNext(bRunning, prompt, history)
	return uvwrap.replNext(bRunning, prompt, history)
end
---@return boolean
function libuv.replIsOneShot()
	return uvwrap.replIsOneShot()
end
---@param codeStr string | nil
---@param eof boolean
---@return boolean, string, string | nil @running, prompt, history
function libuv.replDefaultEval(codeStr, eof)
	-- warning: does not support multi instance
	return uvwrap.replDefaultEval(codeStr, eof)
end

---@param prompt string
---@return string
function libuv.replRead(prompt)
	return uvwrap.replRead(prompt)
end
---@param history string
function libuv.replHistory(history)
	uvwrap.replHistory(history)
end

---@type integer
libuv.version = uvwrap.version
---@type string
libuv.version_string = uvwrap.version_string
---@type lightuserdata
libuv.worker_hello = uvwrap.worker_hello

---@class libuv_err_code
---@field public OK integer
---@field public E2BIG integer
---@field public EACCES integer
---@field public EADDRINUSE integer
---@field public EADDRNOTAVAIL integer
---@field public EAFNOSUPPORT integer
---@field public EAGAIN integer
---@field public EAI_ADDRFAMILY integer
---@field public EAI_AGAIN integer
---@field public EAI_BADFLAGS integer
---@field public EAI_BADHINTS integer
---@field public EAI_CANCELED integer
---@field public EAI_FAIL integer
---@field public EAI_FAMILY integer
---@field public EAI_MEMORY integer
---@field public EAI_NODATA integer
---@field public EAI_NONAME integer
---@field public EAI_OVERFLOW integer
---@field public EAI_PROTOCOL integer
---@field public EAI_SERVICE integer
---@field public EAI_SOCKTYPE integer
---@field public EALREADY integer
---@field public EBADF integer
---@field public EBUSY integer
---@field public ECANCELED integer
---@field public ECHARSET integer
---@field public ECONNABORTED integer
---@field public ECONNREFUSED integer
---@field public ECONNRESET integer
---@field public EDESTADDRREQ integer
---@field public EEXIST integer
---@field public EFAULT integer
---@field public EFBIG integer
---@field public EHOSTUNREACH integer
---@field public EINTR integer
---@field public EINVAL integer
---@field public EIO integer
---@field public EISCONN integer
---@field public EISDIR integer
---@field public ELOOP integer
---@field public EMFILE integer
---@field public EMSGSIZE integer
---@field public ENAMETOOLONG integer
---@field public ENETDOWN integer
---@field public ENETUNREACH integer
---@field public ENFILE integer
---@field public ENOBUFS integer
---@field public ENODEV integer
---@field public ENOENT integer
---@field public ENOMEM integer
---@field public ENONET integer
---@field public ENOPROTOOPT integer
---@field public ENOSPC integer
---@field public ENOSYS integer
---@field public ENOTCONN integer
---@field public ENOTDIR integer
---@field public ENOTEMPTY integer
---@field public ENOTSOCK integer
---@field public ENOTSUP integer
---@field public EPERM integer
---@field public EPIPE integer
---@field public EPROTO integer
---@field public EPROTONOSUPPORT integer
---@field public EPROTOTYPE integer
---@field public ERANGE integer
---@field public EROFS integer
---@field public ESHUTDOWN integer
---@field public ESPIPE integer
---@field public ESRCH integer
---@field public ETIMEDOUT integer
---@field public ETXTBSY integer
---@field public EXDEV integer
---@field public UNKNOWN integer
---@field public EOF integer
---@field public ENXIO integer
---@field public EMLINK integer
---@field public EHOSTDOWN integer
---@field public EREMOTEIO integer
---@field public ENOTTY integer
---@field public EFTYPE integer

---@type libuv_err_code
libuv.err_code = uvwrap.err_code

---@class libuv_handle_type
---@field public UNKNOWN_HANDLE integer
---@field public ASYNC integer
---@field public CHECK integer
---@field public FS_EVENT integer
---@field public FS_POLL integer
---@field public HANDLE integer
---@field public IDLE integer
---@field public NAMED_PIPE integer
---@field public POLL integer
---@field public PREPARE integer
---@field public PROCESS integer
---@field public STREAM integer
---@field public TCP integer
---@field public TIMER integer
---@field public TTY integer
---@field public UDP integer
---@field public SIGNAL integer
---@field public FILE integer
---@field public MAX integer

---@type libuv_handle_type
libuv.handle_type = uvwrap.handle_type
libuv.handle_name = uvwrap.handle_name

---@class libuv_alloc_type
---@field public LIBUV_INTERNAL integer
---@field public REQUEST integer
---@field public UVWRAP integer
---@field public BUFFER integer

---@type libuv_alloc_type
libuv.alloc_type = uvwrap.alloc_type

-- }======================================================


---@class libuv_mbio:table
local mbio = {}
libuv.mbio = mbio

local queueWorkAsync = libuv.queueWorkAsync
local queueWorkAsyncWait = libuv.queueWorkAsyncWait

--[[
** {======================================================
** ReadFile
** =======================================================
--]]

---@param fileName string
---@return luaL_MemBuffer | nil, nil | integer, nil | string
function mbio.readFile(fileName)
	return libmbio.readFile(fileName)
end
---@param fileName string
---@param callback fun(mb:luaL_MemBuffer | nil, errCode:nil | integer, errStr:nil | string):void
function mbio.readFileAsync(fileName, callback)
	local ptr = libmbio.packReadFileParam(fileName)
	queueWorkAsync(libmbio.readFilePtr, ptr, function(result, status)
		if status ~= OK then printerr("mbio.readFileAsync callback error: ", status) end
		callback(libmbio.unpackReadFileResult(result))
	end)
end
---@param fileName string
---@return luaL_MemBuffer | nil, nil | integer, nil | string
function mbio.readFileAsyncWait(fileName)
	local _, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	local ptr = libmbio.packReadFileParam(fileName)
	local result = queueWorkAsyncWait(libmbio.readFilePtr, ptr)
	return libmbio.unpackReadFileResult(result)
end

-- }======================================================

--[[
** {======================================================
** WriteFile
** =======================================================
--]]

---@param fileName string
---@param mb luaL_MemBuffer
---@return boolean, nil | integer, nil | string
function mbio.writeFile(fileName, mb)
	libmbio.writeFile(fileName, mb)
end
---@alias MBIOWriteCallback fun(ret:boolean, errCode:nil | integer, errStr:nil | string):void
---@overload fun(fileName:string, mb:luaL_MemBuffer):void
---@overload fun(fileName:string, mb:luaL_MemBuffer, callback:MBIOWriteCallback):void
---@param fileName string
---@param mb luaL_MemBuffer
---@param callback MBIOWriteCallback | nil
function mbio.writeFileAsync(fileName, mb, callback)
	local ptr = libmbio.packWriteFileParam(fileName, mb)
	queueWorkAsync(libmbio.writeFilePtr, ptr, callback and function(result, status)
		if status ~= OK then printerr("mbio.writeFileAsync callback error: ", status) end
		callback(libmbio.unpackWriteFileResult(result))
	end or nil)
end
---@param fileName string
---@param mb luaL_MemBuffer
---@return boolean, nil | integer, nil | string
function mbio.writeFileAsyncWait(fileName, mb)
	local _, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	local ptr = libmbio.packWriteFileParam(fileName, mb)
	local result = queueWorkAsyncWait(libmbio.writeFilePtr, ptr)
	return libmbio.unpackWriteFileResult(result)
end

-- }======================================================

--[[
** {======================================================
** PacketManager
** =======================================================
--]]

---@alias NextPacketSignature fun(self:PacketManager):string | nil

---@class PacketManager:userdata
---@field public packPacket fun(self:PacketManager, data:string):string
---@field public addPackData fun(self:PacketManager, packData:string):void
---@field public getPacket fun(self:PacketManager):libuv_packet_status, string
---@field public eachPacket fun(self:PacketManager):NextPacketSignature, PacketManager
---@field public getRemainForRead fun(self:PacketManager):integer

---@type PacketManager
function libuv.PacketManager()
	return uvwrap.PacketManager()
end

---@class libuv_packet_status
---@field public OK integer
---@field public NeedMore integer
---@field public ErrorLength integer

---@type libuv_packet_status
libuv.packet_status = uvwrap.packet_status

-- }======================================================

return libuv
