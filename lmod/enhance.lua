-- Built-in Enhance Types

--[[
** {======================================================
** Basic Class Type
** =======================================================
--]]

---@class integer:number

---@class lightuserdata

---@alias MemAllocCallback fun(oldPtr:lightuserdata, newPtr:lightuserdata, newSize:integer):void

---@alias ErrorMessageHandler fun(msg:string):string | nil

-- }======================================================

--[[
** {======================================================
** global
** =======================================================
--]]

---@overload fun(value:any):string
---@overload fun(value:any, level:integer):string
---@param value any
---@param level integer
---@return string
function tostring(value, level) end

---@param value any
---@return string
function typedetail(value) end

---@param opt string | '"stop"' | '"restart"' | '"collect"' | '"count"' | '"step"' | '"setpause"' | '"setstepmul"' | '"isrunning"' | '"onestep"'
---@param arg integer
---@return number | boolean
function collectgarbage(opt, arg) end

---@param callback fun():void
function atexit(callback) end

---@param callback fun():void
function atrepl(callback) end

---@vararg any
function printerr(...) end

---@type lightuserdata
NULL = nil

---@type table
---@field argc integer
---@field argv lightuserdata
arg = nil

-- }======================================================

--[[
** {======================================================
** debug
** =======================================================
--]]

---@class SpecialKeys:table
---@field CLIBS lightuserdata
---@field HOOKKEY lightuserdata

---@return SpecialKeys
function debug.getspecialkeys() end

---@class SizeOfStruct:table
---@field global_State integer
---@field lua_State integer
---@field Table integer
---@field TKey integer
---@field TValue integer
---@field Node integer
---@field UTString integer
---@field UUdata integer
---@field LClosure integer
---@field CClosure integer
---@field Proto integer
---@field CallInfo integer
---@field luaL_Stream integer

---@return SizeOfStruct
function debug.sizeofstruct() end

---@param value table
---@return integer, integer, integer, boolean @totalByteSize, sizearray, lsizenode, isdummy
function debug.tablemem(value) end

---@return string
function debug.getgcstate() end

---@overload fun(func:function):string
---@overload fun(func:function, recursive:boolean):string
---@overload fun(func:function, recursive:boolean, options:string):string
---@param func function
---@param recursive boolean
---@param options string | '"hcklupz"' @header, codes, constants, locals, upvalues, protos, zykstyle
---@return string
function debug.protoinfo(func, recursive, options) end

---@alias UpValueNextSignature fun(func:function, idx:integer):integer, string, any

---@param func function
---@return UpValueNextSignature, function, integer @UpNext, func, 0
function debug.upvalues(func) end

---@alias LocalNextSignature fun(func:function, idx:integer):integer, string, any

---@overload fun(forl:function | integer):LocalNextSignature, any, integer
---@overload fun(t:thread, forl:function | integer):LocalNextSignature, any, integer
---@param t thread
---@param forl function | integer
---@return LocalNextSignature, any, integer @LocalNext, forl, 0
function debug.locals(t, forl) end

---@overload fun(src:string):any
---@overload fun(src:string, level:integer):any
---@param src string
---@param level integer
---@return any
function debug.inject(src, level) end

-- }======================================================

--[[
** {======================================================
** os
** =======================================================
--]]

---@return string
function os.sysname() end

-- }======================================================

--[[
** {======================================================
** utf8
** =======================================================
--]]

---@alias CharGetterSignature fun(idx:integer):integer

---@overload fun(func:CharGetterSignature):string
---@overload fun(...):string
---@param func CharGetterSignature
---@vararg integer
---@return string
function utf8.char(func, ...) end

-- }======================================================

--[[
** {======================================================
** string
** =======================================================
--]]

---@type string
local string

---@overload fun(func:CharGetterSignature):string
---@overload fun(...):string
---@param func CharGetterSignature
---@vararg integer
---@return string
function string.char(func, ...) end

---@alias ByteNextSignature fun():integer

---@overload fun(str:string):ByteNextSignature
---@overload fun(str:string, s:integer):ByteNextSignature
---@overload fun(str:string, s:integer, e:integer):ByteNextSignature
---@param str string
---@param s integer
---@param e integer
---@return ByteNextSignature
function string.bytes(str, s, e) end

---@param str string
---@return string
function string.escape(str) end

---@param str string
---@return boolean
function string.isvar(str) end

-- }======================================================

--[[
** {======================================================
** io
** =======================================================
--]]

---@type file
local file

---@return integer
function file:fileno() end

---@param fileName string
---@return string | nil, nil | string, nil | integer @data, errStr, errCode
function io.readfile(fileName) end

---@param fileName string
---@param data string
---@return boolean | nil, nil | string, nil | integer @data, errStr, errCode
function io.writefile(fileName, data) end

-- }======================================================

--[[
** {======================================================
** table
** =======================================================
--]]

---@overload fun():table
---@overload fun(narr:integer):table
---@overload fun(narr:integer, nrec:integer):table
---@param narr integer
---@param nrec integer
---@return table
function table.create(narr, nrec) end

-- }======================================================

--[[
** {======================================================
** util
** =======================================================
--]]

---@param callback ErrorMessageHandler
function util.setErrorMessageHandler(callback) end

---@class luaL_MemBuffer:userdata
---@field public getClear fun(self:luaL_MemBuffer):lightuserdata, integer, lightuserdata, lightuserdata
---@field public setReplace fun(self:luaL_MemBuffer, ptr:lightuserdata, sz:integer, release:lightuserdata, ud:lightuserdata):void
---@field public release fun(self:luaL_MemBuffer):void

---@overload fun():luaL_MemBuffer
---@overload fun(ptr:lightuserdata, sz:integer):luaL_MemBuffer
---@overload fun(ptr:lightuserdata, sz:integer, release:lightuserdata):luaL_MemBuffer
---@overload fun(ptr:lightuserdata, sz:integer, release:lightuserdata, ud:lightuserdata):luaL_MemBuffer
---@param ptr lightuserdata
---@param sz integer
---@param release lightuserdata
---@param ud lightuserdata
---@return luaL_MemBuffer
function util.luaL_MemBuffer(ptr, sz, release, ud) end

function util.printBuffer() end

---@return string
function util.printFinish() end

-- }======================================================
