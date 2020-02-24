print("======================================================================")
do
	function memoize(f)
		local mem = setmetatable({}, {__mode="kv"})
		return function(x)
			local r = mem[x]
			if r == nil then
				r = f(x)
				mem[x] = r
			end
			return r
		end
	end
end
print("======================================================================")
do
	function vardump(value, depth, key)
		local linePrefix = key and "["..key.."] = " or ""
		depth = depth or 0
		local spaces = ""
		for i = 1, depth do
			spaces = spaces .. "    "
		end
		local toval = type(value)
		if toval == "table" then
			local mTable = getmetatable(value)
			if mTable == nil then
				print(spaces .. linePrefix .. "(table) ")
			else
				print(spaces .. "(metatable) ")
				value = mTable
			end
			for tableKey, tableValue in pairs(value) do
				vardump(tableValue, depth + 1, tableKey)
			end
		elseif toval == "function" or toval == "thread" or toval == "userdata" or toval == "nil" then
			print(spaces .. linePrefix .. tostring(value))
		else
			print(spaces .. linePrefix .. "(" .. toval .. ")" .. tostring(value))
		end
	end
end
