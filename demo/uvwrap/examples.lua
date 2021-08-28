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
local fs_event = libuv.fs_event
local event_type = fs_event.event_type
local event_flag = fs_event.event_flag
local signal = libuv.signal
local sig_num = signal.sig_num
local sig_name = signal.sig_name
local sys = libuv.sys
local pipe = libuv.pipe
local tty = libuv.tty
local tty_mode = tty.tty_mode
local handle_type = libuv.handle_type
local handle_name = libuv.handle_name
local udp = libuv.udp
local udp_flag = udp.udp_flag

local idle = libuv.idle
local prepare = libuv.prepare

local handle_type = libuv.handle_type

local err_name = libuv.err_name
local strerror = libuv.strerror
local err_code = libuv.err_code
local OK = err_code.OK
local EOF = err_code.EOF

libuv.set_msgh(function(msg)
	print("In custom msg handler:", msg, debug.traceback())
end)

libuv.defer_run_loop()

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
libuv.set_realloc_cb(function(old, new, sz) -- make sure set realloc callback first
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
	process.setup_args(arg.argc, arg.argv)
end
--]]

local function printf_err(...)
	io.stderr:write(string.format(...))
end

function tick()
	local counter = timer.new()
	local count = 0
	counter:start(function()
		count = count + 1
		print("tick")
		if count >= 10 then
			counter:stop()
			counter = nil
			print("BOOM!")
		end
	end, 1000, 1000)
end

function cgi()
	local function invoke_cgi_script(client)
		local exe_path = arg[0]
		if exe_path:sub(1, 1) ~= "." then
			exe_path = "." / exe_path
		end
		local child_stdio = process.stdio_container(3)
		child_stdio:add(stdio_flag.IGNORE, 0) -- stdin
		child_stdio:add(stdio_flag.INHERIT_STREAM, client) -- stdout
		child_stdio:add(stdio_flag.IGNORE, 0) -- stderr
		local child_handle, status
		child_handle, status = process.new({
			file = exe_path,
			args = {
				exe_path,
				"tick",
			},
			stdio = child_stdio,
			exit_cb = function(exit_status, term_signal)
				print("cgi child exit:", exit_status, term_signal)
				client:close()
				child_handle = nil
			end,
		})
		if child_handle then
			print("Spawn new process:", child_handle)
		else
			printf_err("Spawn new process error %s: %s", err_name(status), strerror(status))
		end
	end
	local server = tcp.new()
	local addr = network.sockaddr()
	addr:ip4_addr("0.0.0.0", 7000)
	server:bind(addr)
	server:listen(128, function(status)
		if status ~= OK then
			print("TCP listen error:", status)
			return
		end
		local client = tcp.new()
		if server:accept(client) == OK then
			invoke_cgi_script(client)
		else
			client:close()
		end
	end)
end

function cgi_client()
	local addr = network.sockaddr()
	addr:ip4_addr("0.0.0.0", 7000)
	local socket = tcp.new()
	local status = socket:connect(addr, function(status)
		if status < 0 then
			print("connect error:", status, strerror(status))
			return
		end
		print("connect status:", status, i, socket:fileno())
		socket:read_start(function(nread, str)
			if nread < 0 then
				if nread == EOF then
					print("Receive EOF")
				end
				socket:close()
				return
			end
			print("Receive:", nread, str)
		end)
	end)
	if status ~= OK then
		printf_err("TCP connect error %s: %s\n", err_name(status), strerror(status))
	end
end

function detach()
	local status, str = pcall(function()
		local exe_path = "sleep"
		local child_handle, status = process.new({
			file = exe_path,
			args = {
				exe_path,
				"100",
			},
			flags = process_flag.DETACHED,
		})
		if child_handle then
			print("Launched sleep with PID", child_handle:get_pid())
			child_handle:unref()
		else
			printf_err("Spawn new process error %s: %s", err_name(status), strerror(status))
		end
	end)
	print("Spawn new process:", status, str)
end

function dns()
	print("irc.freenode.net is... ")
	network.getaddrinfo("irc.freenode.net", "6667", {
		flags = 0,
		family = address_family.INET,
		socktype = socktype.STREAM,
		protocol = protocol.TCP,
	}, function(status, addrinfo)
		if status < 0 then
			print("getaddrinfo callback error", err_name(status), strerror(status))
			return
		end
		local addr = addrinfo.addr
		print(addr)
		local socket = tcp.new()
		local status = socket:connect(addr, function(status)
			if status < 0 then
				print("connect failed error", err_name(status))
				return
			end
			socket:read_start(function(nread, str)
				if nread < 0 then
					if nread ~= EOF then
						print("Read error", err_name(status))
					end
					socket:close()
				end
				print(str)
			end)
		end)
		if status ~= OK then
			printf_err("TCP connect error %s: %s\n", err_name(status), strerror(status))
		end
	end)
end

function hello()
	print("Now quitting.")
end

function idle_basic()
	local idler = idle.new()
	local counter = 0
	local function wait_for_a_while()
		if counter == 0 then
			print("In wait_for_a_while")
		end
		counter = counter + 1
		if counter >= 10e2 then
			idler:stop()
			print("idler stop")
		end
	end
	counter = 0
	idler:start(wait_for_a_while)
	print("Idling...")
	loop.run(nil, loop.run_mode.DEFAULT)
	loop.walk(function(handle, ptr)
		print("======================================")
		print("handle:", handle, "ptr:", ptr)
		if handle then
			local t = handle:get_type()
			print("handle->type:", t, handle_name[t], "handle fileno:", handle:fileno())
			print("handle->loop:", handle:get_loop())
		else
			print("handle has no lua object")
		end
	end)
	counter = 0
	idler:start(wait_for_a_while)
end

function idle_compute()
	local idler = idle.new()
	local function crunch_away()
		print("Computing PI...")
		idler:stop()
	end
	local function on_type(result, str)
		if result == 0 then -- means EOF
			print("Read EOF")
		elseif result < 0 then
			print("error opening file:", strerror(result))
		end
		print("Typed", str)
		fs.read(0, -1, on_type)
		idler:start(crunch_away)
	end
	fs.read(0, -1, on_type)
	idler:start(crunch_away)
end

function interface()
	local ias = network.interface_addresses()
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
	local cpu_infos = sys.cpu_info()
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
			local pipe = pipe.new(true)
			local child_stdio = process.stdio_container(3)
			child_stdio:add(stdio_flag.CREATE_PIPE | stdio_flag.READABLE_PIPE, pipe) -- stdin
			child_stdio:add(stdio_flag.IGNORE, 0) -- stdout
			child_stdio:add(stdio_flag.INHERIT_FD, 2) -- stderr
			local child_handle, status
			child_handle, status = process.new({
				file = exe_path,
				args = {
					exe_path,
					"multi_echo_worker",
				},
				stdio = child_stdio,
				exit_cb = function(exit_status, term_signal)
					printf_err("Process exited with status %d, signal %d\n", exit_status, term_signal)
					child_handle:close()
					child_handle = nil
				end,
			})
			if child_handle then
				print("Spawn new process:", child_handle)
				print("Started worker", child_handle:get_pid())
				table.insert(workers, {
					pipe = pipe,
					child_handle = child_handle,
				})
			else
				printf_err("Spawn new process error %s: %s", err_name(status), strerror(status))
			end
		end

	end
	setup_workers()
	local server = tcp.new()
	local addr = network.sockaddr()
	addr:ip4_addr("0.0.0.0", 7000)
	server:bind(addr)
	server:listen(128, function(status)
		if status ~= OK then
			print("TCP listen error:", status, err_name(status), strerror(status))
			return
		end
		local client = tcp.new()
		if server:accept(client) == OK then
			local worker = workers[round_robin_counter]
			worker.pipe:write2("a", client, function(status) end)
			round_robin_counter = (round_robin_counter % child_worker_count) + 1
		else
			client:close()
		end
	end)
end

function multi_echo_worker()
	local pipe = pipe.new(true)
	pipe:open(0) -- stdin
	pipe:read_start(function(nread, str)
		if nread < 0 then
			if nread ~= EOF then
				printf_err("Read error %s\n", err_name(nread))
			end
			pipe:close()
			return
		end
		if pipe:pending_count() == 0 then
			printf_err("No pending count\n")
		end
		assert(pipe:pending_type() == handle_type.TCP)
		local client = tcp.new()
		if pipe:accept(client) == OK then
			printf_err("Worker %d: Accepted fd %d\n", process.getpid(), client:fileno())
			client:read_start(function(nread, str)
				if nread < 0 then
					if nread ~= EOF then
						printf_err("Read error %s\n", err_name(status))
					end
					client:close()
				end
				client:write(str, function(status)
					if status ~= OK then
						printf_err("Write error %s\n", err_name(status))
					end
				end)
			end)
		else
			client:close()
		end
	end)
end

function multi_echo_hammer()
	local addr = network.sockaddr()
	addr:ip4_addr("0.0.0.0", 7000)
	local PHRASE = "hello world"
	for i = 1, 1000, 1 do -- 3686
		local socket = tcp.new()
		local status = socket:connect(addr, function(status)
			if status < 0 then
				print("connect error:", status, strerror(status))
				return
			end
			print("connect status:", status, i, socket:fileno())
			socket:read_start(function(nread, str)
				if nread < 0 then
					if nread == EOF then
						print("Receive EOF")
					end
					socket:close()
					return
				end
				if str ~= PHRASE then
					socket:close()
					print("Error occur:", str, PHRASE)
					return
				end
				socket:write(PHRASE, function(status) end)
			end)
			socket:write(PHRASE, function(status) end)
		end)
		if status ~= OK then
			printf_err("TCP connect error %s: %s\n", err_name(status), strerror(status))
		end
	end
end

function onchange(command, ...)
	local files = {...}
	if not command or #files == 0 then
		print(string.format("Usage: %s onchange <command> <file1> [file2 ...]", arg[0]))
		return
	end
	onchange_handles = {}
	for _, file in ipairs(files) do
		print("Adding watch on", file)
		local handle = fs_event.new()
		handle:start(function(filename, events, status)
			local event_str = ""
			if events & event_type.RENAME ~= 0 then
				event_str = "renamed"
			elseif events & event_type.CHANGE ~= 0 then
				event_str = "changed"
			end
			print(string.format("Change detected in %s: %s %s", handle:getpath(), event_str, filename))
			os.execute(command)
		end, file, event_flag.RECURSIVE)
		table.insert(onchange_handles, handle)
	end
end

local PIPENAME = os.sysname == "Windows" and "\\\\?\\pipe\\echo.sock" or "/tmp/echo.sock"
function pipe_echo_server()
	local server = pipe.new()
	server:bind(PIPENAME)
	server:listen(128, function(status)
		if status < 0 then
			print("Listen error:", err_name(status), strerror(status))
			return
		end
		local client = pipe.new()
		if server:accept(client) == OK then
			client:read_start(function(nread, str)
				if nread < 0 then
					if nread == EOF then
						print("Read EOF")
					else
						printf_err("Read error %s\n", err_name(r))
					end
					client:close()
					return
				end
				print("Pipe read:", nread, str)
				client:write(str, function(status)
					if status < 0 then
						printf_err("Write error %s\n", err_name(r))
					end
				end)
			end)
		else
			print("Accept failed")
			client:close()
		end
	end)
end

function pipe_echo_client()
	-- local CLIENTNAME = os.sysname == "Windows" and "\\\\?\\pipe\\client.sock" or "/tmp/client.sock"
	local client = pipe.new()
	-- client:bind(CLIENTNAME) -- client should not bind
	client:connect(PIPENAME, function(status)
		if status ~= OK then
			printf_err("Connect error %s\n", err_name(r))
			return
		end
		local str = "Hello World John!"
		client:write(str, function(status)
			if status < 0 then
				printf_err("Write error %s\n", err_name(r))
			else
				client:read_start(function(nread, str)
					if nread < 0 then
						if nread == EOF then
							pirnt("Receive EOF")
						else
							printf_err("Read error %s\n", err_name(r))
						end
					else
						print("Read from Server:", nread, str)
					end
					client:close()
				end)
			end
		end)
	end)
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
	local child_stdio = process.stdio_container(3)
	child_stdio:add(stdio_flag.IGNORE, 0) -- stdin
	child_stdio:add(stdio_flag.IGNORE, 0) -- stdout
	-- child_stdio:add(stdio_flag.INHERIT_FD, 1) -- stdout
	child_stdio:add(stdio_flag.INHERIT_FD, 2) -- stderr
	local child_handle, status
	child_handle, status = process.new({
		file = exe_path,
		args = {
			exe_path,
			"proc_streams_test",
		},
		stdio = child_stdio,
		exit_cb = function(exit_status, term_signal)
			printf_err("Process exited with status %d, signal %d\n", exit_status, term_signal)
			child_handle:close()
			child_handle = nil
		end,
	})
	if child_handle then
		print("Spawn new process:", child_handle)
	else
		printf_err("Spawn new process error %s: %s", err_name(status), strerror(status))
	end
end

function ref_timer()
	local gc = timer.new()
	gc:unref()
	gc:start(function()
		print("Freeing unused objects")
	end, 0, 2000)
	-- could actually be a TCP download or something
	local fake_job = timer.new()
	fake_job:start(function()
		print("Fake job done")
	end, 9000, 0)
end

function signal_action()
	local siga = signal.new()
	siga:start(function(signum)
		print("Signal received A:", sig_name[signum])
		siga:stop()
	end, sig_num.SIGUSR1)
	local sigb = signal.new()
	sigb:start(function(signum)
		print("Signal received B:", sig_name[signum])
		sigb:stop()
	end, sig_num.SIGUSR1)
end

function spawn()
	local exe_path = "mkdir"
	local child_handle, status
	child_handle, status = process.new({
		file = exe_path,
		args = {
			exe_path,
			"test-dir",
		},
		exit_cb = function(exit_status, term_signal)
			printf_err("Process exited with status %d, signal %d\n", exit_status, term_signal)
			child_handle:close()
			child_handle = nil
		end,
	})
	if child_handle then
		print("Launched process with ID", child_handle:get_pid())
	else
		printf_err("Spawn new process error %s: %s", err_name(status), strerror(status))
	end
end

function tcp_echo_server()
	local server = tcp.new()
	local addr = network.sockaddr()
	addr:ip4_addr("0.0.0.0", 7000)
	server:bind(addr)
	server:listen(128, function(status)
		if status < OK then
			printf_err("New connection error %s\n", strerror(status))
			return
		end
		local client = tcp.new()
		if server:accept(client) == OK then
			client:read_start(function(nread, str)
				if nread < 0 then
					if nread ~= EOF then
						printf_err("Read error %s\n", err_name(status))
					else
						print("Receive EOF")
					end
					client:close()
				end
				if nread > 0 then
					client:write(str, function(status)
						if status ~= OK then
							printf_err("Write error %s\n", strerror(status))
						end
					end)
					print(str)
				end
			end)
		else
			client:close()
		end
	end)
end

function tcp_echo_client()
	local addr = network.sockaddr()
	addr:ip4_addr("0.0.0.0", 7000)
	local socket = tcp.new()
	local status = socket:connect(addr, function(status)
		if status < 0 then
			printf_err("connect failed error %s\n", err_name(status))
			return
		end
		socket:write("GoodGoodStudyDayDayUp", function(status)
			if status ~= OK then
				printf_err("write failed error %s\n", err_name(status))
			end
			socket:read_start(function(nread, str)
				if nread < 0 then
					if nread ~= EOF then
						print("Read error", err_name(status))
					end
					socket:close()
					return
				end
				print(str)
				socket:close()
			end)
		end)
	end)
	if status ~= OK then
		printf_err("TCP connect error %s: %s\n", err_name(status), strerror(status))
	end
end

function tty_use()
	local STDOUT_FILENO = 1
	local tty_handle = tty.new(STDOUT_FILENO)
	tty_handle:set_mode(tty_mode.NORMAL)
	if libuv.guess_handle(STDOUT_FILENO) == handle_type.TTY then
		tty_handle:write("\027[41;37m", function(status) end) -- "\033[41;37m" in c
	end
	tty_handle:write("Hello TTY\n", function(status) end)
	tty.reset_mode()
end

function tty_gravity()
	local STDOUT_FILENO = 1
	local tty_handle = tty.new(STDOUT_FILENO)
	tty_handle:set_mode(tty_mode.NORMAL)
	local width, height = tty_handle:get_winsize()
	printf_err("Width %d, height %d\n", width, height)
	tty_handle:write("Hello TTY\n", function(status) end)
	local pos = 0
	local message = "  Hello TTY  "
	local t = timer.new()
	t:start(function()
		-- "\033[2J\033[H\033[%dB\033[%dC\033[42;37m%s" in c
		local fmt = "\027[2J\027[H\027[%dB\027[%dC\027[42;37m%s"
		local str = string.format(fmt, pos, math.floor((width - #message) / 2), message)
		tty_handle:write(str, function(status) end)
		pos = pos + 1
		if pos > height then
			tty.reset_mode()
			t:stop()
			t:close()
		end
	end, 200, 200)
end

function udp_dhcp()
	local recv_socket = udp.new()
	local recv_addr = network.sockaddr()
	recv_addr:ip4_addr("0.0.0.0", 68)
	recv_socket:bind(recv_addr, udp_flag.REUSEADDR)
	recv_socket:recv_start(function(nread, str, addr, flags)
		if nread < 0 then
			printf_err("UDP recv Error %d %s: %s", nread, err_name(nread), strerror(nread))
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
		recv_socket:recv_stop()
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
	local send_socket = udp.new()
	local broadcast_addr = network.sockaddr()
	broadcast_addr:ip4_addr("0.0.0.0", 0)
	send_socket:bind(broadcast_addr)
	send_socket:set_broadcast(true)
	local discover_msg = make_discover_msg()
	local send_addr = network.sockaddr()
	send_addr:ip4_addr("255.255.255.255", 67)
	send_socket:send(discover_msg, send_addr, function(status)
		if status ~= OK then
			printf_err("Send error %s\n", strerror(status))
		end
	end)
end

function uvcat(filename)
	if not filename then return end
	-- async
	fs.open(filename, open_flag.RDONLY, 0, function(fd)
		if fd < 0 then
			printf_err("error opening file: %s\n", strerror(fd))
			return
		end
		local function read_cb(result, str)
			if result < 0 then
				printf_err("Read error: %s\n", strerror(fd))
			elseif result == 0 then -- EOF
				-- synchronous
				fs.close(fd)
			else
				fs.write(1, str, -1, function(result)
					if result < 0 then
						printf_err("Write error: %s\n", strerror(fd))
					else
						fs.read(fd, -1, read_cb)
					end
				end)
			end
		end
		fs.read(fd, -1, read_cb)
	end)
end

function uvstop()
	local idler = idle.new()
	local counter = 0
	idler:start(function()
		print("Idle callback")
		counter = counter + 1
		if counter >= 5 then
			loop.stop()
			print("uv_stop() called")
		end
	end)
	local prep = prepare.new()
	prep:start(function()
		print("Prep callback")
	end)
end

function uvtee(filename)
	if not filename then return end
	local stdin_pipe = pipe.new()
	stdin_pipe:open(0)
	local stdout_pipe = pipe.new()
	stdout_pipe:open(1)
	local fd = fs.open(filename, open_flag.CREAT | open_flag.RDWR, 0o644)
	local file_pipe = pipe.new()
	file_pipe:open(fd)
	stdin_pipe:read_start(function(nread, str)
		if nread < 0 then
			if nread == EOF then
				stdin_pipe:close()
				stdout_pipe:close()
				file_pipe:close()
			else
				printf_err("Pipe read error:", strerror(nread))
			end
		elseif nread > 0 then
			stdout_pipe:write(str, function(status)
				if status ~= OK then
					printf_err("Pipe write error:", strerror(status))
				end
			end)
			file_pipe:write(str, function(status)
				if status ~= OK then
					printf_err("Pipe write error:", strerror(status))
				end
			end)
		end
	end)
end

_G[arg[1] or "hello"](table.unpack(arg, 2))

--[[
loop.run()
local result = loop.close()
print("loop.close result:", result)
--]]
-- PrintMemory()
