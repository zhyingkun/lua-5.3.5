-------------------------------------------------------------------------------
-- parser
-------------------------------------------------------------------------------
local parser = {
    subj = "",
    pos = 1,
}

function parser:setinput(subj)
    self.subj = subj
    self.pos = 1
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
        return function(modes, _F, parser)
            load(parser:parse("restofline"))()(modes, _F, parser)
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

function modes:interpretprimitive(word)
    local f = self._F[word]
    if type(f) == "function" then
        f()(self, self._F, self.parser)
        return true
    else
        return false
    end
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

-------------------------------------------------------------------------------
-- Runtime Insert Word
-------------------------------------------------------------------------------
local defsubj = [=[
    %L return function(modes, _F, parser) _F["\n"] = function() return function(modes, _F, parser) end end end
    %L return function(modes, _F, parser) _F[""] = function() return function(modes, _F, parser) modes:setmode("stop") end end end
    %L return function(modes, _F, parser) _F["[L"] = function() return function(modes, _F, parser) load(parser:parsebypattern("^(.-)%sL]()"))()(modes, _F, parser) end end end
    [L
        return function(modes, _F, parser)
            _F["DUP"] = function()
                return function(modes, _F, parser)
                    local stack = modes.stack
                    local num = stack:pop()
                    stack:push(num)
                    stack:push(num)
                end
            end
            _F["*"] = function()
                return function(modes, _F, parser)
                    local stack = modes.stack
                    stack:push(stack:pop() * stack:pop())
                end
            end
            _F["."] = function()
                return function(modes, _F, parser)
                    io.write(" " .. modes.stack:pop())
                end
            end
        end
    L]
]=]

local function run(subj)
    parser:setinput(subj)
    modes:setmode("interpret")
    while modes:islive() do
        modes:runmode()
    end
    io.write("\n")
end

run(defsubj)
-- run("5 DUP * .")

return run
