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

local function OutputService(service, prefix)
	OutputLine(prefix .. "service " .. service.name .. " {")
	local subPrefix = prefix .. prefixIndent
	for _, method in ipairs(service.method) do
		OutputLine(subPrefix .. "rpc " .. method.name .. "(" .. NoDot(method.input_type) .. ") returns (" .. NoDot(method.output_type) .. ");")
	end
	OutputLine(prefix .. "}")
end

local OnlyRepeated = true
local function OutputField(field, prefix)
	local typeName = Type[field.type] or NoDot(field.type_name)
	assert(type(typeName) == "string")
	local label = OnlyRepeated and field.label ~= 3 and "" or Label[field.label] .. " "

	local optionsStr = ""
	if next(field.options or {}) then
		for optionName, value in pairs(field.options) do
			optionsStr = "[" .. optionName .. " = " .. value .. "]"
		end
	end

	OutputLine(prefix .. label .. typeName .. " " .. field.name .. " = " .. tostring(field.number) .. " " .. optionsStr .. ";")
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
		OutputLine("option " .. optionName .. " = " .. value .. ";")
	end

	for _, enum in ipairs(file.enum_type or {}) do
		OutputLine("")
		OutputEnum(enum, "")
	end
	for _, msg in ipairs(file.message_type or {}) do
		OutputLine("")
		OutputMessage(msg, "")
	end
	for _, srv in ipairs(file.service or {}) do
		OutputLine("")
		OutputService(srv, "")
	end
end

local function OutputFileSet(fileSet)
	for _, file in ipairs(fileSet.file) do
		OutputFile(file)
	end
end

local function FieldTableToProtoSrc(fieldTbl, indent)
	outputTbl = {}
	prefixIndent = indent or "  "
	OutputFileSet(fieldTbl)
	return table.concat(outputTbl, "\n")
end

--- Parse and Convert features ------

local varint = require("protobuf.varint")
local PBType = varint.PBType
local PBLabel = varint.PBLabel
local FileDescriptorSetConfig = require("protobuf.descriptor_config")(PBLabel, PBType)
local ParseVarint = varint.ParseVarint

local fd = io.open(arg[1], "rb")
local msg = fd:read("a")
fd:close()
fd = nil
local fieldTbl = ParseVarint(msg, FileDescriptorSetConfig)
--[[
local protobuf = require("protobuf")
protobuf.register_file("_descriptor.pb")
local fieldTbl = protobuf.decode("google._protobuf.FileDescriptorSet", msg)
protobuf.extract(fieldTbl)
--]]
-- print(tostring(fieldTbl, 16))
-- print(require("libjson").tostring(fieldTbl))
local str = FieldTableToProtoSrc(fieldTbl)
print(str)
