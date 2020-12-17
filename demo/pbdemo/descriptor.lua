local pbc = require("libprotobuf")

local useNumber = false
local function ParseVarint(msg, config)
	local fieldTbl = {}
	for _, number, wiretype, field in pbc.varints(msg) do
		local fieldConfig = config[number]
		assert(type(fieldConfig) == "table")
		local key = useNumber and number or fieldConfig[1]
		local bIsArray = fieldConfig[2]
		local process = fieldConfig[3]
		local value = field
		local tProcess = type(process)
		if tProcess == "function" then
			value = process(field)
		elseif tProcess == "table" then
			value = ParseVarint(field, process)
		else
			assert(tProcess == "nil")
		end
		local oldField = fieldTbl[key]
		if oldField == nil then
			fieldTbl[key] = bIsArray and { value } or value
		else
			assert(bIsArray)
			assert(type(oldField) == "table")
			table.insert(oldField, value)
		end
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

local Label = {
	"LABEL_OPTIONAL",
	"LABEL_REQUIRED",
	"LABEL_REPEATED",
}
local Type = {
	"TYPE_DOUBLE",
	"TYPE_FLOAT",
	"TYPE_INT64",
	"TYPE_UINT64",
	"TYPE_INT32",
	"TYPE_FIXED64",
	"TYPE_FIXED32",
	"TYPE_BOOL",
	"TYPE_STRING",
	"TYPE_GROUP",
	"TYPE_MESSAGE",
	"TYPE_BYTES",
	"TYPE_UINT32",
	"TYPE_ENUM",
	"TYPE_SFIXED32",
	"TYPE_SFIXED64",
	"TYPE_SINT32",
	"TYPE_SINT64",
}
local FieldDescriptorProtoConfig = {
	{ "name", false, nil },
	{ "extendee", false, nil },
	{ "number", false, nil },
	{ "label", false, function(field) return Label[field] end },
	{ "type", false, function(field) return Type[field] end },
	{ "type_name", false, nil },
	{ "default_value", false, nil },
	{ "options", false, StringToHex },
	{ "oneof_index", false, nil },
	{ "json_name", false, nil },
}
local DescriptorProtoConfig = {
	{ "name", false, nil },
	{ "field", true, FieldDescriptorProtoConfig },
	{ "nested_type", true, "self recursive" },
	{ "enum_type", true, nil },
	{ "extension_range", true, StringToHex },
	{ "extension", true, FieldDescriptorProtoConfig },
	{ "options", false, StringToHex },
	{ "oneof_decl", true, StringToHex },
	{ "reserved_range", true, StringToHex },
	{ "reserved_name", true, nil },
}
DescriptorProtoConfig[3][3] = DescriptorProtoConfig
local FileDescriptorProtoConfig = {
	{ "name", false, nil },
	{ "package", false, nil },
	{ "dependency", true, nil },
	{ "message_type", true, DescriptorProtoConfig },
	{ "enum_type", true, nil },
	{ "service", true, StringToHex },
	{ "extension", true, FieldDescriptorProtoConfig },
	{ "options", false, StringToHex },
	{ "source_code_info", false, StringToHex },
	{ "public_dependency", true, nil },
	{ "weak_dependency", true, nil },
	{ "syntax", false, nil },
}
local FileDescriptorSetConfig = {
	{ "file", true, FileDescriptorProtoConfig },
}

local fd = io.open(arg[1], "rb")
local msg = fd:read("a")
fd:close()
fd = nil
local fieldTbl = ParseVarint(msg, FileDescriptorSetConfig)
print(tostring(fieldTbl, 16))
