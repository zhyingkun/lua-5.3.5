local c = require "libprotobuf"
local P = c._env_new()

--[[ Encode by wmessage start ]]----------------
local _writer
-- MessageName => { FieldName => encoder: function(wmessage, FieldName, value) end }
local WMessageFuncsCache = setmetatable({} , {
	__index = function(cache, msgName)
		local wmsgFuncs = setmetatable({ _MsgName = msgName }, {
			__index = function(self, fieldName)
				local t, msg = c._env_type(P, self._MsgName, fieldName)
				local func = assert(_writer[t], fieldName)(msg)
				self[fieldName] = func
				return func
			end
		})
		-- cache: MessageName => WMessageFunctions
		cache[msgName] = wmsgFuncs
		return wmsgFuncs -- FieldName => wmessage function
	end
})
local function encode_message(wmsg, msgName, tbl)
	local wmsgFuncs = WMessageFuncsCache[msgName] -- FieldName => wmessage function
	for fieldName, value in pairs(tbl) do
		local func = wmsgFuncs[fieldName]
		func(wmsg, fieldName, value)
	end
end
_writer = {
	real = c._wmessage_real,
	enum = c._wmessage_string,
	string = c._wmessage_string,
	int = c._wmessage_int,
}
function _writer:bool(k,v)
	c._wmessage_int(self, k, v and 1 or 0)
end
function _writer:message(k, v , message_type)
	local submessage = c._wmessage_message(self, k)
	encode_message(submessage, message_type, v)
end
function _writer:real_repeated(k,v)
	for _,v in ipairs(v) do
		c._wmessage_real(self,k,v)
	end
end
function _writer:bool_repeated(k,v)
	for _,v in ipairs(v) do
		c._wmessage_int(self, k, v and 1 or 0)
	end
end
function _writer:string_repeated(k,v)
	for _,v in ipairs(v) do
		c._wmessage_string(self,k,v)
	end
end
function _writer:message_repeated(k,v, message_type)
	for _,v in ipairs(v) do
		local submessage = c._wmessage_message(self, k)
		encode_message(submessage, message_type, v)
	end
end
function _writer:int_repeated(k,v)
	for _,v in ipairs(v) do
		c._wmessage_int(self,k,v)
	end
end
_writer[1] = function(msg) return _writer.int end
_writer[2] = function(msg) return _writer.real end
_writer[3] = function(msg) return _writer.bool end
_writer[4] = function(msg) return _writer.string end
_writer[5] = function(msg) return _writer.string end
_writer[6] = function(msg)
	local message = _writer.message
	return function(self,key , v)
		return message(self, key, v, msg)
	end
end
_writer[7] = _writer[1]
_writer[8] = _writer[1]
_writer[9] = _writer[5]
_writer[10] = _writer[7]
_writer[11] = _writer[7]
_writer[128+1] = function(msg) return _writer.int_repeated end
_writer[128+2] = function(msg) return _writer.real_repeated end
_writer[128+3] = function(msg) return _writer.bool_repeated end
_writer[128+4] = function(msg) return _writer.string_repeated end
_writer[128+5] = function(msg) return _writer.string_repeated end
_writer[128+6] = function(msg)
	local message = _writer.message_repeated
	return function(self,key, v)
		return message(self, key, v, msg)
	end
end
_writer[128+7] = _writer[128+1]
_writer[128+8] = _writer[128+1]
_writer[128+9] = _writer[128+5]
_writer[128+10] = _writer[128+7]
_writer[128+11] = _writer[128+7]
--[[ Encode by wmessage end ]]----------------

local M = {}

function M.register(buffer)
	c._env_register(P, buffer)
end

function M.register_file(filename)
	local f = assert(io.open(filename , "rb"))
	local buffer = f:read("a")
	f:close()
	c._env_register(P, buffer)
end

function M.lasterror()
	return c._last_error(P)
end

function M.check(typename, field)
	return c._env_type(P, typename, field) ~= 0
end

function M.enum_id(enum_type, enum_name)
	return c._env_enum_id(P, enum_type, enum_name)
end

function M.encode(message, tbl)
	local wmsg = c._wmessage_new(P, message)
	assert(wmsg, message)
	encode_message(wmsg, message, tbl)
	local s = c._wmessage_buffer_string(wmsg)
	c._wmessage_delete(wmsg)
	return s
end

function M.decode(typename, buffer)
	return c.decode(P, typename, buffer)
end

function M.decode_pure(typename, buffer)
	return c.decode_pure(P, typename, buffer)
end

-- func: function(old_ptr, new_ptr, new_size) end
function M.memcb(func)
	c.set_realloc_cb(func)
end

return M
