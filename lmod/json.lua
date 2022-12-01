local c = require "libjson"

local function IsSequence(tbl)
	local idx = 1;
	for key in pairs(tbl) do
		if key ~= idx then return false end
		idx = idx + 1;
	end
	return true
end
local LuaValueToCJson
local TableUnique
local function TableToCJson(tbl)
	if TableUnique[tbl] then
		error("Error: table contains ring")
	end
	TableUnique[tbl] = true
	local bIsSeq = IsSequence(tbl)
	local item
	if bIsSeq then
		item = c.create_array()
	else
		item = c.create_object()
	end
	for key, value in pairs(tbl) do
		local tk = type(key)
		if tk ~= "string" and tk ~= "number" then
			error("key should be string or number")
		end
		local subItem = LuaValueToCJson(value)
		if subItem then
			if bIsSeq then
				c.add_item_to_array(item, subItem)
			else
				c.add_item_to_object(item, key, subItem)
			end
		end
	end
	return item
end
local FuncToCJson = {
	bool = c.create_value,
	number = c.create_value,
	string = c.create_value,
	table = TableToCJson,
}
local ConvertFunc
function LuaValueToCJson(value)
	local func = FuncToCJson[type(value)]
	if not func then
		if not ConvertFunc then return end
		value = ConvertFunc(value)
		if not value then return end
		-- return LuaValueToCJson(value)
		func = FuncToCJson[type(value)]
		if not func then return end
	end
	return func(value)
end


local CJsonToLuaValue
local function CJsonArrayToTable(item)
	local tbl = {}
	c.for_each_array_subitem(item, function(key, subItem)
		tbl[key + 1] = CJsonToLuaValue(subItem)
	end)
	return tbl
end
local function CJsonObjectToTable(item)
	local tbl = {}
	c.for_each_object_subitem(item, function(key, subItem)
		tbl[key] = CJsonToLuaValue(subItem)
	end)
	return tbl
end
local FuncToLuaValue = {
	[0] = function(item) return false end,
	[1] = function(item) return false end,
	[2] = function(item) return nil end,
	[3] = c.get_value,
	[4] = c.get_value,
	[5] = CJsonArrayToTable,
	[6] = CJsonObjectToTable,
}
function CJsonToLuaValue(item)
	local t = c.type(item)
	local func = FuncToLuaValue[t]
	return func(item)
end

---@class json:table
local json = {}

--[[
function convertFunc(value)
	return nil/bool/number/string/table -- nil for drop it
end
--]]
---@param value any
---@param convertFunc fun(value:any):nil | boolean | number | string | table
---@param fmt boolean
---@return string
function json.tostring(value, convertFunc, fmt)
	ConvertFunc = type(convertFunc) == "function" and convertFunc or nil
	TableUnique = {}
	local item = LuaValueToCJson(value)
	TableUnique = nil
	if not item then return "" end
	local str = c.print(item, fmt)
	c.delete(item)
	return str
end

---@param str string
---@return any
function json.parse(str)
	local item = c.parse(str)
	if not item then return end
	local value = CJsonToLuaValue(item)
	c.delete(item)
	return value
end

---@param str string
---@return string
function json.minify(str)
	return c.minify(str)
end

---@param callback MemAllocCallback | "function(oldPtr, newPtr, newSize) end"
function json.setMemoryAllocatedCallback(callback)
	c.set_realloc_cb(callback)
end

return json
