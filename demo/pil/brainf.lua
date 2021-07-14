local Instructions = setmetatable({}, {
	__index = function(self, atom)
		return function() end
	end
})
local PreExecutor = {}
local ProgramCount -- number
local MemBufferMT = {
	__index = function(self, idx)
		return 0
	end
}
local MemBuffer -- table
local Ptr -- number

local function RegisteInstruction(atom, func)
	assert(#atom == 1)
	Instructions[atom:byte()] = func
end
local function RegistePreExec(func)
	table.insert(PreExecutor, func)
end
local function ExecuteBrainf(codes)
	ProgramCount = 1
	Ptr = 1
	MemBuffer = setmetatable({}, MemBufferMT)
	for _, func in ipairs(PreExecutor) do
		func(codes)
	end
	local len = #codes
	while ProgramCount > 0 and ProgramCount <= len do
		Instructions[codes:byte(ProgramCount)](ProgramCount)
		ProgramCount = ProgramCount + 1
	end
end
RegisteInstruction(">", function()
	Ptr = Ptr + 1
end)
RegisteInstruction("<", function()
	Ptr = Ptr - 1
end)
RegisteInstruction("+", function()
	MemBuffer[Ptr] = MemBuffer[Ptr] + 1
end)
RegisteInstruction("-", function()
	MemBuffer[Ptr] = MemBuffer[Ptr] - 1
end)
RegisteInstruction(".", function()
	io.write(string.char(MemBuffer[Ptr]))
end)
RegisteInstruction(",", function()
	MemBuffer[Ptr] = io.read(1)
end)
local ScopePair -- table
RegistePreExec(function(codes)
	ScopePair = {}
	local stack = {}
	local left = ("["):byte()
	local right = ("]"):byte()
	local idx = 0
	for byte in string.bytes(codes) do
		idx = idx + 1
		if byte == left then
			table.insert(stack, idx)
		elseif byte == right then
			local leftCount = table.remove(stack)
			ScopePair[leftCount] = idx
			ScopePair[idx] = leftCount
		end
	end
end)
RegisteInstruction("[", function(CurrentCount)
	if MemBuffer[Ptr] == 0 then
		ProgramCount = ScopePair[CurrentCount]
		assert(ProgramCount, "Left CurrentCount: " .. CurrentCount)
	end
end)
RegisteInstruction("]", function(CurrentCount)
	if MemBuffer[Ptr] ~= 0 then
		ProgramCount = ScopePair[CurrentCount]
		assert(ProgramCount, "Right CurrentCount: " .. CurrentCount)
	end
end)


local HelloWorld = [==[
++++++++++[>+++++++>++++++++++>+++>+<<<<-]
>++.>+.+++++++..+++.>++.<<+++++++++++++++.
>.+++.------.--------.>+.>.
]==]
ExecuteBrainf(HelloWorld)
ExecuteBrainf(HelloWorld)
