local libgb2312 = require("libgb2312")

---@class gb2312
local gb2312 = {}

---@overload fun(str:string, nth:integer):integer | nil
---@overload fun(str:string, nth:integer, i:integer):integer | nil
---@param str string
---@param nth integer
---@param i integer
---@return integer | nil
function gb2312.offset(str, nth, i)
	return libgb2312.offset(str, nth, i)
end
---@overload fun(str:string):integer, ...
---@overload fun(str:string, i:integer):integer, ...
---@overload fun(str:string, i:integer, e:integer):integer, ...
---@param str string
---@param i integer
---@param e integer
---@return integer, ...
function gb2312.codepoint(str, i, e)
	return libgb2312.codepoint()
end

---@alias CodepointGetterSignature fun():integer

---@overload fun(getter:CodepointGetterSignature):string
---@overload fun(...):string
---@param getter CodepointGetterSignature
---@vararg integer
---@return string
function gb2312.char(getter, ...)
	return libgb2312.char()
end
---@overload fun(str:string):integer
---@overload fun(str:string, i:integer):integer
---@overload fun(str:string, i:integer, j:integer):integer
---@param str string
---@param i integer
---@param j integer
---@return integer
function gb2312.len(str, i, j)
	return libgb2312.len(str, i, j)
end

---@alias CodepointIterSignature fun(str:string, idx:ingeter):nil | (integer, integer)

---@param str string
---@return CodepointIterSignature, string, integer
function gb2312.codes(str)
	return libgb2312.codes(str)
end

---@type string
gb2312.charpattern = libgb2312.charpattern

return gb2312
