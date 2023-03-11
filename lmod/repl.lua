---@type libuv
local libuv

--[[
** {======================================================
** Message Pack and Unpack
** =======================================================
--]]

local replPBCode = { 0xA, 0xBD, 0x1, 0xA, 0x10, 0x70, 0x72, 0x6F, 0x74, 0x6F, 0x2F, 0x72, 0x65, 0x70, 0x6C, 0x2E, 0x70, 0x72, 0x6F, 0x74, 0x6F, 0x12, 0x6, 0x5F, 0x52, 0x45, 0x50, 0x4C, 0x5F, 0x22, 0x2C, 0xA, 0x4, 0x52, 0x65, 0x61, 0x64, 0x12, 0x12, 0xA, 0x4, 0x63, 0x6F, 0x64, 0x65, 0x18, 0x1, 0x20, 0x1, 0x28, 0x9, 0x52, 0x4, 0x63, 0x6F, 0x64, 0x65, 0x12, 0x10, 0xA, 0x3, 0x65, 0x6F, 0x66, 0x18, 0x2, 0x20, 0x1, 0x28, 0x8, 0x52, 0x3, 0x65, 0x6F, 0x66, 0x22, 0x6B, 0xA, 0x5, 0x50, 0x72, 0x69, 0x6E, 0x74, 0x12, 0x16, 0xA, 0x6, 0x70, 0x72, 0x6F, 0x6D, 0x70, 0x74, 0x18, 0x1, 0x20, 0x1, 0x28, 0x9, 0x52, 0x6, 0x70, 0x72, 0x6F, 0x6D, 0x70, 0x74, 0x12, 0x16, 0xA, 0x6, 0x6F, 0x75, 0x74, 0x70, 0x75, 0x74, 0x18, 0x2, 0x20, 0x1, 0x28, 0x9, 0x52, 0x6, 0x6F, 0x75, 0x74, 0x70, 0x75, 0x74, 0x12, 0x18, 0xA, 0x7, 0x68, 0x69, 0x73, 0x74, 0x6F, 0x72, 0x79, 0x18, 0x3, 0x20, 0x1, 0x28, 0x9, 0x52, 0x7, 0x68, 0x69, 0x73, 0x74, 0x6F, 0x72, 0x79, 0x12, 0x18, 0xA, 0x7, 0x72, 0x75, 0x6E, 0x6E, 0x69, 0x6E, 0x67, 0x18, 0x4, 0x20, 0x1, 0x28, 0x8, 0x52, 0x7, 0x72, 0x75, 0x6E, 0x6E, 0x69, 0x6E, 0x67, 0x62, 0x6, 0x70, 0x72, 0x6F, 0x74, 0x6F, 0x33 }
local replPB = string.char(function(idx) return replPBCode[idx] end)

local ProtocolRead = "_REPL_.Read"
local ProtocolPrint = "_REPL_.Print"

---@type pbc
local pbc

---@class REPLPacket
local REPLPacket = {
	---@type PacketManager
	packetManager = nil
}
REPLPacket.__index = REPLPacket

function REPLPacket:packMessage(messageType, dataTable)
	local serializedData = pbc.encode(messageType, dataTable)
	return self.packetManager:packPacket(messageType, packet)
end

function REPLPacket:packReadMessage(codeStr, eof)
	return self:packMessage(ProtocolRead, {
		code = codeStr,
		eof = eof,
	})
end

function REPLPacket:packPrintMessage(running, prompt, history, output)
	return self:packMessage(ProtocolPrint, {
		running = running,
		prompt = prompt,
		history = history,
		output = output,
	})
end

function REPLPacket:addPackData(data)
	self.packetManager:addPackData(data)
end

function REPLPacket:getRemainForRead()
	return self.packetManager:getRemainForRead()
end

function REPLPacket:getPacket()
	local status, messageType, data = self.packetManager:getPacket()
	if status == libuv.packet_status.OK then
		return messageType, pbc.decode(messageType, data)
	elseif status == libuv.packet_status.NeedMore then
		return
	else -- libuv.packet_status.ErrorLength
		error("Parse Packet Error")
	end
end
function REPLPacket:packets()
	return self.getPacket, self, nil
end

---@return REPLPacket
local function REPLPacketHandler()
	local handler = {
		packetManager = libuv.PacketManager()
	}
	return setmetatable(handler, REPLPacket)
end

-- }======================================================

---@class repl:table
local repl = {}

local ErrMsg = "repl must init with libuv"

---@overload fun(uv:libuv):void
---@overload fun(uv:libuv, pb:pbc):void
---@param uv libuv
---@param pb pbc
function repl.init(uv, pb)
	if type(uv) ~= "table" then error(ErrMsg) end
	libuv = uv
	pbc = pb or require("protobuf")
	pbc.register(replPB)
end

---@overload fun():void
---@overload fun(callback:REPLEvalSignature, firstPrompt:string):void
---@param callback REPLEvalSignature
---@param firstPrompt string
function repl.startAsync(callback, firstPrompt)
	if not libuv then error(ErrMsg) end
	libuv.replStartAsync(callback, firstPrompt)
end

---@type REPLEvalSignature
function repl.defaultEval(codeStr, eof)
	if not libuv then error(ErrMsg) end
	return libuv.replDefaultEval(codeStr, eof)
end

--[[
** {======================================================
** Eval default implementation in Lua
** =======================================================
--]]

local firstLine = true
local codeCache
local function compileSourceCode(codeStr)
	local code = codeStr
	if firstLine then
		if codeStr:byte() == ("="):byte() then
			code = "return " .. codeStr:sub(2)
		else
			local loader = load("return " .. codeStr)
			if loader then
				return loader, codeStr
			end
		end
	else
		code = codeCache .. "\n" .. codeStr
	end
	local loader, msg = load(code)
	return loader or msg, code
end
local function inComplete(loader)
	return type(loader) == "string" and loader:sub(-5, -1) == "<eof>"
end
---@type REPLEvalSignature
function repl.defaultEvalLua(codeStr, eof)
	local running = true
	local history
	if eof then
		if firstLine then
			running = false
			print()
		else
			firstLine = true
			history = codeCache
			print()
		end
	else
		local loader, code = compileSourceCode(codeStr)
		if inComplete(loader) then
			firstLine = false
			codeCache = code
		else
			firstLine = true
			history = code
			local msg
			if type(loader) == "function" then
				local ret, em = pcall(function()
					print(loader())
				end)
				if not ret then
					msg = em
				end
			else
				msg = loader
			end
			if msg then
				print("lua: " .. msg)
			end
		end
	end
	local prompt = firstLine and "> " or ">> "
	return running, prompt, history
end

-- }======================================================

local function printError(msg, status)
	printerr(msg, status, libuv.errName(status), libuv.strError(status))
end

---@alias REPLRemoteEvalSignature fun(codeStr:string | nil, eof:boolean):boolean, string, string | nil, string | nil @running, prompt, history, output

---@overload fun():void
---@overload fun(ip:string):void
---@overload fun(ip:string, port:integer):void
---@overload fun(ip:string, port:integer, callback:REPLEvalSignature):void
---@param ip string
---@param port integer
---@param callback REPLEvalSignature
function repl.serverStartAsync(ip, port, callback)
	if not libuv then error(ErrMsg) end
	callback = callback or repl.getServerDefaultEval()
	local OK = libuv.err_code.OK
	local EOF = libuv.err_code.EOF

	local sockAddr = libuv.network.SockAddr():ip4Addr(ip or "0.0.0.0", port or 1999)
	libuv.tcp.Tcp():bind(sockAddr):listenStartAsync(128, function(status, tcpSocket)
		if status ~= OK then
			printError("TCP listen error:", status)
			return
		end
		local statusAccept, tcpConnection = tcpSocket:accept()
		if statusAccept ~= OK then
			printError("TCP accept error:", statusAccept)
			return
		end
		local fd = tcpConnection:fileno()
		print("New REPL connection start:", fd)
		local packetHandler = REPLPacketHandler()
		tcpConnection:readStartAsync(function(nread, str, connection)
			if nread < 0 then
				if nread == EOF then
					print("REPL connection end:", fd)
				else
					printError("REPL connection read error:", nread)
				end
				connection:closeAsync()
				return
			end
			packetHandler:addPackData(str)
			for msgName, readTable in packetHandler:packets() do
				assert(msgName == ProtocolRead)
				local running, prompt, history, output = callback(readTable.code, readTable.eof)
				local msg = packetHandler:packPrintMessage(running, prompt, history, output)
				connection:writeAsync(msg, function(statusWrite)
					if statusWrite ~= OK then
						printError("REPL Server TCP Write error:", statusWrite)
					end
				end)
			end
		end)
	end)
end

---@param evalFunc REPLEvalSignature
---@return REPLRemoteEvalSignature
function repl.makeServerEval(evalFunc)
	return function(codeStr, eof)
		util.printBuffer()
		local running, prompt, history = evalFunc(codeStr, eof)
		local output = util.printFinish()
		return running, prompt, history, output
	end
end

local ServerDefaultEval

---@return REPLRemoteEvalSignature
function repl.getServerDefaultEval()
	if not libuv then error(ErrMsg) end
	if not ServerDefaultEval then
		ServerDefaultEval = repl.makeServerEval(libuv.replDefaultEval)
	end
	return ServerDefaultEval
end

---@overload fun():void
---@overload fun(serverIP:string):void
---@overload fun(serverIP:string, serverPort:integer | string):void
---@param serverIP string @ Server IP or Domain Name
---@param serverPort integer | string
function repl.clientStart(serverIP, serverPort)
	if not libuv then error(ErrMsg) end
	local OK = libuv.err_code.OK

	serverIP = serverIP or "0.0.0.0"
	serverPort = serverPort or "1999"
	--[[
	-- local sockAddr = libuv.network.SockAddr():ip4Addr(serverIP, serverPort)
	-- local addrInfo, errCode = libuv.network.getAddrInfo(serverIP, serverPort)
	libuv.network.getAddrInfoAsync(serverIP, serverPort, nil, function(addrInfo, status)
		if not addrInfo then
			local msg = string.format("ServerIP or DomainName '%s:%d' error:", serverIP, serverPort)
			printError(msg, status)
			return
		end
		local sockAddr = addrInfo.addr
		local status = libuv.tcp.Tcp():connectAsync(sockAddr, function(status, tcpClient)
			if status < 0 then
				local msg = string.format("REPL TCP Connect to %s:%d failed:", serverIP, serverPort)
				printError(msg, status)
				return
			end
			local packetHandler = REPLPacketHandler()
			local function sendStrToServer(codeStr, bEOF)
				local msg = packetHandler:packReadMessage(codeStr, bEOF)
				tcpClient:writeAsync(msg, function(statusWrite, handle)
					if statusWrite ~= OK then
						printError("REPL TCP Write error:", statusWrite)
					end
				end)
			end
			tcpClient:readStartAsync(function(nread, str, client)
				if nread < 0 then
					printError("REPL TCP Read error:", nread)
				else
					packetHandler:addPackData(str)
					local tbl
					for msgName, printTable in packetHandler:packets() do
						assert(msgName == ProtocolPrint)
						-- libuv.replHistory(printTable.history)
						io.stdout:write(printTable.output)
						if tbl then 
							print("Receive multi packet more than one")
						end
						tbl = printTable
					end
					if packetHandler:getRemainForRead() == 0 then
						if tbl then
							if tbl.running then
								libuv.replNextAsync(tbl.prompt, tbl.history, sendStrToServer)
							else
								libuv.replShutdown()
								print("REPL Client End.")
								client:closeAsync()
							end
						end
					else
						if tbl then
							print("Receive complete packet and incomplete packet")
							tbl = nil
						else
							print("Receive incomplete packet")
						end
					end
				end
			end)
			print("REPL Client Start...")
			libuv.replInitOneShot()
			libuv.replNextAsync(nil, nil, sendStrToServer)
		end)
		if status ~= OK then
			local msg = string.format("REPL TCP Connect to '%s:%d' error:", serverIP, serverPort)
			printError(msg, status)
		end
	end)
	--]]

	---[[
	local function getSockAddr(serverIP, serverPort)
		local addrInfo, errCode = libuv.network.getAddrInfoAsyncWait(serverIP, serverPort);
		if addrInfo then
			return addrInfo.addr
		end
		local msg = string.format("ServerIP or DomainName '%s:%d' error:", serverIP, serverPort)
		printError(msg, status)
	end
	local function makeTcpConnection(sockAddr)
		local tcpClient = libuv.tcp.Tcp()
		local status = tcpClient:connectAsyncWait(sockAddr)
		if status == OK then
			return tcpClient
		end
		local msg = string.format("REPL TCP Connect to %s:%d failed:", serverIP, serverPort)
		printError(msg, status)
	end
	local function runInCoroutine(func)
		coroutine.wrap(func)()
	end	
	runInCoroutine(function()
		local sockAddr = getSockAddr(serverIP, serverPort)
		if not sockAddr then return end
		local tcpClient = makeTcpConnection(sockAddr)
		if not tcpClient then return end

		local function readAsyncWait()
			while true do
				local nread, str = tcpClient:readCacheWait()
				if nread < 0 then
					printError("REPL TCP Read error:", nread)
				elseif nread > 0 then
					return str
				end
			end
		end
		local packetHandler = REPLPacketHandler()
		tcpClient:readStartCache()
		local function remoteREPL(codeStr, bEOF)
			local msg = packetHandler:packReadMessage(codeStr, bEOF)
			if tcpClient:writeAsyncWait(msg) ~= OK then
				printError("REPL TCP Write error:", statusWrite)
			end
			local tbl
			repeat
				local str = readAsyncWait()
				packetHandler:addPackData(str)
				for msgName, printTable in packetHandler:packets() do
					assert(msgName == ProtocolPrint)
					io.stdout:write(printTable.output)
					if tbl then 
						print("Receive multi packet more than one")
					end
					tbl = printTable
				end
				if packetHandler:getRemainForRead() ~= 0 then
					if tbl then
						print("Receive complete packet and incomplete packet")
						tbl = nil
					else
						print("Receive incomplete packet")
					end
				end
			until tbl
			return tbl.running, tbl.prompt, tbl.history
		end

		print("REPL Client Start...")
		libuv.replInitOneShot()
		local bRunning = true
		local prompt, history, output
		while bRunning do
			local codeStr, bEOF = libuv.replNextAsyncWait(prompt, history)
			bRunning, prompt, history = remoteREPL(codeStr, bEOF)
		end
		libuv.replShutdown()
		print("REPL Client End.")

		tcpClient:closeAsync()
	end)
	--]]
end

---@overload fun():void
---@overload fun(serverIP:string):void
---@overload fun(serverIP:string, serverPort:string):void
---@param serverIP string @ Server IP or Domain Name
---@param serverPort string
function repl.cmdClientRun(serverIP, serverPort)
	repl.init(require("libuv"), require("protobuf"))
	libuv.setLoop()
	repl.clientStart(serverIP, serverPort and math.tointeger(serverPort))
	libuv.run()
end

function repl.clientOneShotAsync(serverIP, serverPort, codeStr, callback)
	if not libuv then error(ErrMsg) end
	local OK = libuv.err_code.OK

	local sockAddr = libuv.network.SockAddr():ip4Addr(serverIP or "0.0.0.0", serverPort or 1999)
	libuv.tcp.Tcp():connectAsync(sockAddr, function(status, tcpClient)
		if status < 0 then
			printError("REPL TCP Connect error:", status)
			return
		end
		local packetHandler = REPLPacketHandler()
		tcpClient:readStartAsync(function(nread, str, client)
			if nread < 0 then
				printError("REPL TCP Read error:", nread)
			else
				packetHandler:addPackData(str)
				for msgName, printTable in packetHandler:packets() do
					assert(msgName == ProtocolPrint)
					callback(printTable.output)
				end
				if packetHandler:getRemainForRead() == 0 then
					client:closeAsync()
				end
			end
		end)
		local msg = packetHandler:packReadMessage(codeStr, false)
		tcpClient:writeAsync(msg, function(statusWrite, handle)
			if statusWrite ~= OK then
				printError("REPL TCP Write error:", statusWrite)
			end
		end)
	end)
end

local ASYNC_WAIT_MSG = "AsyncWait api must running in coroutine"
local running = coroutine.running
local yield = coroutine.yield
local resumeOrigin = coroutine.resume
local function resume(co, ...)
	local status, msg = resumeOrigin(co, ...)
	if not status then
		printerr("repl lua module resume coroutine error: ", msg, debug.traceback(co))
	end
end
function repl.clientOneShotAsyncWait(serverIP, serverPort, codeStr)
	local co, main = running()
	if main then error(ASYNC_WAIT_MSG) end
	repl.clientOneShotAsync(serverIP, serverPort, codeStr, function(output)
		resume(co, output)
	end)
	return yield()
end

return repl
