print("======================================================================")
do
	local forth = require("miniforth")
	local src = [=[
		: CUBE DUP DUP * * ;
		: TUCK SWAP OVER ;
		: NIP SWAP DROP ;
		: 2DROP DROP DROP ;
	]=]
	forth:exec(src)

	print("====")
	local test = [=[
		5 DUP * .
		6 CUBE .
		1 2 3 4 .S
		. . . .
	]=]
	forth:run(test)
end
