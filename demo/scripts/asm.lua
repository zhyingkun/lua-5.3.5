local f = asm.closure(0, false, function(pm)
	local R, U, K, P, RK_R, RK_K = pm.R, pm.U, pm.K, pm.P, pm.RK_R, pm.RK_K
	pm:subproto(0, false, function(pm)
		pm:constant("print")

		pm:upvalue(false, 0)
		pm:upvalue(true, 0)

		pm:GETTABUP(R(0), U(0), RK_K(0))
		pm:LOADK   (R(1), K(0))
		pm:GETUPVAL(R(2), U(1))
		pm:CALL    (R(0), 3, 1)
		pm:RETURN  (R(0), 1)
	end)

	pm:constant("print")
	pm:constant("HelloWorld")
	pm:constant(43)

	pm:GETTABUP(R(0), U(0), RK_K(0))
	pm:LOADK   (R(1), K(1))
	pm:LOADK   (R(2), K(2))
	pm:CALL    (R(0), 3, 1)
	pm:LOADK   (R(0), K(1))
	pm:CLOSURE (R(1), P(0))
	pm:CALL    (R(1), 1, 1)
	pm:CLOSURE (R(1), P(0))
	pm:RETURN  (R(1), 2)
end)

local g = f()
g()
f= nil

collectgarbage()
collectgarbage()
