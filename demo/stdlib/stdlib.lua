print("======================================================================")
print("*** math ***")
do
	function math.deg(n)
		return n * (180.0 / math.pi)
	end

	function math.rad(n)
		return n * (math.pi / 180.0)
	end

	function math.min(...)
		local args = {...}
		local len = #args
		local val = args[1];
		for i=2, len do
			local nval = args[i]
			if nval < val then val = nval end
		end
		return val
	end

	function math.max(...)
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
end
print("======================================================================")
print("*** string ***")
do
	function string.rep(str, n, sep)
		sep = sep or ""
		local result = ""
		for i=1,n-1 do result = result .. str .. sep end
		return result .. str
	end

	print(string.rep("Hello", 3, "_"))

	function string.grep(str, pat)
		assert(type(str) == "string")
		assert(type(pat) == "string")
		for v in str:gmatch("[^\r\n]*") do
			if v:find(pat) then print(v) end
		end
	end

	print(string.grep(tostring(string, 1), "sub"))
end
print("======================================================================")
print("*** table ***")
do
	function table.concat(list, sep, i, j)
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

	function table.maxn(list)
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

	function table.insert(...)
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

	function table.pack(...)
		local list = {...}
		list.n = #list
		return list
	end

	do
		local function unpack(list, i, n)
			i = i or 1
			n = n or #list
			if i <= n then
				return list[i], unpack(list, i + 1, n)
			end
		end

		function table.unpack(list, i, n)
			return unpack(list, i, n)
		end
	end

	function table.remove(list, pos)
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

	function table.move(list, first, ed, start, list2)
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

	do
		local function partition(list, comp, low, up)
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

		local function recusiveSort(list, comp, low, up)
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

		function table.sort(list, comp)
			local len = #list
			comp = comp or function(a, b) return a < b end
			print(len, comp(list[2], list[3]))
			if len > 1 then
				recusiveSort(list, comp, 1, len)
			end
		end
	end

	local list = {"I", "like", "you", "!"}
	table.insert(list, 2, "very")
	print(table.remove(list, #list+1))
	print(table.maxn(list), table.concat(list, " "))

	local tmp = table.move(list, 2, 4, 1, {})
	for k,v in pairs(tmp) do print("pair:", k,v) end

	table.sort(list)
	print(table.maxn(list), table.concat(list, " _ "))
end
print("======================================================================")
print("*** debug ***")
do
	function debug.debug()
		while true do
			io.stderr:write("debug> ")
			local line = io.stdin:read()
			if line == "cont" or line == nil then break end
			local func, err = load(line)
			if func == nil then
				io.stderr:write(err, "\n")
			else
				func()
			end
		end
	end

	-- debug.debug()
end
print("======================================================================")
print("*** package ***")
do
	function package.searchpath(modname, path)
		modname = string.gsub(modname, "%.", "/")
		local msg = {}
		for c in string.gmatch(path, "[^;]+") do
			local fname = string.gsub(c, "?", modname)
			local f = io.open(fname)
			if f then
				f:close()
				return fname
			else
				msg[#msg + 1] = string.format("\n\tno file '%s'", fname);
			end
		end
		return nil, table.concat(msg) -- not found
	end

	local modname = "libhello"
	print(modname, package.searchpath(modname, package.cpath))
end
print("======================================================================")
print("*** global table ***")
do
	function _G.assert(v, message, ...)
		if v then return v, message, ... end
		error(message or "assertion failed!")
	end

	assert(type(nil) == "nil", "type of nil is not 'nil'")

	function _G.select(index, ...)
		local args = {...}
		if type(index) == "string" and string.byte(index) == string.byte("#") then
			return #args
		end
		if type(index) == "number" then
			index = math.tointeger(index)
		else
			index = nil
		end
		if not index then
			error("#1 argument should be integer or '#'")
		end
		for i = 1, index - 1, 1 do
			table.remove(args, 1)
		end
		return table.unpack(args)
	end

	print(select("#", 1, 2, 3, 4))
	print(select(3, 1, 2, 3, 4))

	function _G.ipairs(list)
		return function(list, key)
			if key >= #list then return nil end
			key = key + 1
			return key, list[key]
		end, list, 0
	end

	for k,v in ipairs({"Hello", "Nice", "Good"}) do print("ipair:", k, v) end

	function _G.pairs(list)
		local tbl = getmetatable(list)
		local __pairs
		if type(tbl) == "table" then __pairs = tbl.__pairs end
		if __pairs then
			local next, invariant, key = __pairs(list)
			return next, invariant, key
		end
		return next, list, nil
	end

	for k,v in pairs({"pairs", Hello = "World", Nice = "Good"}) do print("pairs", k, v) end

	function _G.require(modname)
		local loadedVal = package.loaded[modname]
		if loadedVal then
			return loadedVal
		end
		local errorMsg = ""
		for _, searcher in ipairs(package.searchers) do
			local loader, absPath = searcher(modname)
			local typeOfLoader = type(loader)
			if typeOfLoader == "function" then
				local ret = loader(modname, absPath)
				if ret ~= nil then
					package.loaded[modname] = ret
				else
					local origin = package.loaded[modname]
					if origin == nil then
						ret = true
						package.loaded[modname] = true
					else
						ret = origin
					end
				end
				return ret
			elseif typeOfLoader == "string" then
				errorMsg = errorMsg .. loader
			else
				-- should not run here
			end
		end
		error("module " .. modname .. " not found:" .. errorMsg)
		-- no way to here
	end
	
	require("libhello").hellomod()

	function _G.print(...)
		local stdout = io.stdout
		for k,v in ipairs({...}) do
			local str = tostring(v)
			if type(str) ~= "string" then
				error("'tostring' must return a string to 'print'")
			end
			if k > 1 then stdout:write("\t") end
			stdout:write(str)
		end
		stdout:write("\n")
		stdout:flush()
	end

	print("Hello World!")
end
print("======================================================================")
