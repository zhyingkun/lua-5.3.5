local uvwrap = require("libuvwrap")

local function make_mt_index(module)
	return function(self, name)
		local value = uvwrap[module][name]
		self[name] = value
		return value
	end
end

local loop
local function set_loop(loop_)
	loop = loop_
end
local function make_func_loop(func)
	return function(...)
		if not loop then loop = uvwrap.loop.default() end
		return func(loop, ...)
	end
end

local function make_mt_loop_index(module)
	return function(self, name)
		local value = uvwrap[module][name]
		if not value then return end
		local t = type(value)
		if t == "function" then
			local func = make_func_loop(value)
			self[name] = func
			return func
		else
			self[name] = value
			return value
		end
	end
end

local run = uvwrap.loop.run
local close = uvwrap.loop.close
local set_realloc_cb = uvwrap.set_realloc_cb
atexit(function()
	if not loop then return end
	run(loop)
	close(loop)
	set_realloc_cb()
end)

return setmetatable({
	loop = setmetatable({
		default = uvwrap.loop.default,
		new = uvwrap.loop.new,
		run = function(loop_, mode)
			if not loop_ then loop_ = loop end
			run(loop_, mode)
		end,
		close = function(loop_)
			if not loop_ then loop_ = loop end
			close(loop_)
			if loop_ == loop then
				loop = nil
			end
		end,
		queue_work = uvwrap.loop.queue_work,
	}, {
		__index = make_mt_loop_index("loop"),
	}),
	fs = setmetatable({}, {
		__index = make_mt_loop_index("fs"),
	}),
	network = setmetatable({
		getaddrinfo = make_func_loop(uvwrap.network.getaddrinfo),
		getnameinfo = make_func_loop(uvwrap.network.getnameinfo),
	}, {
		__index = make_mt_index("network"),
	}),
	tcp = setmetatable({
		new = make_func_loop(uvwrap.tcp.new),
	}, {
		__index = make_mt_index("tcp"),
	}),
	udp = setmetatable({
		new = make_func_loop(uvwrap.udp.new),
	}, {
		__index = make_mt_index("udp"),
	}),
	fs_event = setmetatable({
		new = make_func_loop(uvwrap.fs_event.new),
	}, {
		__index = make_mt_index("fs_event"),
	}),
	process = setmetatable({
		new = make_func_loop(uvwrap.process.new),
	}, {
		__index = make_mt_index("process"),
	}),
	timer = setmetatable({
		new = make_func_loop(uvwrap.timer.new),
	}, {
		__index = make_mt_index("timer"),
	}),
	idle = setmetatable({
		new = make_func_loop(uvwrap.idle.new),
	}, {
		__index = make_mt_index("idle"),
	}),
	signal = setmetatable({
		new = make_func_loop(uvwrap.signal.new),
	}, {
		__index = make_mt_index("signal"),
	}),
	pipe = setmetatable({
		new = make_func_loop(uvwrap.pipe.new),
	}, {
		__index = make_mt_index("pipe"),
	}),
}, {
	__index = function(self, name)
		local value = uvwrap[name]
		self[name] = value
		return value
	end
})
