local libuv = require("libuv")
local tcp = libuv.tcp
local udp = libuv.udp
local network = libuv.network
local timer = libuv.timer

local err_name = libuv.err_name
local strerror = libuv.strerror
local err_code = libuv.err_code
local OK = err_code.OK
local EOF = err_code.EOF


libuv.set_msgh(function(msg)
	print("In custom msg handler:", msg, debug.traceback())
end)

libuv.defer_run_loop()

local function TcpServer()
	local tcpSocket = tcp.new()
	local sockAddr = network.sockaddr()
	sockAddr:ip4_addr("0.0.0.0", 1024)
	tcpSocket:bind(sockAddr)
	tcpSocket:listen(128, function(status)
		if status ~= OK then
			print("TCP listen error:", status, err_name(status), strerror(status))
			return
		end
		local tcpConnection = tcp.new()
		if tcpSocket:accept(tcpConnection) == OK then
			print("TCP receive a connection: fd is", tcpConnection:fileno())
			tcpConnection:read_start(function(nread, str)
				if nread < 0 then
					if nread == EOF then
						print("TCP Connection Read EOF")
					else
						print("TCP read error:", nread, err_name(nread), strerror(nread))
					end
					tcpConnection:close(function()
						print("TCP on connection close")
					end)
					return
				end
				print("TCP Receive:", nread, nread > 100 and "" or str)
			end)
		end
	end)
end

local function UdpServer()
	local udpSocket = udp.new()
	local sockAddr = network.sockaddr()
	sockAddr:ip4_addr("0.0.0.0", 1025)
	udpSocket:bind(sockAddr)
	udpSocket:recv_start(function(nread, str, addr, flags)
		if nread < 0 then
			print("UDP recv error:", nread, err_name(nread), strerror(nread))
			print(udpSocket:getsockname())
			return
		end
		if nread == 0 then
			if addr then
				print("We recv a empty packet")
			else
				print("There is nothing to read")
			end
		else
			print("UDP recv:", nread, addr, nread > 100 and "" or str)
		end
	end)
end

local function Server()
	TcpServer()
	UdpServer()
end

local function TcpClient()
	local tcpClient = tcp.new()
	local sockAddr = network.sockaddr()
	sockAddr:ip4_addr("0.0.0.0", 1024)
	tcpClient:connect(sockAddr, function(status)
		if status < 0 then
			print("TCP Connect error:", status, err_name(status), strerror(status))
			return
		end
		print("Connect succeed: fd is", tcpClient:fileno())
		local function WriteHello(idx)
			local hello = "Good Good Study Day Day Up!"
			tcpClient:write(hello, function(status)
				if status == OK then
					print("Write Complete:", #hello, idx)
				else
					print(idx, err_name(status), strerror(status))
				end
			end)
		end
		for i = 1, 3, 1 do
			WriteHello(i)
		end
		local delay = timer.new()
		delay:start(function()
			delay:stop()
			local str = "="
			local len = 128
			local buffer = str:rep(len * 1024)
			tcpClient:write(buffer, function(status)
				if status == OK then
					print("Write Complete:", len .. "K")
				else
					print(err_name(status), strerror(status))
				end
			end)
		end, 1000, 0)
	end)
end

local function UdpClient()
	local udpClient = libuv.udp.new()
	local sockAddr = network.sockaddr()
	sockAddr:ip4_addr("127.0.0.1", 1025)

	local function WriteHello(idx)
		local hello = "Hello World!"
		udpClient:send(hello, sockAddr, function(status)
			if status == OK then
				print("UDP Send Complete:", #hello, udpClient:getsockname(), idx)
			else
				print(idx, err_name(status), strerror(status))
			end
		end)
	end
	for i = 1, 3, 1 do
		WriteHello(i)
	end

	local str = "="

	udpClient:send_buffer_size(65536)
	local len = 65507
	-- local len = 65508

	-- local len = 9216
	-- local len = 9217

	local buffer = str:rep(len)
	udpClient:send(buffer, sockAddr, function(status)
		if status == OK then
			print("UDP Send Complete:", len, udpClient:getsockname())
		else
			print(err_name(status), strerror(status))
		end
	end)
end

local features = setmetatable({
	TcpServer = TcpServer,
	UdpServer = UdpServer,
	["-t"] = TcpClient,
	["-u"] = UdpClient,
}, {
	__index = function(self, key)
		return Server
	end,
})

print("Start...")
features[arg[1] or ""]()
