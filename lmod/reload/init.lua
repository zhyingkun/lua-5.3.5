--[[
For modules or for values in Global table, We have three concept:
1. The real one => _LOADED
2. The dummy one => _LOADED_DUMMY (Associate to any type of real value)
In sandbox.require, we find/compile/run(loader), and we get the reload one.
During the require, the reload one maybe request for GlobalField or OtherModule,
The sandbox will give it a dummy one of that.
--]]

local sandbox = (function()
	local dummy_module_cache -- cache all dummy module
	local dummy_global_cache -- dummies for global field
	local _LOADED_DUMMY

	local _init = (function()
		local weak = { __mode = "kv" }
		return function ()
			dummy_global_cache = setmetatable({}, weak)
			dummy_module_cache = setmetatable({}, weak)
			_LOADED_DUMMY = {}
		end
	end)()
	local function _clear()
		dummy_global_cache = nil
		dummy_module_cache = nil
		_LOADED_DUMMY = nil
	end

	local get_dummy_module = (function()
		local dummy_module_mt
		local function get_or_create_obj(key)
			local obj = dummy_module_cache[key]
			if not obj then
				obj = setmetatable({}, dummy_module_mt)
				dummy_module_cache[key] = obj
				dummy_module_cache[obj] = key
			end
			return obj
		end
		dummy_module_mt = {
			__metatable = "MODULE",
			__newindex = error,
			__pairs = error,
			__index = function(self, k)
				assert(string.isvar(k), "module field is not a valid variable name")
				local parent_key = dummy_module_cache[self]
				local key = parent_key .. "." .. k
				return get_or_create_obj(key)
			end
		}
		return function(name) -- maybe 'name' contains '.'
			local key = "[" .. name .. "]"
			return get_or_create_obj(key)
		end
	end)()
	local get_dummy_global = (function()
		local dummy_global_mt
		local function get_or_create_obj(key)
			local obj = dummy_global_cache[key]
			if not obj then
				obj = setmetatable({}, dummy_global_mt)
				dummy_global_cache[key] = obj
				dummy_global_cache[obj] = key
			end
			return obj
		end
		dummy_global_mt = {
			__metatable = "GLOBAL",
			__newindex = error,
			__pairs = error,
			__index = function(self, k)
				assert(string.isvar(k), "global field must be a valid variable name")
				local parent_key = dummy_global_cache[self]
				local key = parent_key .. "." .. k
				return get_or_create_obj(key)
			end,
		}
		return function(name)
			assert(string.isvar(name), "global name must be a valid variable name")
			return get_or_create_obj(name)
		end
	end)()

	local function internal_require(mod_name)
		local dummy = _LOADED_DUMMY[mod_name]
		if not dummy then
			assert(require(mod_name), "reload module require another module failed: " .. mod_name)
			dummy = get_dummy_module(mod_name)
			_LOADED_DUMMY[mod_name] = dummy
		end
		return dummy
	end

	-- the base lib function never return objects out of sandbox
	local safe_function = {
		require = internal_require, -- sandbox require
		pairs = pairs, -- allow pairs during require
		ipairs = ipairs,
		next = next,
		print = print, -- for debug
		printe = printe, -- for debug
	}

	local _require = (function()
		local make_sandbox = (function()
			local global_mt = {
				__newindex = error,
				__pairs = error,
				__metatable = "SANDBOX",
				__index = function(self, k)
					assert(type(k) == "string", "Global name must be a string")
					local v = safe_function[k]
					if v then return v end
					return get_dummy_global(k)
				end
			}
			return function()
				return setmetatable({}, global_mt)
			end
		end)()
		local function find_loader(mod_name)
			local msg = {}
			for _, searcher in ipairs(package.searchers) do
				local loader, absPath = searcher(mod_name)
				local t = type(loader)
				if t == "function" then
					return loader, absPath
				elseif t == "string" then
					table.insert(msg, loader)
				end
			end
			error(string.format("module '%s' not found:%s", mod_name, table.concat(msg)))
		end
		return function(mod_name) -- load with sandbox
			assert(type(mod_name) == "string")
			local loader, absPath = find_loader(mod_name)
			local env, uv = debug.getupvalue(loader, 1)
			if env == "_ENV" then
				-- this '_ENV' upvalue will be shared with functions in the module
				debug.setupvalue(loader, 1, make_sandbox()) -- every loaded module has a unique sandbox
			end
			local mod_ref_dummy = loader(mod_name, absPath)
			assert(mod_ref_dummy, "reload module must return something")
			if env == "_ENV" then
				debug.setupvalue(loader, 1, nil) -- must set to nil, prevent for enum object
			end
			return mod_ref_dummy, loader
		end
	end)()


	local _value = (function()
		local function get_G(dummy) -- will get the global value for that dummy object (maybe any value type)
			local k = dummy_global_cache[dummy] -- the key: a.b.c
			local G = _G
			-- %a means all alphabet, contains upper and lower
			-- %w means all alphabet and digital
			for w in string.gmatch(k, "[_%a][_%w]*") do
				if G == nil then
					-- error(string.format("Invalid global: %s", k))
					return nil
				end
				G = G[w]
			end
			return G, k
		end
		local function get_M(dummy) -- get the module for that dummy object (maybe any value type)
			local k = dummy_module_cache[dummy]
			local M = debug.getregistry()._LOADED
			local from, to, name = string.find(k, "^%[([_%a][_.%w]*)%]")
			if not from then
				-- error(string.format("Invalid module: %s", k))
				return nil
			end
			local mod = assert(M[name])
			for w in string.gmatch(k:sub(to+1), "[_%a][_%w]*") do
				if mod == nil then
					-- error(string.format("Invalid module key: %s", k))
					return nil
				end
				mod = mod[w]
			end
			return mod, k
		end
		return function(dummy)
			local meta = getmetatable(dummy)
			if meta == "GLOBAL" then
				return get_G(dummy)
			elseif meta == "MODULE" then
				return get_M(dummy)
			else
				error(string.format("Invalid dummy object: %s, %s", meta, tostring(dummy, 1)))
			end
		end
	end)()

	local function _is_dummy(v)
		if safe_function[v] then
			return true
		end
		return getmetatable(v) ~= nil
	end

	return {
		require = _require,
		value = _value,
		is_dummy = _is_dummy,
		init = _init,
		clear = _clear,
	}
end)()

local enum_tbl_func_dummy = (function()
	local dummy_path_list
	local obj_path_list
	local path
	local has_traversed

	local is_dummy = sandbox.is_dummy
	local insert = table.insert

	local iterate_object
	local function iterate_function(func, depth)
		insert(obj_path_list, {func, table.unpack(path)})
		for idx, name, value in debug.upvalues(func) do
			if name == "" then break end -- c closure
			if not name:find("^[_%a]") then
				error("Invalid upvalue: " .. table.concat(path, "."))
			end
			local t = type(value) -- type of value
			if t == "function" or t == "table" then
				path[depth] = name
				iterate_object(value, t, depth)
				path[depth] = nil
			end
		end
	end
	local accept_key_type = {
		number = true,
		string = true,
		boolean = true,
	}
	local function iterate_table(tbl, depth)
		if is_dummy(tbl) then
			insert(dummy_path_list, {tbl, table.unpack(path)})
		else
			insert(obj_path_list, {tbl, table.unpack(path)})
			for key, value in pairs(tbl) do
				if not accept_key_type[type(key)] then
					error(string.format("Invalid key: %s %s", key, table.concat(path, ".")))
				end
				local t = type(value) -- type of value
				if t == "function" or t == "table" then
					path[depth] = key
					iterate_object(value, t, depth)
					path[depth] = nil
				end
			end	
		end
	end
	function iterate_object(obj, t, depth)
		if has_traversed[obj] then return end
		has_traversed[obj] = true
		depth = depth + 1
		if t == "function" then
			return iterate_function(obj, depth)
		else -- table
			return iterate_table(obj, depth)
		end
	end
	return function(object)
		local t = type(object)
		if t ~= "function" and t ~= "table" then return end
		dummy_path_list = {}
		obj_path_list = {}
		path = {}
		has_traversed = {}
		iterate_object(object, t, 0)
		local dummy, obj = dummy_path_list, obj_path_list
		dummy_path_list = nil
		obj_path_list = nil
		path = nil
		has_traversed = nil
		return dummy, obj
	end
end)()

local function find_upvalue(func, upvalue_name)
	for idx, name, value in debug.upvalues(func) do
		if name == "" then return end
		if name == upvalue_name then
			return value, idx
		end
	end
end
local function find_oldTF(old_real_mod, TF_path)
	-- TF_path[1]: the Table/Function from the new mod_ref_dummy, but not dummy
	-- TF_path[2 to len]: the path in the new mod_ref_dummy (every node of the path must be table/function)
	-- find_oldTF will find the old table/function according to the path
	local node = old_real_mod
	local inext = ipairs(TF_path)
	for idx, key in inext, TF_path, 1 do
		local t = type(node)
		if t == "table" then
			node = node[key]
		elseif t == "function" then
			node = find_upvalue(node, key)
		else
			assert(t == "nil")
			return
		end
	end
	return node
end
local function type_error(tbl, msg)
	local obj = tbl[1]
	tbl[1] = msg
	msg = table.concat(tbl, ".")
	tbl[1] = obj
	error(msg)
end
local function match_objects(TF_path_list, old_real_mod)
	-- find match table/func between old module and new one
	local newTbl2oldTbl = {}
	local newFunc2oldFunc = {}
	-- local additionTF = {}
	local insert = table.insert
	for _, TF_path in ipairs(TF_path_list) do
		local newTF = TF_path[1] -- table/function from 'the reload one'
		local ok, old_one = pcall(find_oldTF, old_real_mod, TF_path)
		if not ok then
			type_error(TF_path, "type mismatch: ")
		end
		if old_one == nil then
			-- insert(additionTF, TF_path) -- not found in old_real_mod, will be patch in table merge
		else
			local t = type(newTF)
			assert(type(old_one) == t, "reload table/function must has the same type")
			local newTF2oldTF = t == "table" and newTbl2oldTbl or newFunc2oldFunc
			local oldTF = newTF2oldTF[newTF]
			if oldTF then
				assert(oldTF == old_one, "two old table/function must map to new different two")
			else
				newTF2oldTF[newTF] = old_one
			end
		end
	end
	return newTbl2oldTbl, newFunc2oldFunc -- , additionTF
end

local function match_upvalues(newFunc2oldFunc)
	-- find match func's upvalues
	local newID2oldUV = {}
	for newFunc, oldFunc in pairs(newFunc2oldFunc) do
		for idx, name, value in debug.upvalues(newFunc) do
			if name == "" then break end
			local _, old_idx = find_upvalue(oldFunc, name)
			if old_idx then
				local id = debug.upvalueid(newFunc, idx)
				local old_id = debug.upvalueid(oldFunc, old_idx)
				local oldUV = newID2oldUV[id]
				if not oldUV then
					newID2oldUV[id] = {
						old_func = oldFunc,
						old_idx = old_idx,
						old_id = old_id
					}
				else
					assert(old_id == oldUV.old_id, string.format("Ambiguity upvalue: %s .%s", tostring(newFunc), name))
				end
			end
		end
	end
	return newID2oldUV
end

local function patch_funcs_upvalue(newID2oldUV, newFunc2oldFunc)
	for newFunc in pairs(newFunc2oldFunc) do
		for idx, name, value in debug.upvalues(newFunc) do
			if name == "" then break end
			local id = debug.upvalueid(newFunc, idx)
			local oldUV = newID2oldUV[id]
			if oldUV then
				debug.upvaluejoin(newFunc, idx, oldUV.old_func, oldUV.old_idx)
				if value ~= nil and type(value) ~= "table" then
					debug.setupvalue(newFunc, idx, value)
				end
			end
		end
	end
end

local function patch_tables(newTbl2oldTbl)
	for newTbl, oldTbl in pairs(newTbl2oldTbl) do
		for key, value in pairs(newTbl) do
			if type(value) ~= "table" or -- copy values not a table
					getmetatable(value) ~= nil or -- copy dummy
					oldTbl[key] == nil then -- copy new object
				oldTbl[key] = value
			end
		end
	end
end

local function set_object(real_value, old_real_mod, TFD_path)
	local node = old_real_mod
	local len = #TFD_path
	for idx = 2, len - 1, 1 do
		local key = TFD_path[idx]
		local t = type(node)
		if t == "table" then
			node = node[key]
		elseif t == "function" then
			node = find_upvalue(node, key)
		else
			assert(false)
		end
	end
	local t = type(node)
	if t == "table" then
		node[TFD_path[len]] = real_value
	elseif t == "function" then
		local _, i = find_upvalue(node, TFD_path[len])
		debug.setupvalue(node, i, real_value)
	else
		error(string.format("set_object error: %s, %s, %s", tostring(node), t, tostring(TFD_path, 1)))
	end
end

local function patch_addition_object(obj_path_list, old_real_mod)
	for _, obj_path in ipairs(obj_path_list) do
		set_object(obj_path[1], old_real_mod, obj_path)
	end
end

local function solve_dummies(dummy_path_list, old_real_mod)
	for _, dummy_path in pairs(dummy_path_list) do
		local dummy = dummy_path[1]
		local real_value, key = sandbox.value(dummy)
		set_object(real_value, old_real_mod, dummy_path)
	end
end

local function replace_all_function(newFunc2oldFunc)
	local oldF2newF = {}
	for newFunc, oldFunc in pairs(newFunc2oldFunc) do
		oldF2newF[oldFunc] = newFunc
	end

	local getmetatable = debug.getmetatable
	local getinfo = debug.getinfo
	local locals = debug.locals
	local setlocal = debug.setlocal
	local upvalues = debug.upvalues
	local setupvalue = debug.setupvalue
	local getuservalue = debug.getuservalue
	local setuservalue = debug.setuservalue
	local type = type
	local next = next
	local rawset = rawset

	local co = coroutine.running()
	local exclude = {[co] = true}

	local replace_recursive
	local function replace_recursive_function(func)
		if exclude[func] then return end
		exclude[func] = true
		for idx, name, value in upvalues(func) do
			if value then
				local t = type(value)
				if t == "function" then
					local newOne = oldF2newF[value]
					if newOne then
						setupvalue(func, idx, newOne)
					else
						replace_recursive_function(value)
					end
				else
					replace_recursive(value, t)
				end
			end
		end
	end
	local function replace_recursive_table(tbl)
		assert(type(tbl) == "table")
		if exclude[tbl] then return end
		exclude[tbl] = true
		local oldKey2newKey
		for key, value in next, tbl do
			local t_key = type(key)
			if t_key == "function" then
				local new_key = oldF2newF[key]
				if new_key then
					if not oldKey2newKey then oldKey2newKey = {} end
					oldKey2newKey[key] = new_key
				else
					replace_recursive_function(key)
				end
			else
				replace_recursive(key, t_key)
			end
			local t_value = type(value)
			if t_value == "function" then
				local new_value = oldF2newF[value]
				if new_value then
					rawset(tbl, key, new_value)
				else
					replace_recursive_function(value)
				end
			else
				replace_recursive(value, t_value)
			end
		end
		if oldKey2newKey then
			for oldKey, newKey in pairs(oldKey2newKey) do
				local value = tbl[oldKey]
				rawset(tbl, oldKey, nil)
				rawset(tbl, newKey, value)
			end
		end
		local mt = getmetatable(tbl)
		if mt then return replace_recursive_table(mt) end
	end
	local function replace_recursive_userdata(ud)
		if exclude[ud] then return end
		exclude[ud] = true
		local uv = getuservalue(root)
		if uv then
			local t = type(uv)
			if t == "function" then
				local nv = oldF2newF[uv]
				if nv then
					setuservalue(ud, nv)
				else
					replace_recursive_function(uv)
				end
			else
				replace_recursive(uv, t)
			end
		end
		local mt = getmetatable(ud)
		if mt then return replace_recursive_table(mt) end
	end
	local function replace_stack_frame(co, level)
		local info = getinfo(co, level, "f")
		if not info then return end
		local f = info.func
		info = nil
		replace_recursive_function(f)
		-- function 'locals': level == 0 means the top frame in the stack
		-- for current running thread: level == 1 means current stack (replace_stack_frame itself)
		-- for other thread: level == 1 means the second top frame in the stack
		for idx, name, value in locals(co, level) do -- idx < 0 means vararg
			if value then
				local t = type(value)
				if t == "function" then
					local newOne = oldF2newF[value]
					if newOne then
						setlocal(co, level, idx, newOne)
					else
						replace_recursive_function(value)
					end
				else
					replace_recursive(value, t)
				end
			end
		end
		return replace_stack_frame(co, level + 1) -- tailcall with level means check for outer stack
	end
	local function replace_recursive_thread(co)
		if exclude[co] then return end
		exclude[co] = true
		replace_stack_frame(co, 0) -- for other thread, start from the top frame
	end
	local replace_by_type = {
		table = replace_recursive_table,
		userdata = replace_recursive_userdata,
		thread = replace_recursive_thread,
		["function"] = replace_recursive_function,
	}
	function replace_recursive(value, t)
		local replace_func = replace_by_type[t]
		if replace_func then replace_func(value) end
	end

	-- nil, number, boolean, string, thread, function, lightuserdata may have metatable
	for _,v in pairs { nil, 0, true, "", co, replace_recursive, debug.upvalueid(replace_recursive,1) } do
		local mt = getmetatable(v)
		if mt then replace_recursive_table(mt) end
	end
	-- for current running thread: the level means
	-- 0 => locals itself
	-- 1 => replace_stack_frame itself
	-- 2 => replace_all_function, current stack
	-- 3 => reload_one, the outer
	-- 4 => pcall/xpcall
	-- 5 => reload
	-- 6 => which call the reload
	replace_stack_frame(co, 6)
	return replace_recursive_table(debug.getregistry())
end

local function reload_one(mod_name)
	local old_real_mod = require(mod_name)
	assert(old_real_mod, "reload module must be require succeed first: " .. mod_name)

	local mod_ref_dummy, loader = sandbox.require(mod_name)

	-- all dummy table/function in mod_ref_dummy
	local dummy_path_list, obj_path_list = enum_tbl_func_dummy(mod_ref_dummy)

	-- find all match objects and addition objects
	local newTbl2oldTbl, newFunc2oldFunc = match_objects(obj_path_list, old_real_mod)

	-- according to the match function, find all match upvalues
	local newID2oldUV = match_upvalues(newFunc2oldFunc)

	-- first: make new function join to the old upvalue, and update the upvalue's value
	patch_funcs_upvalue(newID2oldUV, newFunc2oldFunc) -- must before patch table

	-- second: update new table's value to the old table, contains the mod_ref_dummy itself and old_real_mod itself
	patch_tables(newTbl2oldTbl)

	if loader then
		-- update '_ENV' for all closure created by the loader
		debug.setupvalue(loader, 1, _ENV)
	end

	-- replace all dummies with real value
	solve_dummies(dummy_path_list, old_real_mod)

	-- traverse the VM, replace old function with the new one
	replace_all_function(newFunc2oldFunc)
end

---@class reload:table
local reload = {}

---@param modName string
---@return boolean, any
function reload.reload(modName)
	sandbox.init()
	local ok, result = xpcall(reload_one, debug.traceback, modName)
	sandbox.clear()
	return ok, result
end

return reload
