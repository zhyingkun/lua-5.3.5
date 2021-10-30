#! /usr/bin/env lua

local file = arg[1]
local fd = file and io.open(file, "rb") or io.stdin
local msg = fd:read("a")
fd:close()
fd = nil

local fieldTbl = require("protobuf.varint").ParseVarintRaw(msg)
print(tostring(fieldTbl, 16))
