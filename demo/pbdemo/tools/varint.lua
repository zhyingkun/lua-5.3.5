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

return ParseVarint
