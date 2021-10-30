local file = arg[1]
if not file then
	print("pass varint binary file path in first parament")
	return
end
local fd = io.open(file, "rb")
local msg = fd:read("a")
fd:close()
fd = nil

local fieldTbl = require("protobuf.varint").ParseVarintRaw(msg)
print(tostring(fieldTbl, 16))
