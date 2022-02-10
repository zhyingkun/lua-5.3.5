#! /usr/bin/env lua

local fileName = arg[1]
if not fileName then
	print("Pass bin file path as a parameter")
	return
end

local bin = io.readfile(fileName)

local codes = {}
local insert = table.insert
for byte in bin:bytes() do
	insert(codes, string.format("0x%X", byte))
end

print("local data = { ", table.concat(codes, ", "), "}")
