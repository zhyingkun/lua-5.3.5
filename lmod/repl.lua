---@type libuv
local libuv

--[[
** {======================================================
** Message Pack and Unpack
** =======================================================
--]]

local packetPBCode = { 0xA, 0x5F, 0xA, 0x12, 0x70, 0x72, 0x6F, 0x74, 0x6F, 0x2F, 0x70, 0x61, 0x63, 0x6B, 0x65, 0x74, 0x2E, 0x70, 0x72, 0x6F, 0x74, 0x6F, 0x12, 0xF, 0x5F, 0x50, 0x61, 0x63, 0x6B, 0x65, 0x74, 0x4D, 0x61, 0x6E, 0x61, 0x67, 0x65, 0x72, 0x5F, 0x22, 0x30, 0xA, 0x6, 0x50, 0x61, 0x63, 0x6B, 0x65, 0x74, 0x12, 0x12, 0xA, 0x4, 0x6E, 0x61, 0x6D, 0x65, 0x18, 0x1, 0x20, 0x1, 0x28, 0x9, 0x52, 0x4, 0x6E, 0x61, 0x6D, 0x65, 0x12, 0x12, 0xA, 0x4, 0x64, 0x61, 0x74, 0x61, 0x18, 0x2, 0x20, 0x1, 0x28, 0xC, 0x52, 0x4, 0x64, 0x61, 0x74, 0x61, 0x62, 0x6, 0x70, 0x72, 0x6F, 0x74, 0x6F, 0x33 }
local packetPB = string.char(function(idx) return packetPBCode[idx] end)

local ProtocolPacket = "_PacketManager_.Packet"

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
	local packet = pbc.encode(ProtocolPacket, {
		name = messageType,
		data = serializedData
	})
	local packetSerial = self.packetManager:packPacket(packet)
	return packetSerial
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
	local status, str = self.packetManager:getPacket()
	if status == libuv.packet_status.OK then
		local packetTable = pbc.decode(ProtocolPacket, str)
		return packetTable.name, pbc.decode(packetTable.name, packetTable.data)
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
	pbc.register(packetPB)
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
	libuv.tcp.Tcp():bind(sockAddr):listenAsync(128, function(status, tcpSocket)
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
---@overload fun(serverIP:string, serverPort:integer):void
---@param serverIP string
---@param serverPort integer
function repl.clientStart(serverIP, serverPort)
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
				local bRunning, prompt, history = true, "> ", ""
				for msgName, printTable in packetHandler:packets() do
					assert(msgName == ProtocolPrint)
					-- libuv.replHistory(printTable.history)
					bRunning = printTable.running
					prompt = printTable.prompt
					history = printTable.history
					io.stdout:write(printTable.output)
				end
				if packetHandler:getRemainForRead() == 0 then
					if not libuv.replNext(bRunning, prompt, history) then
						print("REPL Client End.")
						client:closeAsync()
					end
				end
			end
		end)
		print("REPL Client Start...")
		libuv.replStartOneShotAsync(function(codeStr, bEOF)
			local msg = packetHandler:packReadMessage(codeStr, bEOF)
			tcpClient:writeAsync(msg, function(statusWrite, handle)
				if statusWrite ~= OK then
					printError("REPL TCP Write error:", statusWrite)
				end
			end)
		end, "> ")
	end)
end

---@overload fun():void
---@overload fun(serverIP:string):void
---@overload fun(serverIP:string, serverPort:integer):void
---@param serverIP string
---@param serverPort integer
function repl.clientRun(serverIP, serverPort)
	if not libuv then error(ErrMsg) end
	libuv.setLoop()
	repl.clientStart(serverIP, serverPort)
	libuv.run()
end

return repl
