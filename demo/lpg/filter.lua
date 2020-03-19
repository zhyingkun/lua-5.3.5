local filter = {}
function filter.cycle(lowlevel, context, extra)
    return function(chunk)
        local ret
        ret, context = lowlevel(context, chunk, extra)
        return ret
    end
end

function normalize(marker)
    return filter.cycle(require("libluasocket.mime").eol, 0, marker)
end

-- function filter.chain(...)
--     local arg = {...}
--     local f = arg[1]
--     local function chainpair(f1, f2)
--         return function(chunk)
--             local ret = f2(f1(chunk))
--             if chunk then
--                 return ret
--             else
--                 return ret .. f2()
--             end
--         end
--     end    
--     for i = 2, #arg do
--         f = chainpair(f, arg[i])
--     end
--     return f
-- end

local source = {}

function source.empty(err)
    return function()
        return nil, err
    end
end

function source.file(handle, io_err)
    if handle then
        return function()
            local chunk = handle:read(2048)
            if not chunk then handle:close() end
            return chunk
        end
    else
        return source.empty(io_err or "unable to open file")
    end
end

function source.chain(src, f)
    return function()
        if not src then return nil end
        local chunk, err = src()
        if not chunk then
            src = nil
            return f(nil)
        else
            return f(chunk)
        end
    end
end

local sink = {}

function sink.table(t)
    t = t or {}
    local function f(chunk, err)
        if chunk then table.insert(t, chunk) end
        return 1
    end
    return f, t
end

local function null() return 1 end

function sink.null() return null end

function sink.file(fd)
    return function(chunk, err)
        if chunk then
            local ret
            ret, err = fd:write(chunk)
            if ret then
                return 1
            end
        end
        -- print(err)
        return nil, err
    end
end

local pump = {}

function pump.step(src, snk)
    local chunk, src_err = src()
    local ret, snk_err = snk(chunk, src_err)
    if chunk and ret then
        return 1
    else
        return nil, src_err or snk_err
    end
end

function pump.all(src, snk, step)
    step = step or pump.step
    while true do
        local ret, err = step(src, snk)
        if not ret then
            if err then
                return nil, err
            else
                return 1
            end
        end
    end
end


local CRLF = "\013\010"
local input = source.chain(source.file(io.stdin), normalize(CRLF))
local output = sink.file(io.open(arg[1], "w"))
pump.all(input, output)

-- local qp = filter.chain(normalize(CRLF), encode("quoted-printable"), wrap("quote-printable"))
-- local input = source.chain(source.file(io.stdin), qp)
-- local output = sink.file(io.stdout)
-- pump.all(input, output)

-- local input = source.file(io.stdin)
-- local output, t = sink.table()
-- output = sink.chain(normalize(CRLF), output)
-- pump.all(input, output)

-- for chunk in source.file(io.stdin) do io.write(chunk) end

-- local input = source.chain(source.file(io.open("input.bin", "rb")), encode("base64"))
-- local output = sink.chain(wrap(76), sink.file(io.open("output.b64", "w")))
-- pump.all(intput, output)

-- local smtp  = require("socket.smtp")
-- local mime = require("mime")
-- local ltn12 = require("ltn12") --> Lua Technical Note 12
-- local message = smtp.message({
--     headers = {
--         from = "xxx@xxx.com",
--         to = "xxx@xxx.com",
--         subject = "A message with an attachment",
--     },
--     body = {
--         preamble = "Hope you can see the attachment" .. CRLF,
--         [1] = {
--             body = "Here is our logo" .. CRLF,
--         },
--         [2] = {
--             headers = {
--                 ["content-type"] = [=[image/png; name="luasocket.png"]=],
--                 ["content-disposition"] = [=[attachment; filename="luasocket.png"]=],
--                 ["content-description"] = "LuaSocket logo",
--                 ["content-transfer-encoding"] = "BASE64",
--             },
--             body = ltn12.source.chain(ltn12.source.file(io.open("luasocket.png", "rb")), ltn12.filter.chain(mime.encode("base64"), mime.wrap()))),
--         }
--     }
-- })
-- assert(smtp.send({
--     rcpt = "xxx@xxx.com",
--     from = "xxx@xxx.com",
--     source = message,
-- }))
