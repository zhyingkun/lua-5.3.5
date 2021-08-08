#!/usr/local/zyk/lua/bin/lua
-- #!/usr/bin/env lua

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
local fs_event = libuv.fs_event
local event_type = fs_event.event_type
local event_flag = fs_event.event_flag
local signal = libuv.signal
local sig_num = signal.sig_num
local sig_name = signal.sig_name
local sys = libuv.sys
local pipe = libuv.pipe

local idle = libuv.idle

local handle_type = libuv.handle_type

local err_name = libuv.err_name
local strerror = libuv.strerror
local err_code = libuv.err_code
local OK = err_code.OK
local EOF = err_code.EOF

libuv.set_msgh(function(msg)
	print("In custom msg handler:", msg)
end)

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
		local child_stdio = process.stdio_container(3)
		child_stdio:add(stdio_flag.IGNORE, 0) -- stdin
		child_stdio:add(stdio_flag.INHERIT_STREAM, client) -- stdout
		child_stdio:add(stdio_flag.IGNORE, 0) -- stderr
		local status, str = pcall(function()
			local child_handle
			child_handle = process.new({
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
		end)
		print("Spawn new process:", status, str)
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

function detach()
	local status, str = pcall(function()
		local exe_path = "sleep"
		local child_handle = process.new({
			file = exe_path,
			args = {
				exe_path,
				"100",
			},
			flags = process_flag.DETACHED,
		})
		print("Launched sleep with PID", child_handle:get_pid())
		child_handle:unref()
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
		socket:connect(addr, function(status)
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
	end)
end

function hello()
	print("Now quitting.")
end

function idle_basic()
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
	local idler = idle.new()
	counter = 0
	idler:start(wait_for_a_while)
	print("Idling...")
	loop.run(loop.run_mode.DEFAULT)
	loop.walk(function(handle, ptr)
		print("======================================")
		print("handle:", handle, "ptr:", ptr)
		if handle then
			print("handle->type:", handle:get_type(), handle:type_name(), "handle fileno:", handle:fileno())
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
			local child_handle
			local status, str = pcall(function()
				child_handle = process.new({
					file = exe_path,
					args = {
						exe_path,
						"multi_echo_worker",
					},
					stdio = child_stdio,
					exit_cb = function(exit_status, term_signal)
						io.stderr:write(string.format("Process exited with status %d, signal %d\n", exit_status, term_signal))
						child_handle:close()
						child_handle = nil
					end,
				})
			end)
			if not status then
				print("Spawn new process error:", status, str)
				return
			end
			print("Started worker", child_handle:get_pid())
			table.insert(workers, {
				pipe = pipe,
				child_handle = child_handle,
			})
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
				io.stderr:write("Read error " .. err_name(nread) .. "\n")
			end
			pipe:close()
			return
		end
		if pipe:pending_count() == 0 then
			io.stderr:write("No pending count\n")
		end
		assert(pipe:pending_type() == handle_type.TCP)
		local client = tcp.new()
		if pipe:accept(client) == OK then
			io.stderr:write(string.format("Worker %d: Accepted fd %d\n", process.getpid(), client:fileno()))
			client:read_start(function(nread, str)
				if nread < 0 then
					if nread ~= EOF then
						io.stderr:write(string.format("Read error %s\n", err_name(status)))
					end
					client:close()
				end
				client:write(str, function(status)
					if status ~= OK then
						io.stderr:write(string.format("Write error %s\n", err_name(status)))
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
	for i = 1, 3700, 1 do -- 3686
		local socket = tcp.new()
		local status, str = pcall(function()
			socket:connect(addr, function(status)
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
		end)
		if not status then
			print("connect error:", str)
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

function pipe_echo_server()

end

function proc_streams_test()
	io.stderr:write("This is stderr\n")
	io.stdout:write("This is stdout\n")
end

function proc_streams()
	local exe_path = arg[0]
	local child_stdio = process.stdio_container(3)
	child_stdio:add(stdio_flag.IGNORE, 0) -- stdin
	child_stdio:add(stdio_flag.IGNORE, 0) -- stdout
	-- child_stdio:add(stdio_flag.INHERIT_FD, 1) -- stdout
	child_stdio:add(stdio_flag.INHERIT_FD, 2) -- stderr
	local status, str = pcall(function()
		local child_handle
		child_handle = process.new({
			file = exe_path,
			args = {
				exe_path,
				"proc_streams_test",
			},
			stdio = child_stdio,
			exit_cb = function(exit_status, term_signal)
				io.stderr:write(string.format("Process exited with status %d, signal %d\n", exit_status, term_signal))
				child_handle:close()
				child_handle = nil
			end,
		})
	end)
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
end

function tcp_echo_server()
end

function tty()
end

function tty_gravity()
end

function udp_dhcp()
end

function uvcat()
	print("Start ...")
	local libuv = require("libuv")
	local fs = libuv.fs
	local filename = "./word.txt"
	local filerename = "./reword.txt"
	
	-- async
	fs.open(filename, fs.open_flag.RDONLY, 0, function(fd)
		print("In callback of fs.open:", fd)
		local function read_cb(result, str)
			print("In callback of fs.read:", result)
			if str ~= nil then
				print("Read from file:", str)
				fs.read(fd, -1, read_cb)
			else
				fs.close(fd, function()
					print("In callback of fs.close")
				end)
			end
		end
		fs.read(fd, -1, read_cb)
	end)
	
	local ret = libuv.rundefault()
	print("ret from libuv.run:", ret)
	
	-- sync
	local fd = fs.open(filename, fs.open_flag.RDONLY, 0)
	print("after sync fs.open:", fd)
	local result, str = fs.read(fd, -1)
	print("after sync fs.read:", result, str)
	local ret = fs.close(fd)
	print("after sync fs.close:", ret)
	
	local ret = fs.rename(filename, filerename)
	print("after sync fs.rename:", ret)
	
	local ret = libuv.rundefault()
	print("ret from libuv.run:", ret)
	
	print("End.")	
end

function uvstop()
end

function uvtee()
end

function uvwget()
end

_G[arg[1] or "hello"](table.unpack(arg, 2))
