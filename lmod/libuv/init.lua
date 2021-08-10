local uvwrap = require("libuvwrap")
local loop = uvwrap.loop.default()

local function make_mt_index(module)
	return function(self, name)
		local value = uvwrap[module][name]
		self[name] = value
		return value
	end
end

local function make_mt_loop_index(module)
	return function(self, name)
		local value = uvwrap[module][name]
		if not value then return end
		local t = type(value)
		if t == "function" then
			local function func(...)
				return value(loop, ...)
			end
			self[name] = func
			return func
		else
			self[name] = value
			return value
		end
	end
end

local function make_func_loop(func)
	return function(...)
		return func(loop, ...)
	end
end

atexit(function()
	uvwrap.loop.run(loop)
	uvwrap.loop.close(loop)
end)

return setmetatable({
	loop = setmetatable({
		default = uvwrap.loop.default,
		new = uvwrap.loop.new,
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
