hello = require("libhello")
lfuncyield = function(val)
    print("lfuncyield val:", val)
    local fromresume = coroutine.yield("lfuncyield yield")
    print("lfuncyield fromresume:", fromresume)
    -- error("Call error in lfuncyield")
end
world = {}
world.hi = "Hello World!"
world.singSong = function(who)
    print("You can count on me like 1 2 3 ---" .. who)
    return 100
end
lfunc = function(val)
    print("lfunc val:", val)
end
cor =
    coroutine.create(
    function(val)
        print("Start coroutine...")
        print("from first resume val:", val)
        print("Call cfuncyield...")
        hello.cfuncyield()
        -- lfuncyield(14)
        print("Call cfuncyield end")
        print("Call cfunc...")
        hello.cfunc()
        print("Call cfunc end")
        print("End coroutine...")
    end
)

print("main thread Start...")
-- world.singSong()
hello.cfunc()
flag, fromyield = coroutine.resume(cor, 17)
print("main thread fromyield:", flag, fromyield)
print(coroutine.resume(cor, 18))

print("Call foo, return values: ", hello.foo(1, 2, 3, 4))
print("main thread End")
