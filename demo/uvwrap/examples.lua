#!/usr/bin/env lua

local libuv = require("libuv")
local loop = libuv.loop
local tcp = libuv.tcp
local network = libuv.network
local address_family = network.address_family
local socktype = network.socktype
local protocol = network.protocol
local process = libuv.process
local stdio_flag = process.stdio_flag
local process_flag = process.process_flag
local timer = libuv.timer
local fs = libuv.fs
local open_flag = fs.open_flag
local fsevent = libuv.fsevent
local event_type = fsevent.event_type
local event_flag = fsevent.event_flag
local signal = libuv.signal
local sig_num = signal.sig_num
local sig_name = signal.sig_name
local sys = libuv.sys
local pipe = libuv.pipe
local tty = libuv.tty
local tty_mode = tty.tty_mode
local handle_name = libuv.handle_name
local udp = libuv.udp
local udp_flag = udp.udp_flag

local idle = libuv.idle
local prepare = libuv.prepare

local handle_type = libuv.handle_type

local errName = libuv.errName
local strError = libuv.strError
local err_code = libuv.err_code
local OK = err_code.OK
local EOF = err_code.EOF

util.setErrorMessageHandler(function(msg)
	return "In custom msg handler:" .. msg .. "\n traceback:" .. debug.traceback()
end)

libuv.init()
--libuv.runDeferred()

--[[
local AllocCount = 0
local AllocMem = {}
local function PrintMemory()
	local cnt = 0
	for ptr, sz in pairs(AllocMem) do
		print(ptr, sz)
		cnt = cnt + sz
	end
	print("Current memory:", cnt)
end
libuv.setMemoryAllocatedCallback(function(old, new, sz) -- make sure set realloc callback first
	-- print(old, new, sz, debug.traceback())
	if old == NULL then -- malloc
		AllocCount = AllocCount + 1
		assert(AllocMem[new] == nil, "Error: alloc a used memory address")
		AllocMem[new] = sz
	elseif new == NULL or sz == 0 then -- free
		AllocCount = AllocCount - 1
		assert(AllocMem[old] ~= nil, "Error: free a not used memory address")
		AllocMem[old] = nil
	else -- realloc
		assert(AllocMem[old] ~= nil, "Error: realloc a not used memory address")
		AllocMem[old] = nil
		AllocMem[new] = sz
	end
end)
--]]
--[[
if arg and arg.argc and arg.argv then
	process.setupArgs(arg.argc, arg.argv)
end
--]]

local function printe(...)
	io.stderr:write(string.format(...))
end

function tick()
	local count = 0
	timer.Timer():startAsync(1000, 1000, function(handle)
		count = count + 1
		print("tick")
		if count >= 3 then
			handle:stop()
			print("BOOM!")
		end
	end)
end

function cgi()
	---@param client uv_tcp_t
	local function invoke_cgi_script(client)
		local exe_path = arg[0]
		if exe_path:sub(1, 1) ~= "." then
			exe_path = "." / exe_path
		end
		local child_stdio = process.StdioContainer(3)
		child_stdio:add(stdio_flag.IGNORE, 0) -- stdin
		child_stdio:add(stdio_flag.INHERIT_STREAM, client) -- stdout
		child_stdio:add(stdio_flag.IGNORE, 0) -- stderr
		local child_handle, status
		child_handle, status = process.Process({
			file = exe_path,
			args = {
				exe_path,
				"tick",
			},
			stdio = child_stdio,
			exitCallback = function(exit_status, term_signal)
				print("cgi child exit:", exit_status, term_signal)
				client:closeAsync()
				child_handle = nil
			end,
		})
		if child_handle then
			print("Spawn new process:", child_handle)
		else
			printe("Spawn new process error %s: %s", errName(status), strError(status))
		end
	end
	local addr = network.SockAddr():ip4Addr("0.0.0.0", 7000)
	---@param handle uv_tcp_t
	tcp.Tcp():bind(addr):listenStartAsync(128, function(status, handle)
		if status ~= OK then
			print("TCP listen error:", status)
			return
		end
		local ret, client = handle:accept()
		if ret == OK then
			invoke_cgi_script(client)
		else
			client:closeAsync()
			handle:shutdownAsync()
		end
	end)
end

function cgi_client()
	local addr = network.SockAddr():ip4Addr("0.0.0.0", 7000)
	local status = tcp.Tcp():connectAsync(addr, function(status, socket)
		if status < 0 then
			print("connect error:", status, strError(status))
			return
		end
		print("connect status:", status, i, socket:fileno())
		socket:readStartAsync(function(nread, str, handle)
			if nread < 0 then
				if nread == EOF then
					print("Receive EOF")
				end
				handle:closeAsync()
				return
			end
			print("Receive:", nread, str)
		end)
	end)
	if status ~= OK then
		printe("TCP connect error %s: %s\n", errName(status), strError(status))
	end
end

function detach()
	local status, str = pcall(function()
		local exe_path = "sleep"
		local child_handle, status = process.Process({
			file = exe_path,
			args = {
				exe_path,
				"100",
			},
			flags = process_flag.DETACHED,
		})
		if child_handle then
			print("Launched sleep with PID", child_handle:getPid())
			child_handle:unref()
		else
			printe("Spawn new process error %s: %s", errName(status), strError(status))
		end
	end)
	print("Spawn new process:", status, str)
end

function dns()
	print("irc.freenode.net is... ")
	network.getAddrInfoAsync("irc.freenode.net", "6667", {
		flags = 0,
		family = address_family.INET,
		socktype = socktype.STREAM,
		protocol = protocol.TCP,
	}, function(status, addrinfo)
		if status < 0 then
			print("getaddrinfo callback error", errName(status), strError(status))
			return
		end
		local addr = addrinfo.addr
		print(addr)
		local socket = tcp.Tcp()
		local status = socket:connectAsync(addr, function(status, handle)
			if status < 0 then
				print("connect failed error", errName(status))
				return
			end
			handle:readStartAsync(function(nread, str, h)
				if nread < 0 then
					if nread ~= EOF then
						print("Read error", errName(status))
					end
					h:closeAsync()
				end
				print(str)
			end)
		end)
		if status ~= OK then
			printe("TCP connect error %s: %s\n", errName(status), strError(status))
		end
	end)
end

function hello()
	print("Now quitting.")
end

function idle_basic()
	local idler = idle.Idle()
	local counter = 0
	---@param handle uv_idle_t
	local function wait_for_a_while(handle)
		if counter == 0 then
			print("In wait_for_a_while")
		end
		counter = counter + 1
		if counter >= 10e2 then
			handle:stop()
			idler:stop()
			print("idler stop")
		end
	end
	counter = 0
	idler:startAsync(wait_for_a_while)
	print("Idling...")
	loop.run(loop.run_mode.DEFAULT)
	loop.walk(function(handle, ptr)
		print("======================================")
		print("handle:", handle, "ptr:", ptr)
		if handle then
			local t = handle:getType()
			print("handle->type:", t, handle_name[t], "handle fileno:", handle:fileno())
			print("handle->loop:", handle:getLoop())
		else
			print("handle has no lua object")
		end
	end)
	counter = 0
	idler:startAsync(wait_for_a_while)
end

function idle_compute()
	local idler = idle.Idle()
	local function crunch_away()
		print("Computing PI...")
		idler:stop()
	end
	local function on_type(str, result)
		if result == 0 then -- means EOF
			print("Read EOF")
			return
		elseif result < 0 then
			print("error opening file:", strError(result))
		end
		print("Typed", str)
		fs.readAsync(0, -1, on_type)
		idler:startAsync(crunch_away)
	end
	fs.readAsync(0, -1, on_type)
	idler:startAsync(crunch_away)
end

function interface()
	local ias = network.interfaceAddresses()
	print("Number of interfaces:", #ias)
	for _, ia in ipairs(ias) do
		print("Name:", ia.name)
		print("Internal? ", ia.is_internal and "Yes" or "No")
		local addr = ia.address
		if addr:family() == address_family.INET then
			print("IPv4 address:", addr:name())
		elseif addr:family() == address_family.INET6 then
			print("IPv6 address:", addr:name())
		end
	end
end

function multi_echo_server()
	local workers = {}
	local cpu_infos = sys.cpuInfo()
	-- print(tostring(cpu_infos, 16))
	local child_worker_count = #cpu_infos
	local round_robin_counter = 1
	local function setup_workers()
		local exe_path = arg[0]
		if exe_path:sub(1, 1) ~= "." then
			exe_path = "." / exe_path
		end
		print(exe_path)
		for i = 1, child_worker_count, 1 do
			local pipe = pipe.Pipe(true)
			local child_stdio = process.StdioContainer(3)
			child_stdio:add(stdio_flag.CREATE_PIPE | stdio_flag.READABLE_PIPE, pipe) -- stdin
			child_stdio:add(stdio_flag.IGNORE, 0) -- stdout
			child_stdio:add(stdio_flag.INHERIT_FD, 2) -- stderr
			---@type uv_process_t
			local child_handle
			local status
			child_handle, status = process.Process({
				file = exe_path,
				args = {
					exe_path,
					"multi_echo_worker",
				},
				stdio = child_stdio,
				exit_cb = function(exit_status, term_signal)
					printe("Process exited with status %d, signal %d\n", exit_status, term_signal)
					child_handle:closeAsync()
					child_handle = nil
				end,
			})
			if child_handle then
				print("Spawn new process:", child_handle)
				print("Started worker", child_handle:getPid())
				table.insert(workers, {
					pipe = pipe,
					child_handle = child_handle,
				})
			else
				printe("Spawn new process error %s: %s", errName(status), strError(status))
			end
		end

	end
	setup_workers()
	local addr = network.SockAddr():ip4Addr("0.0.0.0", 7000)
	---@param server uv_tcp_t
	tcp.Tcp():bind(addr):listenStartAsync(128, function(status, server)
		if status ~= OK then
			print("TCP listen error:", status, errName(status), strError(status))
			return
		end
		local client = tcp.Tcp()
		if server:accept(client) == OK then
			local worker = workers[round_robin_counter]
			---@type uv_pipe_t
			local p = worker.pipe
			p:write2Async("a", client, function(status) end)
			round_robin_counter = (round_robin_counter % child_worker_count) + 1
		else
			client:closeAsync()
		end
	end)
end

function multi_echo_worker()
	---@param handle uv_pipe_t
	pipe.Pipe(true):open(0) --[[ stdin ]]:readStartAsync(function(nread, str, handle)
		if nread < 0 then
			if nread ~= EOF then
				printe("Read error %s\n", errName(nread))
			end
			handle:closeAsync()
			return
		end
		if handle:pendingCount() == 0 then
			printe("No pending count\n")
		end
		assert(handle:pendingType() == handle_type.TCP)
		local client = tcp.Tcp()
		if handle:accept(client) == OK then
			printe("Worker %d: Accepted fd %d\n", process.getPid(), client:fileno())
			client:readStartAsync(function(nread, str, clientHandle)
				if nread < 0 then
					if nread ~= EOF then
						printe("Read error %s\n", errName(status))
					end
					clientHandle:closeAsync()
					return
				end
				clientHandle:writeAsync(str, function(status, clientHandle)
					if status ~= OK then
						printe("Write error %s\n", errName(status))
					end
				end)
			end)
		else
			client:closeAsync()
		end
	end)
end

function multi_echo_hammer()
	local addr = network.SockAddr():ip4Addr("0.0.0.0", 7000)
	local PHRASE = "hello world"
	for i = 1, 1000, 1 do -- 3686
		local status = tcp.Tcp():connectAsync(addr, function(status, socket)
			if status < 0 then
				print("connect error:", status, strError(status))
				return
			end
			print("connect status:", status, i, socket:fileno())
			socket:readStartAsync(function(nread, str, handle)
				if nread < 0 then
					if nread == EOF then
						print("Receive EOF")
					end
					handle:closeAsync()
					return
				end
				if str ~= PHRASE then
					handle:closeAsync()
					print("Error occur:", str, PHRASE)
					return
				end
				--socket:writeAsync(PHRASE, function(status) end)
				print("Receive echo succeed!", i)
				handle:closeAsync()
			end)
			socket:writeAsync(PHRASE, function(status) end)
		end)
		if status ~= OK then
			printe("TCP connect error %s: %s\n", errName(status), strError(status))
		end
	end
end

function onchange(command, ...)
	local files = {...}
	if not command or #files == 0 then
		print(string.format("Usage: %s onchange <command> <file1> [file2 ...]", arg[0]))
		return
	end
	local cnt = 0;
	for _, file in ipairs(files) do
		print("Adding watch on", file)
		fsevent.FsEvent():startAsync(function(filename, events, status, handle)
			local event_str = ""
			if events & event_type.RENAME ~= 0 then
				event_str = "renamed"
			elseif events & event_type.CHANGE ~= 0 then
				event_str = "changed"
			end
			print(string.format("Change detected in %s: %s %s", handle:getPath(), event_str, filename))
			os.execute(command)
			cnt = cnt + 1
			if cnt >= 3 then handle:stop() end
		end, file, event_flag.RECURSIVE)
	end
end

local PIPENAME = os.sysname == "Windows" and "\\\\?\\pipe\\echo.sock" or "/tmp/echo.sock"
function pipe_echo_server()
	---@param server uv_pipe_t
	pipe.Pipe():bind(PIPENAME):listenStartAsync(128, function(status, server)
		if status < 0 then
			print("Listen error:", errName(status), strError(status))
			return
		end
		local client = pipe.Pipe()
		if server:accept(client) == OK then
			client:readStartAsync(function(nread, str, client)
				if nread < 0 then
					if nread == EOF then
						print("Read EOF")
					else
						printe("Read error %s\n", errName(nread))
					end
					client:closeAsync()
					return
				end
				print("Pipe read:", nread, str)
				client:writeAsync(str, function(status, client)
					if status < 0 then
						printe("Write error %s\n", errName(nread))
					end
				end)
			end)
		else
			print("Accept failed")
			client:closeAsync()
		end
	end)
end

function pipe_echo_client()
	-- local CLIENTNAME = os.sysname == "Windows" and "\\\\?\\pipe\\client.sock" or "/tmp/client.sock"
	local client = pipe.Pipe()
	-- client:bind(CLIENTNAME) -- client should not bind
	--[[
	client:connectAsync(PIPENAME, function(status)
		if status ~= OK then
			printe("Connect error %s\n", errName(r))
			return
		end
		local str = "Hello World John!"
		client:writeAsync(str, function(status)
			if status < 0 then
				printe("Write error %s\n", errName(r))
			else
				client:readStartAsync(function(nread, str)
					if nread < 0 then
						if nread == EOF then
							pirnt("Receive EOF")
						else
							printe("Read error %s\n", errName(r))
						end
					else
						print("Read from Server:", nread, str)
					end
					client:closeAsync()
				end)
			end
		end)
	end)
	--]]
	---[[
	cocall(function()
		local status = client:connectAsyncWait(PIPENAME)
		if status ~= OK then
			printe("Connect error %s\n", errName(status))
			return
		end
		local str = "Hello World John!"
		tmp = debug.getregistry()
		local status = client:writeAsyncWait(str)
		if status < 0 then
			printe("Write error %s\n", errName(status))
		else
			client:readStartAsync(function(nread, str, client)
				if nread < 0 then
					if nread == EOF then
						pirnt("Receive EOF")
					else
						printe("Read error %s\n", errName(status))
					end
				else
					print("Read from Server:", nread, str)
				end
				client:closeAsync()
			end)
		end
	end)
	--]]
end

function proc_streams_test()
	io.stderr:write("This is stderr\n")
	io.stdout:write("This is stdout\n")
end

function proc_streams()
	local exe_path = arg[0]
	if exe_path:sub(1, 1) ~= "." then
		exe_path = "." / exe_path
	end
	local child_stdio = process.StdioContainer(3)
	child_stdio:add(stdio_flag.IGNORE, 0) -- stdin
	child_stdio:add(stdio_flag.IGNORE, 0) -- stdout
	-- child_stdio:add(stdio_flag.INHERIT_FD, 1) -- stdout
	child_stdio:add(stdio_flag.INHERIT_FD, 2) -- stderr
	local child_handle, status
	child_handle, status = process.Process({
		file = exe_path,
		args = {
			exe_path,
			"proc_streams_test",
		},
		stdio = child_stdio,
		exitCallback = function(exit_status, term_signal)
			printe("Process exited with status %d, signal %d\n", exit_status, term_signal)
			child_handle:closeAsync()
			child_handle = nil
		end,
	})
	if child_handle then
		print("Spawn new process:", child_handle)
	else
		printe("Spawn new process error %s: %s", errName(status), strError(status))
	end
end

function ref_timer()
	local gc = timer.Timer()
	gc:unref()
	gc:startAsync(0, 2000, function(handle)
		print("Freeing unused objects")
	end)
	-- could actually be a TCP download or something
	local fake_job = timer.Timer()
	fake_job:startOneShotAsync(9000, function(handle)
		print("Fake job done")
		gc:stop()
	end)
end

function signal_action()
	signal.Signal():startAsync(sig_num.SIGUSR1, function(signum, handle)
		print("Signal received A:", sig_name[signum])
		handle:stop()
	end)
	signal.Signal():startAsync(sig_num.SIGUSR1, function(signum, handle)
		print("Signal received B:", sig_name[signum])
		handle:stop()
	end)
end

function spawn()
	local exe_path = "mkdir"
	---@type uv_process_t
	local child_handle
	local status
	child_handle, status = process.Process({
		file = exe_path,
		args = {
			exe_path,
			"test-dir",
		},
		exitCallback = function(exit_status, term_signal)
			printe("Process exited with status %d, signal %d\n", exit_status, term_signal)
			child_handle:closeAsync()
			child_handle = nil
		end,
	})
	if child_handle then
		print("Launched process with ID", child_handle:getPid())
	else
		printe("Spawn new process error %s: %s", errName(status), strError(status))
	end
end

function tcp_echo_server()
	local addr = network.SockAddr():ip4Addr("0.0.0.0", 7000)
	tcp.Tcp():bind(addr):listenStartAsync(128, function(status, server)
		if status < OK then
			printe("New connection error %s\n", strError(status))
			return
		end
		local client = tcp.Tcp()
		if server:accept(client) == OK then
			client:readStartAsync(function(nread, str, client)
				if nread < 0 then
					if nread ~= EOF then
						printe("Read error %s\n", errName(status))
					else
						print("Receive EOF")
					end
					client:closeAsync()
				end
				if nread > 0 then
					client:writeAsync(str, function(status, client)
						if status ~= OK then
							printe("Write error %s\n", strError(status))
						end
					end)
					print(str)
				end
			end)
		else
			client:closeAsync()
		end
	end)
end

function tcp_echo_client()
	local addr = network.SockAddr():ip4Addr("0.0.0.0", 7000)
	local status = tcp.Tcp():connectAsync(addr, function(status, socket)
		if status < 0 then
			printe("connect failed error %s\n", errName(status))
			return
		end
		socket:writeAsync("GoodGoodStudyDayDayUp", function(status, socket)
			if status ~= OK then
				printe("write failed error %s\n", errName(status))
			end
			socket:readStartAsync(function(nread, str, socket)
				if nread < 0 then
					if nread ~= EOF then
						print("Read error", errName(status))
					end
					socket:closeAsync()
					return
				end
				print(str)
				socket:closeAsync()
			end)
		end)
	end)
	if status ~= OK then
		printe("TCP connect error %s: %s\n", errName(status), strError(status))
	end
end

function tty_use()
	local STDOUT_FILENO = 1
	local tty_handle = tty.Tty(STDOUT_FILENO)
	tty_handle:setMode(tty_mode.NORMAL)
	if libuv.guessHandle(STDOUT_FILENO) == handle_type.TTY then
		tty_handle:writeAsync("\027[41;37m", function(status) end) -- "\033[41;37m" in c
	end
	tty_handle:writeAsync("Hello TTY\n", function(status) end)
	tty.resetMode()
end

function tty_gravity()
	local STDOUT_FILENO = 1
	local tty_handle = tty.Tty(STDOUT_FILENO)
	tty_handle:setMode(tty_mode.NORMAL)
	local width, height = tty_handle:getWinSize()
	printe("Width %d, height %d\n", width, height)
	tty_handle:writeAsync("Hello TTY\n", function(status) end)
	local pos = 0
	local message = "  Hello TTY  "
	timer.Timer():startAsync(200, 200, function(handle)
		-- "\033[2J\033[H\033[%dB\033[%dC\033[42;37m%s" in c
		local fmt = "\027[2J\027[H\027[%dB\027[%dC\027[42;37m%s"
		local str = string.format(fmt, pos, math.floor((width - #message) / 2), message)
		tty_handle:writeAsync(str, function(status, handle) end)
		pos = pos + 1
		if pos > height then
			tty.resetMode()
			handle:stop()
		end
	end)
end

function udp_dhcp()
	local recv_addr = network.SockAddr():ip4Addr("0.0.0.0", 68)
	udp.Udp():bind(recv_addr, udp_flag.REUSEADDR):recvStartAsync(function(nread, str, addr, flags, recv_socket)
		if nread < 0 then
			printe("UDP recv Error %d %s: %s", nread, errName(nread), strError(nread))
		elseif nread == OK then
			if addr then
				print("We recv a empty packet")
			else
				print("There is nothing to read")
			end
		else
			print("Recv from ", addr)
			print("Offered IP", str:byte(4 * 4 + 1, 4 * 4 + 4))
		end
		recv_socket:recvStop()
	end)
	local function make_discover_msg()
		local randnum = math.random(1, 2^15)
		local rand1 = randnum % 256
		local rand2 = math.floor(randnum / 256) % 256
		local rand3 = math.floor(randnum / 256 / 256) % 256
		local rand4 = math.floor(randnum / 256 / 256 / 256) % 256

		local buffer = setmetatable({
			-- BOOTREQUEST
			[0] = 0x1,
			-- HTYPE ethernet
			[1] = 0x1,
			-- HLEN
			[2] = 0x6,
			-- HOPS
			[3] = 0x0,
			-- XID 4 bytes
			[4] = rand1,
			[5] = rand2,
			[6] = rand3,
			[7] = rand4,
			-- SECS
			[8] = 0x0,
			-- FLAGS
			[10] = 0x80,
			-- CIADDR 12-15 is all zeros
			-- YIADDR 16-19 is all zeros
			-- SIADDR 20-23 is all zeros
			-- GIADDR 24-27 is all zeros
			-- CHADDR 28-43 is the MAC address, use your own
			[28] = 0xe4,
			[29] = 0xce,
			[30] = 0x8f,
			[31] = 0x13,
			[32] = 0xf6,
			[33] = 0xd4,
			-- SNAME 64 bytes zero
			-- FILE 128 bytes zero
			-- OPTIONS
			-- - magic cookie
			[236] = 99,
			[237] = 130,
			[238] = 83,
			[239] = 99,
			-- DHCP Message type
			[240] = 53,
			[241] = 1,
			[242] = 1, -- DHCPDISCOVER
			-- DHCP Parameter request list
			[243] = 55,
			[244] = 4,
			[245] = 1,
			[246] = 3,
			[247] = 15,
			[248] = 6,
		}, {
			__index = function(self, idx)
				return 0
			end,
		})
		return string.char(function(idx)
			if idx > 256 then return nil end
			return buffer[idx - 1]
		end)
	end
	local broadcast_addr = network.SockAddr():ip4Addr("0.0.0.0", 0)
	local discover_msg = make_discover_msg()
	local send_addr = network.SockAddr():ip4Addr("255.255.255.255", 67)
	udp.Udp():bind(broadcast_addr):setBroadcast(true):sendAsync(discover_msg, send_addr, function(status, send_socket)
		if status ~= OK then
			printe("Send error %s\n", strError(status))
		end
	end)
end

function uvcat(filename)
	if not filename then return end
	-- async
	fs.openAsync(filename, open_flag.RDONLY, 0, function(fd)
		if fd < 0 then
			printe("error opening file: %s\n", strError(fd))
			return
		end
		local function read_cb(str, result)
			if result < 0 then
				printe("Read error: %s\n", strError(fd))
			elseif result == 0 then -- EOF
				-- synchronous
				fs.close(fd)
			else
				fs.writeAsync(1, str, -1, function(result)
					if result < 0 then
						printe("Write error: %s\n", strError(fd))
					else
						fs.readAsync(fd, -1, read_cb)
					end
				end)
			end
		end
		fs.readAsync(fd, -1, read_cb)
	end)
end

function uvstop()
	local counter = 0
	idle.Idle():startAsync(function(handle)
		print("Idle callback")
		counter = counter + 1
		if counter >= 5 then
			loop.stop()
			print("uv_stop() called")
		end
	end)
	prepare.Prepare():startAsync(function(handle)
		print("Prep callback")
	end)
end

function uvtee(filename)
	if not filename then return end
	local stdin_pipe = pipe.Pipe():open(0)
	local stdout_pipe = pipe.Pipe():open(1)
	local fd = fs.open(filename, open_flag.CREAT | open_flag.RDWR, 6 * 64 + 4 * 8 + 4) -- 0o644
	local file_pipe = pipe.Pipe():open(fd)
	stdin_pipe:readStartAsync(function(nread, str, stdin_pipe)
		print("Callback: ", nread, stdin_pipe, stdin_pipe:isReadable())
		if nread <= 0 then
			if nread == EOF then
				print("Read EOF")
				stdin_pipe:readStop()
				stdout_pipe:closeAsync()
				file_pipe:closeAsync()
			else
				print("Pipe read error:", strError(nread))
			end
		elseif nread > 0 then
			stdout_pipe:writeAsync(str, function(status, handle)
				if status ~= OK then
					printe("Pipe write error:", strError(status))
				end
			end)
			file_pipe:writeAsync(str, function(status, handle)
				if status ~= OK then
					printe("Pipe write error:", strError(status))
				end
			end)
		end
	end)
end

_G[arg[1] or "hello"](table.unpack(arg, 2))

libuv.run()
collectgarbage()
libuv.run()
collectgarbage()
libuv.run()
print("All handle:")
loop.walk(function(handle, ptr)
	print(handle, ptr)
end)
libuv.close()

--[[
loop.run()
local result = loop.close()
print("loop.close result:", result)
--]]
-- PrintMemory()
