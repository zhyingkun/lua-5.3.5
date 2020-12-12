print("======================================================================")
print("Make sure call finalizer and release memory in one collectgarbage")
do
	collectgarbage("stop")
	while debug.getgcstate() ~= "GCSpause" do collectgarbage("onestep") end
	local tmp = {}
	print(tmp)
	setmetatable(tmp, {__gc = function(obj) print("In Finalizer") end})
	tmp = nil
	while debug.getgcstate() ~= "GCSswpallgc" do collectgarbage("onestep") end
	print("Start First Collect")
	collectgarbage() --> will call finalizer and release memory of table 'tmp'
	print("Start Second Collect")
	collectgarbage()
	print("End Collect")
	collectgarbage("restart")
end
print("======================================================================")
print("Make sure setting table will cause rehash with nil value in that table")
do
	local num = 1
	for i = 1, num, 1 do
		local tmp = {}
		tmp.a = true
		tmp.b = true
		tmp.b = nil
		for j = 1, 10 do
			tmp[tostring(j)] = true --> may be cause rehash, after rehash, table size doesn't change
			tmp[tostring(j)] = nil
		end
	end
end
print("======================================================================")
print("Make sure tostring support deep nested table")
do
	local tbl = {}
	tbl[_G] = true
	tbl.hello = "nice"
	print(tostring(tbl, 1))
	print(tostring(tbl, 1))
	print(tbl[_G])
	local tmp = tbl
	for i = 1, 20, 1 do
		local t = {
			haha = 43,
		}
		tmp.sub = t
		tmp = t
	end
	print(tostring(tbl, 20))
end
print("======================================================================")
