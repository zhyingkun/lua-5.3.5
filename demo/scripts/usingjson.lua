local json = require("json.cjson")

print("json module:", tostring(json, 16))

local AllocCount = 0
local AllocMem = {}
json.memcb(function(old, new, sz)
	-- print(old, new, sz)
	if old == NULL then -- malloc
		AllocCount = AllocCount + 1
		assert(AllocMem[new] == nil, "Error: alloc a used memory address")
		AllocMem[new] = sz
	elseif new == NULL or sz == 0 then -- free
		AllocCount = AllocCount - 1
		assert(AllocMem[old] ~= nil, "Error: free a not used memory address")
		AllocMem[old] = nil
	else -- realloc
		assert(AllocMem[old] ~= nil, "Error: free a not used memory address")
		AllocMem[old] = nil
		assert(AllocMem[new] == nil, "Error: alloc a used memory address")
		AllocMem[new] = sz
	end
end)
local function PrintMemory()
	local cnt = 0
	for _, sz in pairs(AllocMem) do
		cnt = cnt + sz
	end
	print("Current memory:", cnt)
end

local str = [[{
	"a": "Hello",
	"b": [
		{"c": "nice"},
		{"d": "face"}
	]
}]]
local tbl = json.parse(str)
print("json parse:", tostring(tbl, 16))
PrintMemory()

-- tbl[print] = "" -- this will fire error
tbl.func = print
local str = json.tostring(tbl, function(value)
	print("zykTest:", value)
	if value == print then return "func: print" end
end, false)
print("json tostring:", str)
print("json minify:", json.minify(str))
PrintMemory()

local tbl = json.parse(str)
print("json parse again:", tostring(tbl, 16))

print("Memory: ", AllocCount, tostring(AllocMem, 1))
