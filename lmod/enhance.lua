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
---@field public argc integer
---@field public argv lightuserdata
arg = nil

-- }======================================================

--[[
** {======================================================
** debug
** =======================================================
--]]

---@class SpecialKeys:table
---@field public CLIBS lightuserdata
---@field public HOOKKEY lightuserdata

---@return SpecialKeys
function debug.getspecialkeys() end

---@class SizeOfStruct:table
---@field public global_State integer
---@field public lua_State integer
---@field public Table integer
---@field public TKey integer
---@field public TValue integer
---@field public Node integer
---@field public UTString integer
---@field public UUdata integer
---@field public LClosure integer
---@field public CClosure integer
---@field public Proto integer
---@field public CallInfo integer
---@field public luaL_Stream integer

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

---@overload fun(tbl:table):table
---@overload fun(tbl:table, copykv:boolean):table
---@param tbl table
---@param copykv boolean @default is true
---@return table
function table.copy(tbl, copykv) end

---@overload fun(tbl:table, callback:(fun(value:any, key:any):boolean)):boolean
---@overload fun(tbl:table, callback:(fun(value:any, key:any):boolean), bList:boolean):boolean
---@param tbl table
---@param callback fun(value:any, key:any):boolean
---@param bList boolean @default is true
---@return boolean
function table.every(tbl, callback, bList) end

---@overload fun(tbl:table, callback:(fun(value:any, key:any):boolean)):table
---@overload fun(tbl:table, callback:(fun(value:any, key:any):boolean), bList:boolean):table
---@param tbl table
---@param callback fun(value:any, key:any):boolean
---@param bList boolean @default is true
---@return table
function table.filter(tbl, callback, bList) end

---@overload fun(tbl:table, value:any):any
---@overload fun(tbl:table, value:any, bList:boolean):any
---@param tbl table
---@param value any
---@param bList boolean @default is true
---@return any @the key
function table.indexOf(tbl, value, bList) end

---@param tbl table
---@param callback fun(value:any, key:any):any
---@return table
function table.map(tbl, callback) end

---@alias ReduceCallback fun(acc:any, value:any, key:any, list:table):any
---@overload fun(tbl:table, callback:ReduceCallback):any
---@overload fun(tbl:table, callback:ReduceCallback, init:any):any
---@param tbl table<integer, any>
---@param callback ReduceCallback
---@param init any
---@return any
function table.reduce(tbl, callback, init) end

---@overload fun(tbl:table):table
---@overload fun(tbl:table, first:integer):table
---@overload fun(tbl:table, first:integer, last:integer):table
---@overload fun(tbl:table, first:integer, last:integer, dst:table):table
---@param tbl table
---@param first integer
---@param last integer
---@param dst table
---@return table
function table.reverse(tbl, first, last, dst) end

---@overload fun(tbl:table):table
---@overload fun(tbl:table, first:integer):table
---@overload fun(tbl:table, first:integer, last:integer):table
---@param tbl table
---@param first integer
---@param last integer
---@return table
function table.slice(tbl, first, last) end

---@overload fun(tbl:table, first:integer):boolean
---@overload fun(tbl:table, first:integer, count:integer):boolean
---@overload fun(tbl:table, first:integer, count:integer, ...:any):boolean
---@param tbl table
---@param first integer
---@param count integer
---@vararg any
---@return boolean
function table.splice(tbl, first, count, ...) end

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
