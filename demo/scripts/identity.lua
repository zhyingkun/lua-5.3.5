local zero = ("0"):byte(1)
local function idnum(str, idx)
	return str:byte(idx) - zero
end
local weight = {7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2}
local function calmod11(idStr)
	local sum = 0
	for i = 1, 17, 1 do
		sum = sum + idnum(idStr, i) * weight[i]
	end
	return (12 - sum % 11) % 11
end

local id = "12345675901234567X"
local num17 = idnum(id, 17)
local mod11 = calmod11(id)
print("mod11:", mod11, (num17 % 2) == 1 and "male" or "female")

if mod11 == 10 then
	assert(id:sub(18, 18) == "X")
else
	assert(mod11 == idnum(id, 18))
end

print("End")
