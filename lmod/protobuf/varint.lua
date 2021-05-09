local pbc = require("libprotobuf")

local PBType = {
	Double = pbc.todouble,
	Float = "Float",
	Int64 = "Int64",
	Uint64 = "Uint64",
	Int32 = "Int32",
	Fixed64 = "Fixed64",
	Fixed32 = "Fixed32",
	Bool = "Bool", -- as integer
	String = "String",
	Group = error,
	Message = error,
	Bytes = "Bytes", -- as string
	Uint32 = "Uint32",
	Enum = "Enum", -- as integer
	Sfixed32 = "Sfixed32",
	Sfixed64 = "Sfixed64",
	Sint32 = "Sint32",
	Sint64 = "Sint64",
}

local PBLabel = {
	Optional = "Optional",
	Required = "Required",
	Repeated = "Repeated",
}

local function DefaultProcess(field, process)
	assert(type(process) == "string")
	return field
end

local useNumber = false
local function ParseVarint(msg, config)
	local fieldTbl = {}
	local insert = table.insert
	for _, number, wiretype, field in pbc.varints(msg) do
		local fieldConfig = config[number]
		assert(type(fieldConfig) == "table")
		local key = useNumber and number or fieldConfig[1]
		local bIsArray = fieldConfig[2] == PBLabel.Repeated
		local process = fieldConfig[3]
		local value
		local tProcess = type(process)
		if tProcess == "function" then
			value = process(field)
		elseif tProcess == "table" then
			value = ParseVarint(field, process)
		else
			value = DefaultProcess(field, process)
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
			local escStr = string.escape(field)
			local succeed, result = pcall(ParseVarintRaw, field)
			if succeed then
				if field == escStr then
					result["message as string"] = field
				end
				return result
			end
			return escStr
		end,
		[3] = function(field) error("Start group has been deprecated") end,
		[4] = function(field) error("End group has been deprecated") end,
		[5] = HexDump,
	}
	local fieldTbl = {}
	local insert = table.insert
	for _, number, wiretype, field in pbc.varints(msg) do
		local value = TypeProcess[wiretype](field)
		local origin = fieldTbl[number]
		if origin then
			if type(origin) == "table" then
				insert(origin, value)
			else
				fieldTbl[number] = {
					origin,
					value,
				}
			end
		else
			fieldTbl[number] = value
		end
	end
	return fieldTbl
end

return {
	ParseVarint = ParseVarint,
	ParseVarintRaw = ParseVarintRaw,
	PBType = PBType,
	PBLabel = PBLabel,
}
