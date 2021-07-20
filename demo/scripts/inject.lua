local uv = 2

function f(...)
	local a,b = 1,uv
	print("_ENV ===>", _ENV)
	print("a,b ====>", a,b)
	debug.inject([[
		print "=== inject code ==="
		print("\t_ENV ===>", _ENV)
		print("\ta,b,uv ===>", a,b,uv)
		print("\t... ", ...)
		a,b = b,a
		uv = 3
		print "==== inject end ==="
	]])
	print("a,b ====>", a,b)
end

f("HelloWorld")

print("uv=",uv)
