local tabStrCache = {}
local function GetTabStr(size)
	local cache = tabStrCache[size]
	if cache ~= nil then
		return cache
	end
	local str = ""
	for i = 1, size, 1 do
		str = str .. "\t"
	end
	tabStrCache[size] = str
	return str
end

local DetailStr = {}
function DetailStr:Init(level)
	self.strBuffer = {}
	self.recordTable = {}
	self.totalLevel = level
	self.currentLevel = 0
end
function DetailStr:Clear()
	self.strBuffer = nil
	self.recordTable = nil
end
function DetailStr:GetFinalString()
	return table.concat(self.strBuffer)
end
function DetailStr:RecordTable(tbl, level)
	self.recordTable[tbl] = level
end
function DetailStr:CheckTable(tbl)
	local level = self.recordTable[tbl]
	if level ~= nil and level < self.currentLevel then
		return true -- means this table has been recorded, maybe print to buffer later
	end
	return false
end
function DetailStr:RecordSubTable(tbl)
	for _, v in pairs(tbl) do
		if type(v) == "table" and self:CheckTable(tbl) == false then
			self:RecordTable(tbl, self.currentLevel)
		end
	end
end
local origin_tostring = tostring
local insert = table.insert
function DetailStr:RecusiveToString(tbl)
	insert(self.strBuffer, origin_tostring(tbl))
	insert(self.strBuffer, " {\n")
	self:RecordTable(tbl, 0)
	self.currentLevel = self.currentLevel + 1
	local canRecursive = self.currentLevel < self.totalLevel
	if canRecursive then
		self:RecordSubTable(tbl)
	end
	for k, v in pairs(tbl) do
		insert(self.strBuffer, GetTabStr(self.currentLevel))
		insert(self.strBuffer, origin_tostring(k))
		insert(self.strBuffer, " => ")
		if canRecursive and type(v) == "table" and self:CheckTable(v) == false then
			self:RecusiveToString(v)
		else
			insert(self.strBuffer, origin_tostring(v))
		end
		insert(self.strBuffer, "\n")
	end
	self.currentLevel = self.currentLevel - 1
	insert(self.strBuffer, GetTabStr(self.currentLevel))
	insert(self.strBuffer, "}")
end

local MAX_TAB_SIZE = 16

function tostring(var, level)
	if level == nil or tonumber(level) <= 0 or type(var) ~= "table" then return origin_tostring(var) end
	if level > MAX_TAB_SIZE then level = MAX_TAB_SIZE end
	DetailStr:Init(level)
	DetailStr:RecusiveToString(var)
	local str = DetailStr:GetFinalString()
	DetailStr:Clear()
	return str
end
