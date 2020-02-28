-------------------------------------------------------------------------------
-- parser
-------------------------------------------------------------------------------
local parser = {
    subj = "",
    pos = 1,
}

function parser:setinput(subj, pos)
    self.subj = subj
    self.pos = pos or 1
end

function parser:getinput()
    return self.subj, self.pos
end

function parser:parsebypattern(pat)
    local capture, newpos = string.match(self.subj, pat, self.pos)
    if newpos then
        self.pos = newpos
        return capture
    end
end

local what2pattern = {
    spaces = "^([ \t]*)()",
    word = "^([^ \t\n]+)()",
    newline = "^(\n)()",
    restofline = "^([^\n]*)()"
}

function parser:parse(what)
    return self:parsebypattern(what2pattern[what])
end

function parser:getword()
    self:parse("spaces")
    return self:parse("word")
end

function parser:getwordornewline()
    return self:getword() or self:parse("newline")
end

-------------------------------------------------------------------------------
-- _F, which store all words
-------------------------------------------------------------------------------
local _F = {
    ["%L"] = function()
        return function(modes)
            local f = load(modes.parser:parse("restofline"))
            modes:runfunc(f)
        end
    end
}

-------------------------------------------------------------------------------
-- Internal stack
-------------------------------------------------------------------------------
local stack = {
    DS = {}, -- stack
    n = 0, -- length
}

function stack:push(x)
    local n = self.n
    n = n + 1
    self.n = n
    self.DS[n] = x
end

function stack:pop()
    if self.n == 0 then error("stack has no value!") end
    local n = self.n
    local x = self.DS[n]
    self.DS[n] = nil
    n = n - 1
    self.n = n
    return x
end

-------------------------------------------------------------------------------
-- modes, include all interpret
-------------------------------------------------------------------------------
local modes = {
    mode = "interpret",
    parser = parser,
    _F = _F, -- store all words
    stack = stack,
}

function modes:setmode(mode)
    self.mode = mode
end

function modes:getmode()
    return self.mode
end

function modes:islive()
    return self:getmode() ~= "stop"
end

function modes:runmode()
    self[self.mode](self)
end

function modes:runfunc(f)
    if type(f) == "function" then
        local g = f()
        if type(g) == "function" then
            g(self)
        end
        return true
    elseif type(f) == "string" then
        self:exec(f)
        return true
    else
        return false
    end
end

function modes:interpretprimitive(word)
    local f = self._F[word]
    return self:runfunc(f)
end

function modes:interpretnonprimitive(word)
    return false
end

function modes:interpretnumber(word)
    local num = tonumber(word)
    if num then
        self.stack:push(num)
        return true
    else
        return false
    end
end

function modes:printstate(word)
end

function modes:interpret()
    local word = self.parser:getwordornewline() or ""
    self:printstate(word)
    if self:interpretprimitive(word) then
        return
    end
    if self:interpretnonprimitive(word) then
        return
    end
    if self:interpretnumber(word) then
        return
    end
    error("Can't interpret: " .. word)
end

function modes:exec(subj)
    local oldsubj, oldpos = self.parser:getinput()
    local oldmode = self:getmode()
    self.parser:setinput(subj)
    self:setmode("interpret")
    while self:islive() do
        self:runmode()
    end
    self.parser:setinput(oldsubj, oldpos)
    self:setmode(oldmode)
end

function modes:run(subj)
    self:exec(subj)
    io.write("\n")
end

-------------------------------------------------------------------------------
-- Runtime Insert Word
-------------------------------------------------------------------------------
local defsubj = [=[
    %L return function(modes) modes._F["\n"] = function() end end
    %L return function(modes) modes._F[""] = function() modes:setmode("stop") end end
    %L return function(modes) modes._F["[L"] = function() load(modes.parser:parsebypattern("^(.-)%sL]()"))()(modes) end end
    [L
        return function(modes)
            local _F = modes._F
            local stack = modes.stack
            _F["DUP"] = function()
                local num = stack:pop()
                stack:push(num)
                stack:push(num)
            end
            _F["*"] = function()
                local stack = modes.stack
                stack:push(stack:pop() * stack:pop())
            end
            _F["."] = function()
                io.write(" " .. modes.stack:pop())
            end
        end
    L]
    [L
        return function(modes)
            local _F = modes._F
            local stack = modes.stack
            _F[":"] = function()
                local word = modes.parser:getword()
                local str = modes.parser:parsebypattern("^(.-)%s;()")
                if str == nil then error("Define forth function with ':' should end with ';'") end
                _F[word] = str
            end
            _F[".S"] = function()
                io.write(" <" .. stack.n .. ">")
                for _, v in ipairs(stack.DS) do
                    io.write(" " .. v)
                end
            end
            _F["CR"] = function()
                io.write("\n")
            end
            _F["SWAP"] = function()
                local num1 = stack:pop()
                local num2 = stack:pop()
                stack:push(num1)
                stack:push(num2)
            end
            _F["DROP"] = function()
                stack:pop()
            end
            _F["OVER"] = function()
                local num1 = stack:pop()
                local num2 = stack:pop()
                stack:push(num2)
                stack:push(num1)
                stack:push(num2)
            end
            _F["ROT"] = function()
                local num1 = stack:pop()
                local num2 = stack:pop()
                local num3 = stack:pop()
                stack:push(num2)
                stack:push(num1)
                stack:push(num3)
            end
            _F["-ROT"] = function()
                local num1 = stack:pop()
                local num2 = stack:pop()
                local num3 = stack:pop()
                stack:push(num1)
                stack:push(num3)
                stack:push(num2)
            end
            _F["2DUP"] = function()
                local num1 = stack:pop()
                local num2 = stack:pop()
                stack:push(num2)
                stack:push(num1)
                stack:push(num2)
                stack:push(num1)
            end
            _F["2SWAP"] = function()
                local num1 = stack:pop()
                local num2 = stack:pop()
                local num3 = stack:pop()
                local num4 = stack:pop()
                stack:push(num2)
                stack:push(num1)
                stack:push(num4)
                stack:push(num3)
            end
            _F["PICK"] = function()
                local idx = stack:pop()
                stack:push(stack.DS[stack.n - idx])
            end
            _F["ROLL"] = function()
                local idx = stack:pop()
                local num = table.remove(stack.DS, stack.n - idx)
                stack:push(num)
            end
            _F["MOD"] = function()
                local num1 = stack:pop()
                local num2 = stack:pop()
                stack:push(num2 % num1)
            end
            _F["/MOD"] = function()
                local num1 = stack:pop()
                local num2 = stack:pop()
                stack:push(num2 % num1)
                stack:push(num2 // num1)
            end
            _F["MIN"] = function()
                local num1 = stack:pop()
                local num2 = stack:pop()
                stack:push(num1 < num2 and num1 or num2)
            end
            _F["MAX"] = function()
                local num1 = stack:pop()
                local num2 = stack:pop()
                stack:push(num1 > num2 and num1 or num2)
            end
            _F["NEGATE"] = function()
                stack:push(-stack:pop())
            end
            _F["ABS"] = function()
                stack:push(math.abs(stack:pop()))
            end
            _F["2*"] = function()
                stack:push(stack:pop() * 2)
            end
            _F["2/"] = function()
                stack:push(stack:pop() / 2)
            end
            _F["8*"] = function()
                stack:push(stack:pop() * 8)
            end
            _F["1+"] = function()
                stack:push(stack:pop() + 1)
            end
            _F["1-"] = function()
                stack:push(stack:pop() - 1)
            end
            _F["2+"] = function()
                stack:push(stack:pop() + 2)
            end
            _F["2-"] = function()
                stack:push(stack:pop() - 2)
            end
        end
    L]
]=]

print("Init miniforth")
modes:exec(defsubj)

return modes
