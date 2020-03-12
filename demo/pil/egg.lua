--[=[
    Two Eggs Problem
    To find the minial times for checking which floor is the boundary
]=]
local maxeggs = 0
local memorize = {}
local function DropTimes(nfloors, neggs)
    -- assert(nfloors > 0 and neggs > 0)
    local idx = nfloors * maxeggs + neggs
    local value = memorize[idx]
    if value then return value end

    local n
    if neggs == 1 then
        n = nfloors
    elseif nfloors == 1 then
        n = 1
    else
        n = DropTimes(nfloors - 1, neggs) -- for first floor or last floor
        for k = 2, nfloors - 1, 1 do
            local val = DropTimes(k - 1, neggs - 1)
            local val1 = DropTimes(nfloors - k, neggs)
            if val < val1 then val = val1 end
            if val < n then n = val end
        end
        n = n + 1
    end

    memorize[idx] = n
    return n
end
local function FindTwoEggsTimes(nfloors, neggs)
    maxeggs = neggs
    memorize = {}
    return DropTimes(nfloors, neggs)
end
print(FindTwoEggsTimes(100, 2))
print(FindTwoEggsTimes(5000, 3))
