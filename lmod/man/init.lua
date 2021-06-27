local function WalkFiles(directory, suffix, escape, callback)
	local function ShouldEscape(file)
		for _, name in ipairs(escape) do
			if name == file then return true end
		end
		return false
	end
	for name, isdir in require("libdir").dirs(directory) do
		if not isdir and name:find(suffix .. "$") and not ShouldEscape(name) then
			callback(name)
		end
	end
end

local ModName, AbsPath = ...
local Default = ".init"
if ModName:sub(-(#Default), -1) == Default then
	ModName = ModName:sub(1, -(#Default) - 1)
end
local AbsDir = require("libdir").dirname(AbsPath)

local ManualList = {}
local Suffix = ".lua"
WalkFiles(AbsDir, Suffix, {
	"init.lua"
}, function(filename)
	filename = filename:sub(1, -(#Suffix) - 1)
	ManualList[filename] = require(ModName .. "." .. filename)
end)

return ManualList
