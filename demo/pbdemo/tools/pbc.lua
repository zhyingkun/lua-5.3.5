local pbc = require("libprotobuf")
local ParseVarint = require("varint")

local Label = {
	[0] = "optional",
	"required",
	"repeated"
}

local PBCFieldConfig = {
	{ "name", false, nil },
	{ "id", false, nil },
	{ "label", false, function(field) return Label[field] end },
	{ "type", false, nil },
	{ "type_name", false, nil },
	{ "default_int", false, nil },
	{ "default_string", false, nil },
	{ "default_real", false, pbc.todouble },
}

local PBCFileConfig = {
	{ "name", false, nil },
	{ "dependency", true, nil },
	{ "message_name", true, nil },
	{ "message_size", true, nil },
	{ "message_field", true, PBCFieldConfig },
	{ "enum_name", true, nil },
	{ "enum_size", true, nil },
	{ "enum_string", true, nil },
	{ "enum_id", true, nil },
}

local fd = io.open(arg[1], "rb")
local msg = fd:read("a")
fd:close()
fd = nil
local fieldTbl = ParseVarint(msg, PBCFileConfig)

local TypeName = {
	"double",
	"float",
	"int64",
	"uint64",
	"int32",
	"fixed64",
	"fixed32",
	"bool",
	"string",
	nil,
	nil,
	"bytes",
	"uint32",
	nil,
	"sfixed32",
	"sfixed64",
	"sint32",
	"sint64",
}

local outputTbl = {}
local insert = table.insert
local function OutputLine(str)
	insert(outputTbl, str)
end

local idx = 0
for index, name in ipairs(fieldTbl.enum_name) do
	OutputLine("enum " .. name:sub(1, -2) .. " {")
	for i = 1, fieldTbl.enum_size[index], 1 do
		idx = idx + 1
		local str = fieldTbl.enum_string[idx]:sub(1, -2)
		local id = fieldTbl.enum_id[idx]
		OutputLine("\t" .. str .. " = " .. tostring(id) .. ";")
	end
	OutputLine("}")
end

local idx = 0
for index, name in ipairs(fieldTbl.message_name) do
	OutputLine("message " .. name:sub(1, -2) .. " {")
	for i = 1, fieldTbl.message_size[index], 1 do
		idx = idx + 1
		local field = fieldTbl.message_field[idx]
		local typeName = TypeName[field.type]
		if typeName == nil then
			typeName = field.type_name:match(".*%.(.*).$")
		end
		local statement = "\t" .. field.label .. " " .. typeName .. " " .. field.name:sub(1, -2) .. " = " .. tostring(field.id) .. ";"
		OutputLine(statement)
	end
	OutputLine("}")
end

print(table.concat(outputTbl, "\n"))
