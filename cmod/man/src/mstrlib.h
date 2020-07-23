#ifndef mstrlib_h
#define mstrlib_h

#include <manual.h>

static const ManualItem strlib_manual[] = {
    {"string.byte", "int, ... = string.byte(str [, i = 1 [, j = i]])"},
    {"string.char", "str = string.char(int [, ...])"},

    {"string.len", "int = string.len(str)"},
    {"string.format", "str = string.format(formatstring, ...)"},
    {"string.rep", "str = string.rep(str, n [, sep = \"\"])"},
    {"string.reverse", "str = string.reverse(str)"},

    {"string.lower", "str = string.lower(str)"},
    {"string.upper", "str = string.upper(s)"},

    {"string.find", "start, end = string.find(str, pattern [, init = 1 [, plain = false]])"},
    {"string.sub", "str = string.sub(str, i [, j = -1])"},
    {"string.gsub", "str, times = string.gsub(str, pattern, repl [, n])"},
    {"string.match", "str = string.match(str, pattern [, init = 1])"},
    {"string.gmatch", "iterator = string.gmatch(str, pattern)"},

    {"string.dump", "str = string.dump(function [, strip = false])"},
    {"string.pack", "str = string.pack(fmt, v1, v2, ...)"},
    {"string.packsize", "int = string.packsize(fmt)"},
    {"string.unpack", "v1, v2, ..., int = string.unpack(fmt, s [, pos = 1])"},

    {NULL, NULL},
};

#endif
