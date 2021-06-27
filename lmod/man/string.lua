return {
	byte = "function string.byte(str [, i = 1 [, j = i]]) return int, ... end",
	char = "function string.char(int [, ...]) return str end",

	len = "function string.len(str) return int end",
	format = "function string.format(formatstring, ...) return str end",
	rep = "function string.rep(str, n [, sep = \"\"]) return str end",
	reverse = "function string.reverse(str) return str end",

	lower = "function string.lower(str) return str end",
	upper = "function string.upper(s) return str end",

	find = "function string.find(str, pattern [, init = 1 [, plain = false]]) return start, end end",
	sub = "function string.sub(str, i [, j = -1]) return str end",
	gsub = "function string.gsub(str, pattern, repl [, n]) return str, times end",
	match = "function string.match(str, pattern [, init = 1]) return str end",
	gmatch = "function string.gmatch(str, pattern) return iterator end",

	dump = "function string.dump(function [, strip = false]) return str end",
	pack = "function string.pack(fmt, v1, v2, ...) return str end",
	packsize = "function string.packsize(fmt) return int end",
	unpack = "function string.unpack(fmt, s [, pos = 1]) return v1, v2, ..., int end",
}
