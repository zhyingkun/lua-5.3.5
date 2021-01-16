local UnderScore = ("_"):byte(1)
local LowerMin = ("a"):byte(1)
local LowerMax = ("z"):byte(1)
local UpperMin = ("A"):byte(1)
local UpperMax = ("Z"):byte(1)
local NumberMin = ("0"):byte(1)
local NumberMax = ("9"):byte(1)
local function Between(value, min, max) return min <= value and value <= max end
local function IsLAlpha(byte)
	if byte == UnderScore or
			Between(byte, LowerMin, LowerMax) or
			Between(byte, UpperMin, UpperMax) then
		return true
	end
	return false
end
local function IsLAlpNum(byte)
	if IsLAlpha(byte) or
			Between(byte, NumberMin, NumberMax) then
		return true
	end
	return false
end

local function IsVar(str)
	assert(type(str) == "string")
	if not IsLAlpha(str:byte(1)) then return false end
	for i = 2, #str, 1 do
		if not IsLAlpNum(str:byte(i)) then return false end
	end
	return true
end

local PrintMin = (" "):byte(1)
local PrintMax = ("~"):byte(1)
local function EscapeString(str)
	assert(type(str) == "string")
	local strTbl = {}
	local insert = table.insert
	for i = 1, #str, 1 do
		local byte = str:byte(i)
		if Between(byte, PrintMin, PrintMax) then
			insert(strTbl, str:sub(i, i))
		else
			insert(strTbl, string.format("\\%03d", byte))
		end
	end
	return table.concat(strTbl)
end

local KeyErrMsg = "key in table only support integer and variable string"
local KeyMap = setmetatable({
	number = function(key)
		local num = math.tointeger(key)
		assert(num, KeyErrMsg)
		return "[" .. tostring(num) .. "]"
	end,
	string = function(key)
		assert(IsVar(key), KeyErrMsg)
		return key
	end,
}, {
	__index = function(tbl, key)
		return function(key)
			error(KeyErrMsg)
		end
	end,
})
local ValueErrMsg = "value in table only support number, string, boolean, table"
local ValueMap = setmetatable({
	number = function(value) return tostring(value) end,
	string = function(value) return "\"" .. EscapeString(value) .. "\"" end,
	boolean = function(value) return tostring(value) end,
}, {
	__index = function(tbl, key)
		return function(key)
			error(ValueErrMsg)
		end
	end,
})
local prefixIndent
local insert = table.insert
local function TTLSRecursive(tbl, prefix)
	local subPrefix = prefix .. prefixIndent
	local buffer = {"{"}
	for key, value in pairs(tbl) do
		local keyStr = KeyMap[type(key)](key)
		local tv = type(value)
		local valueStr = tv == "table" and TTLSRecursive(value, subPrefix) or ValueMap[tv](value)
		insert(buffer, subPrefix .. keyStr .. " = " .. valueStr .. ",")
	end
	insert(buffer, prefix .. "}")
	local str = table.concat(buffer, "\n")
	buffer = nil
	return str
end

local function TableToLuaSource(tbl, indent)
	assert(type(tbl) == "table", "First parament should be a table")
	prefixIndent = indent or "  "
	return "return " .. TTLSRecursive(tbl, "")
end

local tbl = {
	hello = {
		nice = "world",
	},
	"good?\234",
	[23] = false,
	["we"] = 43,
}
local str = TableToLuaSource(tbl)
print(str)
-- print(tostring(tbl, 1))
