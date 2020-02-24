subj = "5 DUP * ."
pos = 1

function parsebypattern(pat)
    local capture, newpos = string.match(subj, pat, pos)
    if newpos then
        pos = newpos
        return capture
    end
end

what2pattern = {
    spaces = "^([ \t]*)()",
    word = "^([^ \t\n]+)()",
    newline = "^(\n)()",
    restofline = "^([^\n]*)()"
}

function parse(what)
    return parsebypattern(what2pattern[what])
end

function getword()
    parse("spaces")
    return parse("word")
end

function getwordornewline()
    return getword() or parse("newline")
end

_F = {}
function eval(str) load(str)() end
_F["%L"] = function() eval(parse("restofline")) end

mode = "interpret"
modes = {}

function run(subjj)
    subj = subjj
    pos = 1
    mode = "interpret"
    while mode ~= "stop" do
        modes[mode]()
    end
    io.write("\n")
end

word = ""

function interpretprimitive()
    local f = _F[word]
    if type(f) == "function" then
        f()
        return true
    end
end

function interpretnonprimitive()
    return false
end

function interpretnumber()
    local num = tonumber(word)
    if num then
        _F[word] = function() push(DS, num) end
        interpretprimitive()
        return true
    end
end

function printstate()
end

function modes.interpret()
    word = getwordornewline() or ""
    printstate()
    local _ = interpretprimitive() or interpretnonprimitive() or interpretnumber() or error("Can't interpret: " .. word)
end

mysubj1 = [=[
    %L _F["\n"] = function() end
    %L _F[""] = function() mode = "stop" end
    %L _F["[L"] = function() eval(parsebypattern("^(.-)%sL]()")) end
    [L
        DS = { n = 0 }
        push = function(stack, x)
            local n = stack.n
            n = n + 1
            stack.n = n
            stack[n] = x
        end
        pop = function(stack)
            local n = stack.n
            local x = stack[n]
            stack[n] = nil
            n = n - 1
            stack.n = n
            return x
        end
        _F["5"] = function() push(DS, 5) end
        _F["DUP"] = function() push(DS, DS[DS.n]) end
        _F["*"] = function() push(DS, pop(DS) * pop(DS)) end
        _F["."] = function() io.write(" " .. pop(DS)) end
    L]
]=]

run(mysubj1)
run("5 DUP * .")
