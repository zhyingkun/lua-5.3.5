local libuv = require("libuv")
local tcp = libuv.tcp
local udp = libuv.udp
local network = libuv.network
local timer = libuv.timer

local errName = libuv.errName
local strError = libuv.strError
local err_code = libuv.err_code
local OK = err_code.OK
local EOF = err_code.EOF

util.setErrorMessageHandler(function(msg)
	return "In custom msg handler:" .. msg .. "\n traceback:" .. debug.traceback()
end)

libuv.setLoop()
libuv.runDeferred()

local function TcpServer()
	local tcpSocket = tcp.Tcp()
	local sockAddr = network.SockAddr()
	sockAddr:ip4Addr("0.0.0.0", 1024)
	tcpSocket:bind(sockAddr)
	tcpSocket:listenStartAsync(128, function(status)
		if status ~= OK then
			print("TCP listen error:", status, errName(status), strError(status))
			return
		end
		local tcpConnection = tcp.Tcp()
		if tcpSocket:accept(tcpConnection) == OK then
			print("TCP receive a connection: fd is", tcpConnection:fileno())
			tcpConnection:readStartAsync(function(nread, str)
				if nread < 0 then
					if nread == EOF then
						print("TCP Connection Read EOF")
					else
						print("TCP read error:", nread, errName(nread), strError(nread))
					end
					tcpConnection:closeAsync(function()
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
	local udpSocket = udp.Udp()
	local sockAddr = network.SockAddr()
	sockAddr:ip4Addr("0.0.0.0", 1025)
	udpSocket:bind(sockAddr)
	udpSocket:recvStartAsync(function(nread, str, addr, flags)
		if nread < 0 then
			print("UDP recv error:", nread, errName(nread), strError(nread))
			print(udpSocket:getSockName())
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
	local tcpClient = tcp.Tcp()
	local sockAddr = network.SockAddr()
	sockAddr:ip4Addr("0.0.0.0", 1024)
	--[[
	tcpClient:connectAsync(sockAddr, function(status)
		if status < 0 then
			print("TCP Connect error:", status, errName(status), strError(status))
			return
		end
		print("Connect succeed: fd is", tcpClient:fileno())
		local function WriteHello(idx)
			local hello = "Good Good Study Day Day Up!"
			tcpClient:writeAsync(hello, function(status)
				if status == OK then
					print("Write Complete:", #hello, idx)
				else
					print(idx, errName(status), strError(status))
				end
			end)
		end
		for i = 1, 3, 1 do
			WriteHello(i)
		end
		local delay = timer.Timer()
		delay:startOneShotAsync(1000, function()
			delay:stop()
			local str = "="
			local len = 128
			local buffer = str:rep(len * 1024)
			tcpClient:writeAsync(buffer, function(status)
				if status == OK then
					print("Write Complete:", len .. "K")
				else
					print(errName(status), strError(status))
				end
			end)
		end)
	end)
	--]]
	cocall(function()
		local status = tcpClient:connectAsyncWait(sockAddr)
		if status < 0 then
			print("TCP Connect error:", status, errName(status), strError(status))
			return
		end
		print("Connect succeed: fd is", tcpClient:fileno())
		local function WriteHello(idx)
			local hello = "Good Good Study Day Day Up!"
			--[[
			tcpClient:writeAsync(hello, function(status)
				if status == OK then
					print("Write Complete:", #hello, idx)
				else
					print(idx, errName(status), strError(status))
				end
			end)
			--]]
			cocall(function()
				local status = tcpClient:writeAsyncWait(hello)
				if status == OK then
					print("Write Complete:", #hello, idx)
				else
					print(idx, errName(status), strError(status))
				end
			end)
		end
		for i = 1, 3, 1 do
			WriteHello(i)
		end
		local delay = timer.Timer()
		--[[
		delay:startOneShotAsync(1000, function()
			delay:stop()
			local str = "="
			local len = 128
			local buffer = str:rep(len * 1024)
			tcpClient:writeAsync(buffer, function(status)
				if status == OK then
					print("Write Complete:", len .. "K")
				else
					print(errName(status), strError(status))
				end
			end)
		end)
		--]]
		delay:startOneShotAsyncWait(1000)
		delay:stop()
		local str = "="
		local len = 128
		local buffer = str:rep(len * 1024)
		local status = tcpClient:writeAsyncWait(buffer)
		if status == OK then
			print("Write Complete:", len .. "K")
		else
			print(errName(status), strError(status))
		end
	end)
end

local function UdpClient()
	local udpClient = udp.Udp()
	local sockAddr = network.SockAddr()
	sockAddr:ip4Addr("127.0.0.1", 1025)

	local function WriteHello(idx)
		local hello = "Hello World!"
		--[[
		udpClient:sendAsync(hello, sockAddr, function(status)
			if status == OK then
				print("UDP Send Complete:", #hello, udpClient:getSockName(), idx)
			else
				print(idx, errName(status), strError(status))
			end
		end)
		--]]
		cocall(function()
			local status = udpClient:sendAsyncWait(hello, sockAddr)
			if status == OK then
				print("UDP Send Complete:", #hello, udpClient:getSockName(), idx)
			else
				print(idx, errName(status), strError(status))
			end
		end)
	end
	for i = 1, 3, 1 do
		WriteHello(i)
	end

	local str = "="

	udpClient:sendBufferSize(65536)
	local len = 65507
	-- local len = 65508

	-- local len = 9216
	-- local len = 9217

	local buffer = str:rep(len)
	--[[
	udpClient:sendAsync(buffer, sockAddr, function(status)
		if status == OK then
			print("UDP Send Complete:", len, udpClient:getSockName())
		else
			print(errName(status), strError(status))
		end
	end)
	--]]
	cocall(function()
		local status = udpClient:sendAsyncWait(buffer, sockAddr)
		if status == OK then
			print("UDP Send Complete:", len, udpClient:getSockName())
		else
			print(errName(status), strError(status))
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
