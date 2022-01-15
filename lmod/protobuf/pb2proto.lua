--- Configs for option Begin ------
local function AsString(value) return "\"" .. value .. "\"" end
local function AsBool(value) return value == 1 and "true" or "false" end

local FileOptionsSrc = {
	java_package = AsString,
	java_outer_classname = AsString,
	java_multiple_files = AsBool,
	java_generate_equals_and_hash = AsBool,
	java_string_check_utf8 = AsBool,
	optimize_for = function(value)
		local OptimizeMode = { "SPEED", "CODE_SIZE", "LITE_RUNTIME" }
		return OptimizeMode[value]
	end,
	go_package = AsString,
	cc_generic_services = AsBool,
	java_generic_services = AsBool,
	py_generic_services = AsBool,
	php_generic_services = AsBool,
	deprecated = AsBool,
	cc_enable_arenas = AsBool,
	objc_class_prefix = AsString,
	csharp_namespace = AsString,
	swift_prefix = AsString,
	php_class_prefix = AsString,
	php_namespace = AsString,
	php_metadata_namespace = AsString,
	ruby_package = AsString,
}
local EnumOptionsSrc = {
	allow_alias = AsBool,
	deprecated = AsBool,
}
local EnumValueOptionsSrc = {
	deprecated = AsBool,
}
local MessageOptionsSrc = {
	message_set_wire_format = AsBool,
	no_standard_descriptor_accessor = AsBool,
	deprecated = AsBool,
	map_entry = AsBool,
}
local FieldOptionsSrc = {
	ctype = function(value)
		local CType = { [0] = "STRING", "CORD", "STRING_PIECE" }
		return CType[value]
	end,
	packed = AsBool,
	jstype = function(value)
		local JSType = { [0] = "JS_NORMAL", "JS_STRING", "JS_NUMBER" }
		return JSType[value]
	end,
	lazy = AsBool,
	deprecated = AsBool,
	weak = AsBool,
}
local ServiceOptionsSrc = {
	deprecated = AsBool,
}
local MethodOptionsSrc = {
	deprecated = AsBool,
	idempotency_level = function(value)
		local IdempotencyLevel = {
			[0] = "IDEMPOTENCY_UNKNOWN",
			"NO_SIDE_EFFECTS",
			"IDEMPOTENT",
		}
		return IdempotencyLevel[value]
	end,
}

local function GetOptionSrc(config, name, value)
	local func = config[name]
	if func then return func(value) end
	return "UninterpretedOption"
end
local function GetOneLineOptionSrc(config, options, other)
	local srcTbl = other or {}
	for optionName, value in pairs(options or {}) do
		local src = GetOptionSrc(config, optionName, value)
		table.insert(srcTbl, optionName .. " = " .. src)
	end
	if next(srcTbl) then
		return " [" .. table.concat(srcTbl, ", ") .. "]"
	end
	return ""
end
--- Configs for option End ------

--- Output source to buffer Start ------
local oneIndent
local indentCnt = 0
local function IncreaseIndent() indentCnt = indentCnt + 1 end
local function DecreaseIndent() indentCnt = indentCnt - 1 end
local indentCache = { [0] = "" }
local function GetCurrentIndent()
	local indent = indentCache[indentCnt]
	if not indent then
		indent = string.rep(oneIndent, indentCnt)
		indentCache[indentCnt] = indent	
	end
	return indent
end

local outputTbl
local insert = table.insert
local function OutputLine(str)
	if str ~= "" then str = GetCurrentIndent() .. str end
	insert(outputTbl, str)
end
--- Output source to buffer End ------

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

local function OutputOptions(options, config)
	for optionName, value in pairs(options or {}) do
		local optionSrc = GetOptionSrc(config, optionName, value)
		OutputLine("option " .. optionName .. " = " .. optionSrc .. ";")
	end
end
local function OutputReserved(ranges, names)
	local rangeStrs = {}
	for _, range in ipairs(ranges or {}) do
		local s = range["start"]
		local e = range["end"]
		local str = tostring(s)
		if s ~= e then
			str = str .. " to " .. tostring(e)
		end
		table.insert(rangeStrs, str)
	end
	if next(rangeStrs) then
		OutputLine("reserved " .. table.concat(rangeStrs, ", ") .. ";")
	end
	local nameStrs = {}
	for _, name in ipairs(names or {}) do
		table.insert(nameStrs, "\"" .. name .. "\"")
	end
	if next(nameStrs) then
		OutputLine("reserved " .. table.concat(nameStrs, ", ") .. ";")
	end
end

local function OutputEnum(enum)
	OutputLine("enum " .. enum.name .. " {")
	IncreaseIndent()

	OutputOptions(enum.options, EnumOptionsSrc)
	OutputReserved(enum.reserved_range, enum.reserved_name)

	for _, v in ipairs(enum.value) do
		local optionsStr = GetOneLineOptionSrc(EnumValueOptionsSrc, v.options)
		OutputLine(v.name .. " = " .. v.number .. optionsStr .. ";")
	end
	DecreaseIndent()
	OutputLine("}")
end

local function NoDot(str) return str:byte(1) == ("."):byte(1) and str:sub(2, -1) or str end

local function OutputService(service)
	OutputLine("service " .. service.name .. " {")
	IncreaseIndent()
	OutputOptions(service.options, ServiceOptionsSrc)
	for _, method in ipairs(service.method) do
		local subFix = ";"
		if method.options then subFix = " {" end
		local client = method.client_streaming == 1 and "stream " or ""
		local server = method.server_streaming == 1 and "stream " or ""
		OutputLine("rpc " .. method.name .. "(" .. client .. NoDot(method.input_type) .. ") returns (" .. server .. NoDot(method.output_type) .. ")" .. subFix)
		if method.options then
			IncreaseIndent()
			OutputOptions(method.options, MethodOptionsSrc)
			DecreaseIndent()
			OutputLine("}")
		end
	end
	DecreaseIndent()
	OutputLine("}")
end

local FieldDefaultValue = { -- fieldType => function(value) return OptionStr end
	tonumber,
	tonumber,
	math.tointeger,
	math.tointeger,
	math.tointeger,
	math.tointeger,
	math.tointeger,
	function(value) return ({["true"] = true, ["false"] = false})[value] end,
	function(value) return [["]] .. value .. [["]] end,
	nil, -- TYPE_GROUP
	nil, -- TYPE_MESSAGE
	function(value) return [["]] .. value .. [["]] end,
	math.tointeger,
	nil, -- TYPE_ENUM
	math.tointeger,
	math.tointeger,
	math.tointeger,
	math.tointeger,
}
local OnlyRepeated = true
local function OutputField(field)
	local typeName = Type[field.type] or NoDot(field.type_name)
	assert(type(typeName) == "string")
	local label = OnlyRepeated and field.label ~= 3 and "" or Label[field.label] .. " "
	local other = {}
	if field.json_name ~= field.name then
		table.insert(other, "json_name = \"" .. field.json_name .. "\"")
	end
	if field.default_value ~= nil then
		table.insert(other, "default = " .. FieldDefaultValue[field.type](field.default_value))
	end
	local optionsStr = GetOneLineOptionSrc(FieldOptionsSrc, field.options, other)
	OutputLine(label .. typeName .. " " .. field.name .. " = " .. tostring(field.number) .. optionsStr .. ";")
end

local function OutputOneofFields(decl, fields)
	OutputLine("oneof " .. decl.name .. " {")
	IncreaseIndent()
	for _, field in ipairs(fields) do
		OutputField(field)
	end
	DecreaseIndent()
	OutputLine("}")
end

local function OutputMessage(msg)
	OutputLine("message " .. msg.name .. " {")
	IncreaseIndent()
	OutputOptions(msg.options, MessageOptionsSrc)
	OutputReserved(msg.reserved_range, msg.reserved_name)
	for _, enum in ipairs(msg.enum_type or {}) do
		OutputEnum(enum)
	end
	for _, message in ipairs(msg.nested_type or {}) do
		OutputMessage(message)
	end
	local normalFields = {}
	local oneofs = {}
	for _, field in ipairs(msg.field or {}) do
		if field.oneof_index then
			local oneof = oneofs[field.oneof_index]
			if not oneof then
				oneof = {}
				oneofs[field.oneof_index] = oneof
			end
			table.insert(oneof, field)
		else
			table.insert(normalFields, field)
		end
	end
	for _, field in ipairs(normalFields) do
		OutputField(field)
	end
	for idx, decl in ipairs(msg.oneof_decl or {}) do
		OutputOneofFields(decl, oneofs[idx - 1])
	end

	DecreaseIndent()
	OutputLine("}")
end

local function OutputFile(file)
	if file.syntax then
		OutputLine("syntax = \"" .. file.syntax .. "\";")
	end

	if file.package then
		OutputLine("package " .. file.package .. ";")
	end

	local public = {}
	for _, idx in ipairs(file.public_dependency or {}) do public[idx + 1] = true end
	if next(file.dependency or {}) then OutputLine("") end
	for idx, dep in ipairs(file.dependency or {}) do
		local publicStr = public[idx] and "public " or ""
		OutputLine("import " .. publicStr .. "\"" .. dep .. "\";")
	end

	if next(file.options or {}) then OutputLine("") end
	for optionName, value in pairs(file.options or {}) do
		local optionSrc = GetOptionSrc(FileOptionsSrc, optionName, value)
		OutputLine("option " .. optionName .. " = " .. optionSrc .. ";")
	end

	for _, enum in ipairs(file.enum_type or {}) do
		OutputLine("")
		OutputEnum(enum)
	end
	for _, msg in ipairs(file.message_type or {}) do
		OutputLine("")
		OutputMessage(msg)
	end
	for _, srv in ipairs(file.service or {}) do
		OutputLine("")
		OutputService(srv)
	end
end

local function OutputFileSet(fileSet)
	for _, file in ipairs(fileSet.file) do
		OutputFile(file)
	end
end

local function FieldTableToProtoSrc(fieldTbl, indent)
	outputTbl = {}
	oneIndent = indent or "  "
	OutputFileSet(fieldTbl)
	return table.concat(outputTbl, "\n")
end

--- Parse and Convert features ------

local varint = require("protobuf.varint")
local PBType = varint.PBType
local PBLabel = varint.PBLabel
local FileDescriptorSetConfig = require("protobuf.descriptor_config")(PBLabel, PBType)
local ParseVarint = varint.ParseVarint

--[[
local fd = io.open(arg[1], "rb")
local msg = fd:read("a")
fd:close()
fd = nil
local fieldTbl = ParseVarint(msg, FileDescriptorSetConfig)
--]]
--[[
local protobuf = require("protobuf")
protobuf.registerFile("_descriptor.pb")
local fieldTbl = protobuf.decode("google._protobuf.FileDescriptorSet", msg)
protobuf.extract(fieldTbl)
--]]
-- print(tostring(fieldTbl, 16))
-- print(require("libjson").tostring(fieldTbl))
-- local str = FieldTableToProtoSrc(fieldTbl)
-- print(str)

return function(pbbin)
	return FieldTableToProtoSrc(ParseVarint(pbbin, FileDescriptorSetConfig))
end
