-- Math

math.deg = function(n)
	return n * (180.0 / math.pi)
end

math.rad = function(n)
	return n * (math.pi / 180.0)
end

math.min = function(...)
	local args = {...}
	local len = #args
	local val = args[1];
	for i=2, len do
		local nval = args[i]
		if nval < val then val = nval end
	end
	return val
end

math.max = function(...)
	local args = {...}
	local len = #args
	local val = args[1];
	for i=2, len do
		local nval = args[i]
		if nval > val then val = nval end
	end
	return val
end

print(math.deg(math.pi/2), math.rad(90.0))
print(math.min(3, 4, 7, 1, 2, 5), math.max(3, 4, 7, 1, 2, 5))


-- String

string.rep = function(str, n, sep)
	sep = sep or ""
	local result = ""
	for i=1,n-1 do result = result .. str .. sep end
	return result .. str
end

print(string.rep("Hello", 3, "_"))


-- Table

table.concat = function(list, sep, i, j)
	i = i or 1
	j = j or #list
	if j < i then return "" end
	sep = sep or ""
	local result = ""
	for index = i, j-1 do
		result = result .. list[index] .. sep
	end
	return result .. list[j]
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

table.unpack = function(list, i, n)
	i = i or 1
	n = n or #list
	if i <= n then
		return list[i], unpack(list, i + 1, n)
	end
end

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

local partition = function(list, comp, low, up)
	local i = low
	local j = up - 1
	while true do
		i = i + 1
		local vp = list[up - 1]
		while comp(list[i], vp) do
			i = i + 1
		end
		j = j - 1
		while comp(vp, list[j]) do
			j = j - 1
		end
		if j < i then
			list[up - 1] = list[i]
			list[i] = vp
			return i
		end
		local tmp = list[i]
		list[i] = list[j]
		list[j] = tmp
	end
end

local recusiveSort
recusiveSort = function(list, comp, low, up)
	while low < up do
		local vlow = list[low]
		local vup = list[up]
		if comp(vup, vlow) then 
			list[low] = vup
			list[up] = vlow
		end
		if (up - low) == 1 then return end
		local p = (low + up) / 2 -- Pivot
		local vp = list[p]
		vlow = list[low]
		if comp(vp, vlow) then
			list[low] = vp
			list[p] = vlow
		else
			vup = list[up]
			if comp(vup, vp) then
				list[p] = vup
				list[up] = vp
			end
		end
		if (up - low) == 2 then return end
		vp = list[p]
		local vupmo = list[up - 1] -- Value of "Up Minus One"
		list[p] = vupmo
		list[up-1] = vp
		p = partition(list, comp, low, up);
		recusiveSort(list, comp, low, p - 1);
		low = p + 1
	end
end

table.sort = function(list, comp)
	local len = #list
	comp = comp or function(a, b) return a < b end
	print(len, comp(list[2], list[3]))
	if len > 1 then
		recusiveSort(list, comp, 1, len)
	end
end

local list = {"I", "like", "you", "!"}
table.insert(list, 2, "very")
print(table.remove(list, #list+1))
print(table.maxn(list), table.concat(list, " "))

local tmp = table.move(list, 2, 4, 1, {})
for k,v in pairs(tmp) do print(k,v) end

table.sort(list)
print(table.maxn(list), table.concat(list, " _ "))
