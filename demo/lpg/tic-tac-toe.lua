local EMPTY, CROSS, CIRCLE = 0, 1, 2
local names = { [0] = "EMPTY", "CROSS", "CIRCLE"}
local hMax = 10
local currentPlayer = nil

local function ScoreForPlayer(player)
	if player == currentPlayer then
		return 1
	end
	return -1
end

local function GameOver(gameBoard)
	local full = true
	for k, line in ipairs(gameBoard) do
		local a, b, c = line[1], line[2], line[3]
		if a ~= EMPTY and a == b and a == c then
			gameBoard.score = ScoreForPlayer(a)
			return true
		end
		if a == EMPTY or b == EMPTY or c == EMPTY then
			full = false
		end
	end
	for i = 1, 3, 1 do
		local a, b, c = gameBoard[1][i], gameBoard[2][i], gameBoard[3][i]
		if a ~= EMPTY and a == b and a == c then
			gameBoard.score = ScoreForPlayer(a)
			return true
		end
	end
	do
		local a, b, c = gameBoard[1][1], gameBoard[2][2], gameBoard[3][3]
		if a ~= EMPTY and a == b and a == c then
			gameBoard.score = ScoreForPlayer(a)
			return true
		end
	end
	do
		local a, b, c = gameBoard[1][3], gameBoard[2][2], gameBoard[3][1]
		if a ~= EMPTY and a == b and a == c then
			gameBoard.score = ScoreForPlayer(a)
			return true
		end
	end
	if full then
		gameBoard.score = 0
		return true
	end
	return false
end

local function Utility(gameBoard)
	return gameBoard.score or 0
end

local function GetLegalMoves(gameBoard, player)
	local insert = table.insert
	local legals = {}
	for i = 1, 3, 1 do
		for j = 1, 3, 1 do
			if gameBoard[i][j] == EMPTY then
				insert(legals, {i, j, player})
			end
		end
	end
	assert(#legals ~= 0)
	return legals
end

local function Move(gameBoard, step)
	local newBoard = {}
	for i = 1, 3, 1 do
		local newLine = {}
		local line = gameBoard[i]
		for j = 1, 3, 1 do
			newLine[j] = line[j]
		end
		newBoard[i] = newLine
	end
	assert(newBoard[step[1]][step[2]] == EMPTY)
	newBoard[step[1]][step[2]] = step[3]
	return newBoard
end

local function AnotherPlayer(player)
	return player == CIRCLE and CROSS or CIRCLE
end

local function MinimaxValue(gameBoard, h, player)
	if GameOver(gameBoard) or h == hMax then
		return Utility(gameBoard)
	end
	local legalMoves = GetLegalMoves(gameBoard, player)
	local maxValue = -999
	local minValue = 999
	local anotherPlayer = AnotherPlayer(player)
	local nextH = h + 1
	if player == currentPlayer then
		for k, v in pairs(legalMoves) do
			local newBoard = Move(gameBoard, v)
			local value = MinimaxValue(newBoard, nextH, anotherPlayer)
			if value > maxValue then
				maxValue = value
			end
		end
		return maxValue
	else
		for k, v in pairs(legalMoves) do
			local newBoard = Move(gameBoard, v)
			local value = MinimaxValue(newBoard, nextH, anotherPlayer)
			if value < minValue then
				minValue = value
			end
		end
		return minValue
	end
end

local function MinimaxDecision(gameBoard, player)
	currentPlayer = player
	local bestMove
	local maxValue = -999
	local legalMoves = GetLegalMoves(gameBoard, player)
	for k, v in ipairs(legalMoves) do
		local newBoard = Move(gameBoard, v)
		local value = MinimaxValue(newBoard, 1, AnotherPlayer(player))
		if value > maxValue then
			maxValue = value
			bestMove = v
		end
	end
	return bestMove
end

-- turn: CIRCLE

-- local gameBoard = {
-- 	{ EMPTY, CIRCLE, CROSS },
-- 	{ EMPTY, CIRCLE, EMPTY },
-- 	{ CROSS, EMPTY, CROSS },
-- }

-- local gameBoard = {
-- 	{ EMPTY, CIRCLE, EMPTY },
-- 	{ CROSS, EMPTY, EMPTY },
-- 	{ CROSS, EMPTY, EMPTY },
-- }

local gameBoard = {
	{EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY},
}

-- local gameBoard = {
-- 	{EMPTY, CIRCLE, CROSS},
-- 	{EMPTY, CROSS, EMPTY},
-- 	{EMPTY, EMPTY, CIRCLE},
-- }

local p = CIRCLE
while not GameOver(gameBoard) do
	local bestMove = MinimaxDecision(gameBoard, p)
	print(bestMove[1], bestMove[2], names[bestMove[3]])
	gameBoard = Move(gameBoard, bestMove)
	p = AnotherPlayer(p)
end
