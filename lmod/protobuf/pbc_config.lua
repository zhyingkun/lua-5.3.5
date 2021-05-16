return function(PBLabel, PBType)
	--- Configs for simple descriptor.proto Begin ------

	local Optional = PBLabel.Optional
	local Required = PBLabel.Required
	local Repeated = PBLabel.Repeated

	local Int32 = PBType.Int32
	local String = PBType.String
	local Double = PBType.Double

	--[[
	local Label = {
		[0] = "optional",
		"required",
		"repeated"
	}
	--]]

	local PBCFieldConfig = {
		{ "name", Optional, String },
		{ "id", Optional, Int32 },
		{ "label", Optional, Int32 }, -- function(field) return Label[field] end
		{ "type", Optional, Int32 },
		{ "type_name", Optional, String },
		{ "default_int", Optional, Int32 },
		{ "default_string", Optional, String },
		{ "default_real", Optional, Double },
	}

	local PBCFileConfig = {
		{ "name", Optional, String },
		{ "dependency", Repeated, String },
		{ "message_name", Repeated, String },
		{ "message_size", Repeated, Int32 },
		{ "message_field", Repeated, PBCFieldConfig },
		{ "enum_name", Repeated, String },
		{ "enum_size", Repeated, Int32 },
		{ "enum_string", Repeated, String },
		{ "enum_id", Repeated, Int32 },
	}

	--- Configs for simple descriptor.proto End ------

	return PBCFileConfig
end
