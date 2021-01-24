local pbc = require("libprotobuf")

local useNumber = false
local function ParseVarint(msg, config)
	local fieldTbl = {}
	local insert = table.insert
	for _, number, wiretype, field in pbc.varints(msg) do
		local fieldConfig = config[number]
		assert(type(fieldConfig) == "table")
		local key = useNumber and number or fieldConfig[1]
		local bIsArray = fieldConfig[2]
		local process = fieldConfig[3]
		local value
		local tProcess = type(process)
		if tProcess == "function" then
			value = process(field)
		elseif tProcess == "table" then
			value = ParseVarint(field, process)
		else
			assert(tProcess == "nil")
			value = field
		end
		local oldField = fieldTbl[key]
		if oldField == nil then
			fieldTbl[key] = bIsArray and { value } or value
		else
			assert(bIsArray and type(oldField) == "table")
			insert(oldField, value)
		end
	end
	return fieldTbl
end

local function ParseVarintRaw(msg)
	local function HexDump(field)
		local buf = {}
		for byte in string.bytes(field) do
			table.insert(buf, string.format("%02x", byte))
		end
		return "0x" .. table.concat(buf)
	end
	local TypeProcess = {
		[0] = function(field) return field end,
		[1] = HexDump,
		[2] = function(field)
			local succeed, result = pcall(ParseVarintRaw, field)
			if succeed then
				return result
			end
			return string.escape(field)
		end,
		[3] = function(field) error("Start group has been deprecated") end,
		[4] = function(field) error("End group has been deprecated") end,
		[5] = HexDump,
	}
	local fieldTbl = {}
	for _, number, wiretype, field in pbc.varints(msg) do
		fieldTbl[number] = TypeProcess[wiretype](field)
	end
	return fieldTbl
end

return {
	ParseVarint = ParseVarint,
	ParseVarintRaw = ParseVarintRaw,
}
