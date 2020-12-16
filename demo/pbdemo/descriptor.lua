local pbc = require("libprotobuf")

local function ParseVarint(msg, protocb)
	local fieldTbl = {}
	local function SetField(key, field)
		local oldField = fieldTbl[key]
		if oldField == nil then
			fieldTbl[key] = field
		elseif type(oldField) == "table" and getmetatable(oldField) == table then
			oldField:insert(field)
		else
			fieldTbl[key] = setmetatable({ oldField, field }, table)
		end
	end
	for _, number, wiretype, field in pbc.varints(msg) do
		local key, value = protocb(number, wiretype, field)
		SetField(key, value)
	end
	return fieldTbl
end

local function StringToHex(str)
	local result = ""
	for i = 1, #str, 1 do
		local char = str:byte(i)
		result = result .. string.format(" %02X", char)
	end
	return result
end

local function FieldDescriptorProto(number, wiretype, field)
	if wiretype == 0 then
		return number, tostring(field) .. "(" .. tostring(pbc.dezigzag(field)) .. ")"
	elseif wiretype == 1 or wiretype == 5 then
		return number, StringToHex(field)
	else
		return number, field
	end
end

local function DescriptorProto(number, wiretype, field)
	if wiretype == 0 then
		return number, tostring(field) .. "(" .. tostring(pbc.dezigzag(field)) .. ")"
	elseif wiretype == 1 or wiretype == 5 then
		return number, StringToHex(field)
	else
		if number == 2 then
			return number, ParseVarint(field, FieldDescriptorProto)
		else
			return number, field
		end
	end
end

local function FileDescriptorProto(number, wiretype, field)
	if wiretype == 0 then
		return number, tostring(field) .. "(" .. tostring(pbc.dezigzag(field)) .. ")"
	elseif wiretype == 1 or wiretype == 5 then
		return number, StringToHex(field)
	else
		if number == 4 then
			return number, ParseVarint(field, DescriptorProto)
		else
			return number, field
		end
	end
end

local function FileArray(number, wiretype, field)
	assert(number == 1)
	return number, ParseVarint(field, FileDescriptorProto)
end

local fd = io.open("zykTest.pb", "r")
local msg = fd:read("a")
fd:close()
fd = nil
local fieldTbl = ParseVarint(msg, FileArray)
print(tostring(fieldTbl, 16))
