local pbc = require("libprotobuf")
local ParseVarint = require("varint")

local PBCFieldConfig = {
	{ "name", false, nil },
	{ "id", false, nil },
	{ "label", false, nil },
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
print(tostring(fieldTbl, 16))
