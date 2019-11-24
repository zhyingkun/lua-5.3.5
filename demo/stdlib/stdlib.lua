table.concat = function(list, sep, i, j)
	sep = sep or ""
	i = i or 1
	j = j or #list
	local result = ""
	for index = i, j do
		result = result .. list[index] .. sep
	end
	return result
end

table.maxn = function(list)
	if type(list) ~= "table" then
		return 0
	end
	local max = 0
	for k,v in pairs(list) do
		if type(k) == "number" and k > max then
			max = k
		end
	end
	return max
end

table.insert = function(...)
	local args = {...}
	local argc = #args
	local list = args[1]
	if argc == 2 then
		list[argc+1] = args[2]
	elseif argc == 3 then
		local pos = args[2]
		for i = #list, pos, -1 do
			list[i+1] = list[i]
		end
		list[pos] = args[3]
	else
		error("table.insert should have 2 or 3 arguments")
	end
end

table.pack = function(...)
	local list = {...}
	list.n = #list
	return list
end

-- table.unpack = function(list)
-- end

table.remove = function(list, pos)
	local size = #list
	pos = pos or size
	if pos ~= size then
		if pos < 1 or pos > size+1 then
			error("position out of bounds")
		end
	end
	local result = list[pos]
	if pos <= size then
		for i = pos, size do
			list[i] = list[i+1]
		end
	end
	return result
end

table.move = function(list, first, ed, start, list2)
	list2 = list2 or list
	if ed < first then return list2 end
	local count = ed - first
	if start < first or start > ed or list2 ~= list then
		for index = 0, count do
			list2[start+index] = list[first+index]
		end
	else
		for index = count, 0, -1 do
			list2[start+index] = list[first+index]
		end
	end
	return list2
end

local list = {"I", "like", "you", "!"}
table.insert(list, 2, "very")
print(table.remove(list, #list+1))
print(table.maxn(list), table.concat(list, " "))

local tmp = table.move(list, 2, 4, 1, {})
for k,v in pairs(tmp) do print(k,v) end
