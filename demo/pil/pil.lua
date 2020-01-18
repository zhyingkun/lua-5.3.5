print("======================================================================")
do
	-- defines a factorial function
	local function lfact(n)
		if n == 0 then
			return 1
		else
			return n * lfact(n - 1)
		end
	end
	fact = lfact

	-- print("enter a number:")
	-- print(fact(io.read("*n")))
	print("4!  =", fact(4))
end
print("======================================================================")
do
	-- trim all space in start/end
	local function trim(s)
		return string.gsub(s, "^%s*(.-)%s*$", "%1")
	end
	print(trim("   Middle Character!   "))

	local csrc = "int x; /* x */ int y; /* y */"
	print((string.gsub(csrc, "/%*.*%*/", ""))) --> int x;
	print((string.gsub(csrc, "/%*.-%*/", ""))) --> int x; int y;

	-- %bxy: balance character match
	print((string.gsub("a (enclosed (in) parentheses) line", "%b()", ""))) --> a  line
	-- %f[set]: find a boundary of characters in sets
	-- %w: alphabet and number
	print((string.gsub("the anthem is the theme", "%f[%w]the%f[%W]", "one"))) --> one anthem is one theme
	-- %a: alphabet
	print((string.gsub("hello Lua!", "%a", "%0-%0"))) --> h-he-el-ll-lo-o L-Lu-ua-a!

	print((string.gsub("hello Lua", "(.)(.)", "%2%1"))) --> ehll ouLa
	print(string.gsub([[the \quote{task} is to \em{change} that.]], "\\(%a+){(.-)}", "<%1>%2</%1>")) --> the <quote>task</quote> is to <em>change</em> that.

	print(string.match("name = Anna", "(%a+)%s*=%s*(%a+)")) --> name Anna
	print(string.match("Today is 17/7/1990", "(%d+)/(%d+)/(%d+)")) --> 17 7 1990
	print(string.match([[then he said: "it's all right"!]], [==[(["'])(.-)%1]==])) --> " it's all right
	print(string.match("a = [=[[[ something ]] ]==] ]=]; print(a)", "%[(=*)%[(.-)%]%1%]")) --> = [[ something ]] ]==]
	print(string.match("hello", "()ll()")) --> 3 5
end
print("======================================================================")
do
	local function expand(s)
		return (string.gsub(s, "$(%w+)", _G))
	end
	name = "Lua"; status = "great"
	print(expand("$name is $status, isn't it?")) --> Lua is great, isn't it?
	print(expand("$othername is $status, isn't it?")) --> $othername is great, isn't it?

	local function expand(s)
		return (string.gsub(s, "$(%w+)", function(n)
			return tostring(_G[n])
		end))
	end
	print(expand("print = $print; a = $a")) --> print = function: 0x8050ce0; a = nil
end
print("======================================================================")
do
	local function toxml(s)
		return string.gsub(s, "\\(%a+)(%b{})", function(tag, body)
			body = string.sub(body, 2, -2) -- remove the brackets
			body = toxml(body) -- handle nested commands
			return string.format("<%s>%s</%s>", tag, body, tag)
		end)
	end
	print(toxml("\\title{The \\bold{big} example}")) --> <title>The <bold>big</bold> example</title>
end
print("======================================================================")
do
	local function escape(s)
		s = string.gsub(s, "[&=+%%%c]", function(c)
			return string.format("%%%02X", string.byte(c))
		end)
		return string.gsub(s, " ", "+")
	end
	local function unescape(s)
		s = string.gsub(s, "+", " ")
		return string.gsub(s, "%%(%x%x)", function(h)
			return string.char(tonumber(h, 16))
		end)
	end
	print(unescape("a%2Bb+%3D+c")) --> a+b = c

	local function encode(t)
		local b = {}
		for k,v in pairs(t) do
			b[#b + 1] = (escape(k) .. "=" .. escape(v))
		end
		-- concatenates all entries in 'b', separated by "&"
		return table.concat(b, "&")
	end
	local function decode(s)
		local cgi = {}
		for name, value in string.gmatch(s, "([^&=]+)=([^&=]+)") do
			name = unescape(name)
			value = unescape(value)
			cgi[name] = value
		end
		return cgi
	end
	t = {name = "al", query = "a+b = c", q = "yes or no"}
	print(encode(t))
end
print("======================================================================")
do
	local function expandTabs(s, tab)
		tab = tab or 8 -- tab "size" (default is 8)
		local corr = 0 -- correction
		s = string.gsub(s, "()\t", function(p)
			local sp = tab - (p - 1 + corr)%tab
			corr = corr - 1 + sp
			return string.rep(" ", sp)
		end)
		return s
	end
	local function unexpandTabs(s, tab)
		tab = tab or 8
		s = expandTabs(s, tab)
		local pat = string.rep(".", tab)
		s = string.gsub(s, pat, "%0\1")
		s = string.gsub(s, " +\1", "\t")
		s = string.gsub(s, "\1", "")
		return s
	end
end
print("======================================================================")
do
	print((string.gsub([=[char s[] = "a /* here"; /* a tricky string */]=], "/%*.-%*/", "<COMMENT>"))) --> char s[] = "a <COMMENT>
	-- %a* can match empty string, at the begining of the string
	print(string.find(";$% **#$hello13", "%a*")) --> 1 0

	local function nocase(s)
		s = string.gsub(s, "%a", function(c)
			return "[" .. string.lower(c) .. string.upper(c) .. "]"
		end)
		return s
	end
	print(nocase("Hi there!")) --> [hH][iI] [tT][hH][eE][rR][eE]!
end
print("======================================================================")
do
	function code(s)
		return (string.gsub(s, "\\(.)", function(x)
			return string.format("\\%03d", string.byte(x))
		end))
	end
	function decode(s)
		return (string.gsub(s, "\\(%d%d%d)", function(d)
			return "\\" .. string.char(tonumber(d))
		end))
	end
	local s = [[follows a typical string: "This is \"great\"!".]]
	s = decode(string.gsub(code(s), '".-"', string.upper))
	print(s) --> follows a typical string: "THIS IS \"GREAT\"!".

	local function quote(s)
		-- find maximum length of sequences of equals signs
		local n = -1
		for w in string.gmatch(s, "]=*%f[%]]") do
			n = math.max(n, #w - 1) -- -1 to remove the ']'
		end
		
		-- produce a string with 'n' plus one equals signs
		local eq = string.rep("=", n + 1)
		
		-- build quoted string
		return string.format(" [%s[\n%s]%s] ", eq, s, eq)
	end
end
print("======================================================================")
do
	-- utf8.charpattern = [\0-\x7F\xC2-\xF4][\x80-\xBF]*
	local tmp = string.gsub("a\\u0041", "\\u(%x%x%x%x)", function(str)
		return utf8.char(tonumber(str, 16))
	end)
	print(tmp)
	tmp = string.gsub("abc你好def", "[\xC2-\xF4][\x80-\xBF]*", function(str)
		return "\\u" .. string.format("%04X", utf8.codepoint(str))
	end)
	print(tmp)
end
print("======================================================================")
do
	local function sortfile()
		local lines = {}
		-- read the lines in table 'lines'
		for line in io.lines() do
			lines[#lines + 1] = line
		end
		-- sort
		table.sort(lines)
		-- write all the lines
		for _, l in ipairs(lines) do
			io.write(l, "\n")
		end
	end

	local function fsize(file)
		local current = file:seek() -- save current position
		local size = file:seek("end") -- get file size
		file:seek("set", current) -- restore position
		return size
	end

	local function sqrt(x)
		-- computes the square root of 'x' using Newton-Raphson method
		local sqr = x / 2
		repeat
			sqr = (sqr + x/sqr) / 2
			local error = math.abs(sqr^2 - x)
		until error < x/10000 -- local 'error' still visible here
		return sqr
	end
end
print("======================================================================")
do
	--[=[
	while true do
		::redo::
		if true then
			goto continue
		else if true then
			goto redo
		end
		::continue::
	end
	]=]

	local function mazegame()
		goto room1 -- initial room
		::room1:: do
			local move = io.read()
			if move == "south" then
				goto room3
			elseif move == "east" then
				goto room2
			else
				print("invalid move")
				goto room1 -- stay in the same room
			end
		end
		::room2:: do
			local move = io.read()
			if move == "south" then
				goto room4
			elseif move == "west" then
				goto room1
			else
				print("invalid move")
				goto room2
			end
		end
		::room3:: do
			local move = io.read()
			if move == "north" then
				goto room1
			elseif move == "east" then
				goto room4
			else
				print("invalid move")
				goto room3
			end
		end
		::room4:: do
			print("Congratulations, you won!")
		end
	end
end
print("======================================================================")
do
	local oldSin = math.sin
	local k = math.pi / 180
	math.sin = function(x)
		return oldSin(x * k)
	end
end
print("======================================================================")
do
	local oldOpen = io.open
	local function access_OK(filename, mode)
		-- check access
		return true
	end
	io.open = function(filename, mode)
		if access_OK(filename, mode) then
			return oldOpen(filename, mode)
		else
			return nil, "access denied"
		end
	end
end
print("======================================================================")
do
	local mask = 0x8000000000000000
	print(
		math.maxinteger + 1 == math.mininteger --> true
		,math.mininteger - 1 == math.maxinteger --> true
		,-math.mininteger == math.mininteger --> true
		,math.mininteger // -1 == math.mininteger --> true
		,math.ult(0x7fffffffffffffff, 0x8000000000000000) --> true
		,(0x7fffffffffffffff ~ mask) < (0x8000000000000000 ~ mask) --> true
		,math.maxinteger --> 9223372036854775807
		,0x7fffffffffffffff --> 9223372036854775807
		,math.mininteger --> -9223372036854775808
		,0x8000000000000000 --> -9223372036854775808
		,0x7fffffffffffffff < 0x8000000000000000 --> false
	)
end
print("======================================================================")
do
	-- unsigned division
	local function udiv(n, d)
		if d < 0 then -- d is larger than 2^63
			if math.ult(n, d) then
				return 0
			else
				return 1
			end
		end
		local q = ((n >> 1) // d) << 1 -- quotient
		local r = n - q * d -- remainder, rest
		if not math.ult(r, d) then
			q = q + 1
		end
		return q
	end

	-- convert large unsigned integer to float
	local u = 11529215046068469760 -- unsigned int
	local f = (u + 0.0) % 2^64 -- 2^64 is a double
	print(string.format("%.0f", f)) --> 11529215046068469760

	-- convert large float to unsigned integer
	f = 0xA000000000000000.0
	u = math.tointeger(((f + 2^63) % 2^64) - 2^63)
	print(string.format("%x", u))
	-- for n bit signed integer
	-- first bit is 0, then other bits represent the positive integer, value is num
	-- first bit is 1, then other bits represent the negative integer, value is num - 2^(n-1)
end
print("======================================================================")
do
	local fmt = "iii"
	local str = string.pack("iii", 3, -27, 450)
	print(#str) --> 12
	print(string.unpack(fmt, str)) --> 3 -27 450 13

	local s = "hello\0Lua\0world\0"
	local i = 1
	local tbl = {}
	while i <= #s do
		local res
		res, i = string.unpack("z", s, i)
		tbl[#tbl + 1] = res
	end
	print(table.unpack(tbl))

	local x = string.pack("i7", 1 << 54)
	print(string.unpack("i7", x)) --> 18014398509481984 8
	x = string.pack("i7", -(1 << 54))
	print(string.unpack("i7", x)) --> -18014398509481984 8
	print(pcall(function()
		x = string.pack("i7", 1 << 55)
	end))

	x = string.pack("i12", 2^61)
	print(string.unpack("i12", x)) --> 2305843009213693952 13
	x = "aaaaaaaaaaaa" -- fake a large 12-byte number
	print(pcall(function()
		string.unpack("i12", x)
	end))

	s = "\xFF"
	print(string.unpack("b", s)) --> -1 2
	print(string.unpack("B", s)) --> 255 2

	local s = string.pack("s1", "hello")
	local tbl = {}
	for i = 1, #s do tbl[#tbl + 1] = string.unpack("B", s, i) end
	print(table.unpack(tbl))

	local s = string.pack(">i4", 1000000)
	local tbl = {}
	for i = 1, #s do tbl[#tbl + 1] = string.unpack("B", s, i) end
	print(table.unpack(tbl))

	local s = string.pack("<i2 i2", 500, 24)
	local tbl = {}
	for i = 1, #s do tbl[#tbl + 1] = string.unpack("B", s, i) end
	print(table.unpack(tbl))
end
print("======================================================================")
do
	local function mult(a, b)
		local c = {} -- resulting matrix
		for i = 1, #a do
			local resultline = {} -- will be 'c[i]'
			for k, va in pairs(a[i]) do -- 'va' is a[i][k]
				for j, vb in pairs(b[k]) do -- 'vb' is b[k][j]
					local res = (resultline[j] or 0) + va * vb
					resultline[j] = (res ~= 0) and res or nil
				end
			end
			c[i] = resultline
		end
		return c
	end
end
print("======================================================================")
do
	-- A double-ended queue
	local function listNew()
		return {first = 0, last = -1}
	end

	local function pushFirst(list, value)
		local first = list.first - 1
		list.first = first
		list[first] = value
	end

	local function pushLast(list, value)
		local last = list.last + 1
		list.last = last
		list[last] = value
	end

	local function popFirst(list)
		local first = list.first
		if first > list.last then error("list is empty") end
		local value = list[first]
		list[first] = nil -- to allow garbage collection
		list.first = first + 1
		return value
	end

	local function popLast(list)
		local last = list.last
		if list.first > last then error("list is empty") end
		local value = list[last]
		list[last] = nil -- to allow garbage collection
		list.last = last - 1
		return value
	end
end
print("======================================================================")
do
	local function track(t)
		return setmetatable({}, {
			__index = function(_, k)
				print("*access to element " .. tostring(k))
				return t[k] -- access the original table
			end,
			__newindex = function(_, k, v)
				print("*update of element " .. tostring(k) ..
				" to " .. tostring(v))
				t[k] = v -- update original table
			end,
			__pairs = function()
				return function(_, k) -- iteration function
					local nextkey, nextvalue = next(t, k)
					if nextkey ~= nil then -- avoid last value
						print("*traversing element " .. tostring(nextkey))
					end
					return nextkey, nextvalue
				end
			end,
			__len = function() return #t end
		})
	end

	local function readOnly(t)
		return setmetatable({}, {
			__index = t,
			__newindex = function(t, k, v)
				error("attempt to update a read-only table", 2)
			end
		})
	end

	local function getfield(f)
		local v = _G -- start with the table of globals
		for w in string.gmatch(f, "[%a_][%w_]*") do
			v = v[w]
		end
		return v
	end

	local function setfield(f, v)
		local t = _G -- start with the table of globals
		for w, d in string.gmatch(f, "([%a_][%w_]*)(%.?)") do
			if d == "." then -- not last name?
				t[w] = t[w] or {} -- create table if absent
				t = t[w] -- get the table
			else -- last name
				t[w] = v -- do the assignment
			end
		end
	end

	do
		setmetatable({}, {
			__gc = function(o)
				print("new cycle")
				setmetatable({}, getmetatable(o))
			end
		})
	end
	do
		local t = {
			__gc = function()
				-- your 'atexit' code comes here
				print("finishing Lua program")
			end
		}
		_G["*AA*"] = setmetatable(t, t)
	end

	local results = setmetatable({}, {__mode = "v"}) -- weak value
	function mem_loadstring (s)
		local res = results[s]
		if res == nil then -- result not available?
			res = assert(load(s)) -- compute new result
			results[s] = res -- save for later reuse
		end
		return res
	end
end
print("======================================================================")
do
	function gcpuzzle()
		-- explain output form this code:
		local count = 0
		local mt = {
			__gc = function()
				count = count - 1
			end
		}
		local a = {}
		collectgarbage()
		local cnt = collectgarbage("count") * 1024
		for i = 1, 10000 do
			count = count + 1
			a[i] = setmetatable({}, mt)
			if i <= 10 then
				collectgarbage()
				print(collectgarbage("count") * 1024 - cnt)
			end
		end
		collectgarbage()
		print(collectgarbage("count") * 1024, count)
		a = nil
		collectgarbage()
		print(collectgarbage("count") * 1024, count)
		collectgarbage()
		print(collectgarbage("count") * 1024, count)
	end
end
print("======================================================================")
do
	-- in coroutine, pcall has no setjmp, but resume has
	-- resume will check whether it is not yield and find out there is a pcall
	local co = coroutine.wrap(function() print(pcall(coroutine.yield)) end)
	co()
	co()
end
print("======================================================================")
do
	local function permgen(a, n)
		n = n or #a -- default for 'n' is size of 'a'
		if n <= 1 then -- nothing to change?
			coroutine.yield(a)
		else
			for i = 1, n do
				-- put i-th element as the last one
				a[n], a[i] = a[i], a[n]
				-- generate all permutations of the other elements
				permgen(a, n - 1)
				-- restore i-th element
				a[n], a[i] = a[i], a[n]
			end
		end
	end

	local function permutations(a)
		local co = coroutine.create(function()
			permgen(a)
		end)
		return function() -- iterator
			local code, res = coroutine.resume(co)
			return res
		end
	end

	local function permutations(a)
		return coroutine.wrap(function()
			permgen(a)
		end)
	end

	for p in permutations({1,2,3}) do
		print(table.unpack(p))
	end
end
print("======================================================================")
do
	local cmdQueue = {} -- queue of pending operations
	local lib = {}
	function lib.readline(stream, callback)
		local function nextCmd()
			callback(stream:read())
		end
		table.insert(cmdQueue, nextCmd)
	end
	function lib.writeline(stream, line, callback)
		local function nextCmd()
			callback(stream:write(line))
		end
		table.insert(cmdQueue, nextCmd)
	end
	function lib.stop()
		table.insert(cmdQueue, "stop")
	end
	function lib.runloop()
		while true do
			local nextCmd = table.remove(cmdQueue, 1)
			if nextCmd == "stop" then
				break
			else
				nextCmd() -- perform next operation
			end
		end
	end
	-- return lib
	package.preload["async-lib"] = function() return lib end
end
print("======================================================================")
do
	local lib = require "async-lib"
	local function run(...)
		local finish = 0
		local function tostart()
			finish = finish + 1
		end
		local function tofinish()
			finish = finish - 1
			if finish == 0 then
				lib.stop() -- finish event loop when done
			end
		end
		tostart()
		for k,v in ipairs({...}) do
			coroutine.wrap(function()
				tostart()
				v()
				tofinish()
			end)()
		end
		tofinish()
		lib.runloop() -- start event loop
	end
	local function putline(stream, line)
		local co = coroutine.running() -- calling coroutine
		local function callback()
			coroutine.resume(co)
		end
		lib.writeline(stream, line, callback)
		coroutine.yield()
	end
	local function getline(stream, line)
		local co = coroutine.running() -- calling coroutine
		local function callback(l)
			coroutine.resume(co, l)
		end
		lib.readline(stream, callback)
		local line = coroutine.yield()
		return line
	end

	-- implement a concurrent version
	run(function()
		local t = {}
		local inp = io.input()
		local out = io.output()

		while true do
			local line = getline(inp)
			if not line then break end
			t[#t + 1] = line
			print("One: getline")
		end

		for i = #t, 1, -1 do
			putline(out, t[i] .. "\n")
		end
	end, function()
		local t = {}
		local inp = io.open("./CMakeLists.txt", "r")
		local out = io.open("./test.out", "w")
		while true do
			local line = getline(inp)
			if not line then break end
			t[#t + 1] = line
			print("Two: getline")
		end
		for i = #t, 1, -1 do
			putline(out, t[i] .. "\n")
		end
	end)
end
print("======================================================================")
do
	local coros = {}
	local function create(func)
		table.insert(coros, coroutine.create(func))
	end
	local function dispatch()
		local cacheco = coros
		coros = {}
		for _, co in ipairs(cacheco) do
			coroutine.resume(co)
			if coroutine.status(co) ~= "dead" then
				table.insert(coros, co)
			end
		end
	end
	local function transfer()
		local _, main = coroutine.running()
		if main then
			dispatch()
		else
			coroutine.yield()
		end
	end

	create(function()
		for i = 1, 3, 1 do
			print("First Coroutine")
			transfer()
		end
	end)
	create(function()
		for i = 1, 5, 1 do
			print("Second Coroutine")
			transfer()
		end
	end)
	for i = 1, 5, 1 do
		print("Main Thread")
		transfer()
	end
end
