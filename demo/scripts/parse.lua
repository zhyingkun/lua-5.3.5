local a, b, c = 43, "Hello", true, false
local arg1, arg2 = ...
local total, d = 6
local e = b and c or d

print(arg1, arg2)

local tbl = {
    [1] = 66,
    Nice = "Good",
}
tbl.World = {}
function tbl.World:Famous(p)
    for k, v in pairs(self) do
        print(p, k, v)
    end    
end

tbl.World:Famous {}
tbl.World:Famous "Literal"

do
    print(a, b, c, d, e)
end

::JustDoIt::

for i = 1, 10, 1 do
    if i == 3 then
        break
    elseif i < 2 then
        total = total + i
    else
        total = total + i * 10
    end
end

while c do
    c = false
    repeat
        total = total - 100
    until total < 1
end

local function DayDayUp(...)
    local params = {...}
    print(...)
    return b
end
GoodGoodStudy = function(param, param2)
    return DayDayUp(param, param2, c)
end
local ret = GoodGoodStudy("Lua", "Sun")
print(ret)

if tbl[1] == 66 then
    tbl[1] = 88
    goto JustDoIt
end
