local pbc = require("libprotobuf")
local ParseVarint = require("varint")

--- Configs for google descriptor.proto Begin ------

local Optional = false
local Required = false
local Repeated = true

local Double = nil
local Float = nil
local Int64 = nil
local Uint64 = nil
local Int32 = nil
local Fixed64 = nil
local Fixed32 = nil
local Bool = nil -- as integer
local String = nil
local Group = error
local Message = error
local Bytes = nil -- as string
local Uint32 = nil
local Enum = nil -- as integer
local Sfixed32 = nil
local Sfixed64 = nil
local Sint32 = nil
local Sint64 = nil

--[[
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
]]
local UninterpretedOptionConfig = {
	nil,
	{"name", Repeated, {
		{"name_part", Required, String},
		{"is_extension", Required, Bool},
	} },
	{"identifier_value", Optional, String},
	{"positive_int_value", Optional, Uint64},
	{"negative_int_value", Optional, Int64},
	{"double_value", Optional, Double},
	{"string_value", Optional, Bytes},
	{"aggregate_value", Optional, String},
}
local EnumValueOptionsConfig = {
	{"deprecated", Optional, Bool},
	{"uninterpreted_option", Repeated, UninterpretedOptionConfig},
}
local EnumValueDescriptorProtoConfig = {
	{"name", Optional, String},
	{"number", Optional, Int32},
	{"options", Optional, EnumValueOptionsConfig},
}
local EnumOptionsConfig = {
	nil,
	{"allow_alias", Optional, Bool},
	{"deprecated", Optional, Bool},
	[999] = {"uninterpreted_option", Repeated, UninterpretedOptionConfig},
}
local EnumDescriptorProtoConfig = {
	{"name", Optional, String},
	{"value", Repeated, EnumValueDescriptorProtoConfig},
	{"options", Optional, EnumOptionsConfig},
	{"reserved_range", Repeated, {
		{"start", Optional, Int32},
		{"end", Optional, Int32},
	} },
	{"reserved_name", Repeated, String},
}
--[[
local CType = {
	[0] = "STRING",
	"CORD",
	"STRING_PIECE",
}
local JSType = {
	[0] = "JS_NORMAL",
	"JS_STRING",
	"JS_NUMBER",

}
]]
local FieldOptionsConfig = {
	{ "ctype", Optional, Enum }, -- function(field) return CType[field] end
	{ "packed", Optional, Bool }, -- function(field) return JSType[field] end
	{ "deprecated", Optional, Bool },
	nil,
	{ "lazy", Optional, Bool },
	{ "jstype", Optional, Enum },
	[10] = { "weak", Optional, Bool },
	[999] = { "uninterpreted_option", Repeated, UninterpretedOptionConfig },
}
local FieldDescriptorProtoConfig = {
	{ "name", Optional, String },
	{ "extendee", Optional, String },
	{ "number", Optional, Int32 },
	{ "label", Optional, Enum }, -- function(field) return Label[field] end
	{ "type", Optional, Enum }, -- function(field) return Type[field] end
	{ "type_name", Optional, String },
	{ "default_value", Optional, String },
	{ "options", Optional, FieldOptionsConfig },
	{ "oneof_index", Optional, Int32 },
	{ "json_name", Optional, String },
	[17] = { "proto3_optional", Optional, Bool },
}
local ExtensionRangeOptionsConfig = {
	[999] = { "uninterpreted_option", Repeated, UninterpretedOptionConfig },
}
local MessageOptionsConfig = {
	{ "message_set_wire_format", Optional, Bool },
	{ "no_standard_descriptor_accessor", Optional, Bool },
	{ "deprecated", Optional, Bool },
	[7] = { "map_entry", Optional, Bool },
	[8] = nil,
	[9] = nil,
	[999] = { "uninterpreted_option", Repeated, UninterpretedOptionConfig },
}
local OneofOptionsConfig = {
	[999] = { "uninterpreted_option", Repeated, UninterpretedOptionConfig },
}
local OneofDescriptorProtoConfig = {
	{ "name", Optional, String },
	{ "options", Optional, OneofOptionsConfig },
}
local DescriptorProtoConfig = {
	{ "name", Optional, String },
	{ "field", Repeated, FieldDescriptorProtoConfig },
	{ "nested_type", Repeated, "self recursive" },
	{ "enum_type", Repeated, EnumDescriptorProtoConfig },
	{ "extension_range", Repeated, {
		{ "start", Optional, Int32 },
		{ "end", Optional, Int32 },
		{ "options", Optional, ExtensionRangeOptionsConfig },
	} },
	{ "extension", Repeated, FieldDescriptorProtoConfig },
	{ "options", Optional, MessageOptionsConfig },
	{ "oneof_decl", Repeated, OneofDescriptorProtoConfig },
	{ "reserved_range", Repeated, {
		{ "start", Optional, Int32 },
		{ "end", Optional, Int32 },
	} },
	{ "reserved_name", Repeated, String },
}
DescriptorProtoConfig[3][3] = DescriptorProtoConfig
--[[
local IdempotencyLevel = {
	[0] = "IDEMPOTENCY_UNKNOWN",
	"NO_SIDE_EFFECTS",
	"IDEMPOTENT",
}
]]
local MethodOptionsConfig = {
	[33] = { "deprecated", Optional, Bool },
	[34] = { "idempotency_level", Optional, Enum },
	[999] = { "uninterpreted_option", Repeated, UninterpretedOptionConfig },
}
local MethodDescriptorProtoConfig = {
	{ "name", Optional, String },
	{ "input_type", Optional, String },
	{ "output_type", Optional, String },
	{ "options", Optional, MethodOptionsConfig },
	{ "client_streaming", Optional, Bool },
	{ "server_streaming", Optional, Bool },
}
local ServiceOptionsConfig = {
	{ "deprecated", Optional, Bool },
	{ "uninterpreted_option", Repeated, UninterpretedOptionConfig },
}
local ServiceDescriptorProtoConfig = {
	{ "name", Optional, String },
	{ "method", Repeated, MethodDescriptorProtoConfig },
	{ "options", Optional, ServiceOptionsConfig },
}
--[[
local OptimizeMode = {
	"SPEED",
	"CODE_SIZE",
	"LITE_RUNTIME",
}
]]
local FileOptionsConfig = {
	{ "java_package", Optional, String },
	[8] = { "java_outer_classname", Optional, String },
	[10] = { "java_multiple_files", Optional, Bool },
	[20] = { "java_generate_equals_and_hash", Optional, Bool },
	[27] = { "java_string_check_utf8", Optional, Bool },
	[9] = { "optimize_for", Optional, Enum }, -- function(field) return OptimizeMode[field] end
	[11] = { "go_package", Optional, String },
	[16] = { "cc_generic_services", Optional, Bool },
	[17] = { "java_generic_services", Optional, Bool },
	[18] = { "py_generic_services", Optional, Bool },
	[42] = { "php_generic_services", Optional, Bool },
	[23] = { "deprecated", Optional, Bool },
	[31] = { "cc_enable_arenas", Optional, Bool },
	[36] = { "objc_class_prefix", Optional, String },
	[37] = { "csharp_namespace", Optional, String },
	[39] = { "swift_prefix", Optional, String },
	[40] = { "php_class_prefix", Optional, String },
	[41] = { "php_namespace", Optional, String },
	[44] = { "php_metadata_namespace", Optional, String },
	[45] = { "ruby_package", Optional, String },
	[999] = { "uninterpreted_option", Repeated, UninterpretedOptionConfig },
	[38] = nil,
}
local SourceCodeInfoConfig = {
	{ "location", Repeated, {
		{ "path", Repeated, Int32 },
		{ "span", Repeated, Int32 },
		{ "leading_comments", Optional, String },
		{ "trailing_comments", Optional, String },
		nil,
		{ "leading_detached_comments", Repeated, String },
	} },
}
local FileDescriptorProtoConfig = {
	{ "name", Optional, String },
	{ "package", Optional, String },
	{ "dependency", Repeated, String },
	{ "message_type", Repeated, DescriptorProtoConfig },
	{ "enum_type", Repeated, EnumDescriptorProtoConfig },
	{ "service", Repeated, ServiceDescriptorProtoConfig },
	{ "extension", Repeated, FieldDescriptorProtoConfig },
	{ "options", Optional, FileOptionsConfig },
	{ "source_code_info", Optional, SourceCodeInfoConfig },
	{ "public_dependency", Repeated, Int32 },
	{ "weak_dependency", Repeated, Int32 },
	{ "syntax", Optional, String },
}
local FileDescriptorSetConfig = {
	{ "file", Repeated, FileDescriptorProtoConfig },
}

--- Configs for google descriptor.proto End ------

local outputTbl
local prefixIndent
local insert = table.insert
local function OutputLine(str)
	insert(outputTbl, str)
end

local Label = {
	"optional",
	"required",
	"repeated"
}
local Type = {
	"double",
	"float",
	"int64",
	"uint64",
	"int32",
	"fixed64",
	"fixed32",
	"bool",
	"string",
	nil, -- TYPE_GROUP
	nil, -- TYPE_MESSAGE
	"bytes",
	"uint32",
	nil, -- TYPE_ENUM
	"sfixed32",
	"sfixed64",
	"sint32",
	"sint64",
}

local function OutputEnum(enum, prefix)
	OutputLine(prefix .. "enum " .. enum.name .. " {")
	local subPrefix = prefix .. prefixIndent
	for _, v in ipairs(enum.value) do
		OutputLine(subPrefix .. v.name .. " = " .. v.number .. ";")
	end
	OutputLine(prefix .. "}")
end

local function NoDot(str) return str:byte(1) == ("."):byte(1) and str:sub(2, -1) or str end

local OnlyRepeated = true
local function OutputField(field, prefix)
	local typeName = Type[field.type] or NoDot(field.type_name)
	assert(type(typeName) == "string")
	local label = OnlyRepeated and field.label ~= 3 and "" or Label[field.label] .. " "
	OutputLine(prefix .. label .. typeName .. " " .. field.name .. " = " .. tostring(field.number) .. ";")
end

local function OutputMessage(msg, prefix)
	OutputLine(prefix .. "message " .. msg.name .. " {")
	local subPrefix = prefix .. prefixIndent
	for _, enum in ipairs(msg.enum_type or {}) do
		OutputEnum(enum, subPrefix)
	end
	for _, message in ipairs(msg.nested_type or {}) do
		OutputMessage(message, subPrefix)
	end
	for _, field in ipairs(msg.field or {}) do
		OutputField(field, subPrefix)
	end
	OutputLine(prefix .. "}")
end

local function OutputFile(file)
	-- TODO: consider file.name
	if file.package then
		OutputLine("package " .. file.package .. ";\n")
	end

	local public = {}
	for _, idx in ipairs(file.public_dependency or {}) do public[idx + 1] = true end
	for idx, dep in ipairs(file.dependency or {}) do
		local publicStr = public[idx] and "public " or ""
		OutputLine("import " .. publicStr .. "\"" .. dep .. "\";")
	end
	for _, enum in ipairs(file.enum_type or {}) do
		OutputLine("")
		OutputEnum(enum, "")
	end
	for _, msg in ipairs(file.message_type or {}) do
		OutputLine("")
		OutputMessage(msg, "")
	end
end

local function OutputFileSet(fileSet)
	for _, file in ipairs(fileSet.file) do
		OutputFile(file)
	end
end

local function FieldTableToString(fieldTbl, indent)
	outputTbl = {}
	prefixIndent = indent or "  "
	OutputFileSet(fieldTbl)
	return table.concat(outputTbl, "\n")
end

local fd = io.open(arg[1], "rb")
local msg = fd:read("a")
fd:close()
fd = nil
local fieldTbl = ParseVarint(msg, FileDescriptorSetConfig)
print(tostring(fieldTbl, 16))
local str = FieldTableToString(fieldTbl)
print(str)
