/*
** $Id: ltablib.c,v 1.93.1.1 2017/04/19 17:20:42 roberto Exp $
** Library for Table Manipulation
** See Copyright Notice in lua.h
*/

#define ltablib_c
#define LUA_LIB

#include "lprefix.h"

#include <limits.h>
#include <stddef.h>
#include <string.h>

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
#include "luautil.h"

/*
** Operations that an object must define to mimic a table
** (some functions only need some of them)
*/
#define TAB_R 1 /* read */
#define TAB_W 2 /* write */
#define TAB_L 4 /* length */
#define TAB_RW (TAB_R | TAB_W) /* read/write */

#define aux_getn(L, n, w) (checktab(L, n, (w) | TAB_L), luaL_len(L, n))

static int checkfield(lua_State* L, const char* key, int n) {
  lua_pushstring(L, key);
  return (lua_rawget(L, -n) != LUA_TNIL);
}

/*
** Check that 'arg' either is a table or can behave like one (that is,
** has a metatable with the required metamethods)
*/
static void checktab(lua_State* L, int arg, int what) {
  if (lua_type(L, arg) != LUA_TTABLE) { /* is it not a table? */
    int n = 1; /* number of elements to pop */
    if (lua_getmetatable(L, arg) && /* must have metatable */
        (!(what & TAB_R) || checkfield(L, "__index", ++n)) && (!(what & TAB_W) || checkfield(L, "__newindex", ++n)) &&
        (!(what & TAB_L) || checkfield(L, "__len", ++n))) {
      lua_pop(L, n); /* pop metatable and tested metamethods */
    } else
      luaL_checktype(L, arg, LUA_TTABLE); /* force an error */
  }
}

#if defined(LUA_COMPAT_MAXN)
static int maxn(lua_State* L) {
  lua_Number max = 0;
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_pushnil(L); /* first key */
  while (lua_next(L, 1)) {
    lua_pop(L, 1); /* remove value */
    if (lua_type(L, -1) == LUA_TNUMBER) {
      lua_Number v = lua_tonumber(L, -1);
      if (v > max)
        max = v;
    }
  }
  lua_pushnumber(L, max);
  return 1;
}
#endif

// table.insert(list, [pos,] value)
static int tinsert(lua_State* L) {
  lua_Integer e = aux_getn(L, 1, TAB_RW) + 1; /* first empty element */
  lua_Integer pos; /* where to insert new element */
  switch (lua_gettop(L)) {
    case 2: { /* called with only 2 arguments */
      pos = e; /* insert new element at the end */
      break;
    }
    case 3: {
      lua_Integer i;
      pos = luaL_checkinteger(L, 2); /* 2nd argument is the position */
      luaL_argcheck(L, 1 <= pos && pos <= e, 2, "position out of bounds");
      for (i = e; i > pos; i--) { /* move up elements */
        lua_geti(L, 1, i - 1);
        lua_seti(L, 1, i); /* t[i] = t[i - 1] */
      }
      break;
    }
    default: {
      return luaL_error(L, "wrong number of arguments to 'insert'");
    }
  }
  lua_seti(L, 1, pos); /* t[pos] = v */
  return 0;
}

// table.remove(list [, pos])
static int tremove(lua_State* L) {
  lua_Integer size = aux_getn(L, 1, TAB_RW);
  lua_Integer pos = luaL_optinteger(L, 2, size);
  if (pos != size) /* validate 'pos' if given */
    luaL_argcheck(L, 1 <= pos && pos <= size + 1, 1, "position out of bounds");
  lua_geti(L, 1, pos); /* result = t[pos] */
  for (; pos < size; pos++) {
    lua_geti(L, 1, pos + 1);
    lua_seti(L, 1, pos); /* t[pos] = t[pos + 1] */
  }
  lua_pushnil(L);
  lua_seti(L, 1, pos); /* t[pos] = nil */
  return 1;
}

/*
** Copy elements (1[f], ..., 1[e]) into (tt[t], tt[t+1], ...). Whenever
** possible, copy in increasing order, which is better for rehashing.
** "possible" means destination after original range, or smaller
** than origin, or copying to another table.
*/
static int tmove(lua_State* L) {
  lua_Integer f = luaL_checkinteger(L, 2);
  lua_Integer e = luaL_checkinteger(L, 3);
  lua_Integer t = luaL_checkinteger(L, 4);
  int tt = !lua_isnoneornil(L, 5) ? 5 : 1; /* destination table */
  checktab(L, 1, TAB_R);
  checktab(L, tt, TAB_W);
  if (e >= f) { /* otherwise, nothing to move */
    lua_Integer n, i;
    luaL_argcheck(L, f > 0 || e < LUA_MAXINTEGER + f, 3, "too many elements to move");
    n = e - f + 1; /* number of elements to move */
    luaL_argcheck(L, t <= LUA_MAXINTEGER - n + 1, 4, "destination wrap around");
    if (t > e || t <= f || (tt != 1 && !lua_compare(L, 1, tt, LUA_OPEQ))) {
      for (i = 0; i < n; i++) {
        lua_geti(L, 1, f + i);
        lua_seti(L, tt, t + i);
      }
    } else {
      for (i = n - 1; i >= 0; i--) {
        lua_geti(L, 1, f + i);
        lua_seti(L, tt, t + i);
      }
    }
  }
  lua_pushvalue(L, tt); /* return destination table */
  return 1;
}

static void addfield(lua_State* L, luaL_Buffer* b, lua_Integer i) {
  lua_geti(L, 1, i);
  if (!lua_isstring(L, -1))
    luaL_error(L, "invalid value (%s) at index %d in table for 'concat'", luaL_typename(L, -1), i);
  luaL_addvalue(b);
}

// table.concat(list [, sep [, i [, j]]])
static int tconcat(lua_State* L) {
  luaL_Buffer b;
  lua_Integer last = aux_getn(L, 1, TAB_R);
  size_t lsep;
  const char* sep = luaL_optlstring(L, 2, "", &lsep);
  lua_Integer i = luaL_optinteger(L, 3, 1);
  last = luaL_optinteger(L, 4, last);
  luaL_buffinit(L, &b);
  for (; i < last; i++) {
    addfield(L, &b, i);
    luaL_addlstring(&b, sep, lsep);
  }
  if (i == last) /* add last value (if interval was not empty) */
    addfield(L, &b, i);
  luaL_pushresult(&b);
  return 1;
}

/*
** {======================================================
** Pack/unpack
** =======================================================
*/

// table.pack(···)
static int pack(lua_State* L) {
  int i;
  int n = lua_gettop(L); /* number of elements to pack */
  lua_createtable(L, n, 1); /* create result table */
  lua_insert(L, 1); /* put it at index 1 */
  for (i = n; i >= 1; i--) /* assign elements */
    lua_seti(L, 1, i);
  lua_pushinteger(L, n);
  lua_setfield(L, 1, "n"); /* t.n = number of elements */
  return 1; /* return table */
}

// table.unpack(list [, i [, j]])
static int unpack(lua_State* L) {
  lua_Unsigned n;
  lua_Integer i = luaL_optinteger(L, 2, 1);
  lua_Integer e = luaL_opt(L, luaL_checkinteger, 3, luaL_len(L, 1));
  if (i > e)
    return 0; /* empty range */
  n = (lua_Unsigned)e - i; /* number of elements minus 1 (avoid overflows) */
  if (n >= (unsigned int)INT_MAX || !lua_checkstack(L, (int)(++n)))
    return luaL_error(L, "too many results to unpack");
  for (; i < e; i++) { /* push arg[i..e - 1] (to avoid overflows) */
    lua_geti(L, 1, i);
  }
  lua_geti(L, 1, e); /* push last element */
  return (int)n;
}

/* }====================================================== */

/*
** {======================================================
** Quicksort
** (based on 'Algorithms in MODULA-3', Robert Sedgewick;
**  Addison-Wesley, 1993.)
** =======================================================
*/

/* type for array indices */
typedef unsigned int IdxT;

/*
** Produce a "random" 'unsigned int' to randomize pivot choice. This
** macro is used only when 'sort' detects a big imbalance in the result
** of a partition. (If you don't want/need this "randomness", ~0 is a
** good choice.)
*/
#if !defined(l_randomizePivot) /* { */

#include <time.h>

/* size of 'e' measured in number of 'unsigned int's */
#define sof(e) (sizeof(e) / sizeof(unsigned int))

/*
** Use 'time' and 'clock' as sources of "randomness". Because we don't
** know the types 'clock_t' and 'time_t', we cannot cast them to
** anything without risking overflows. A safe way to use their values
** is to copy them to an array of a known type and use the array values.
*/
static unsigned int l_randomizePivot(void) {
  clock_t c = clock();
  time_t t = time(NULL);
  unsigned int buff[sof(c) + sof(t)];
  unsigned int i, rnd = 0;
  memcpy(buff, &c, sof(c) * sizeof(unsigned int));
  memcpy(buff + sof(c), &t, sof(t) * sizeof(unsigned int));
  for (i = 0; i < sof(buff); i++)
    rnd += buff[i];
  return rnd;
}

#endif /* } */

/* arrays larger than 'RANLIMIT' may use randomized pivots */
#define RANLIMIT 100u

static void set2(lua_State* L, IdxT i, IdxT j) {
  lua_seti(L, 1, i);
  lua_seti(L, 1, j);
}

/*
** Return true iff value at stack index 'a' is less than the value at
** index 'b' (according to the order of the sort).
*/
// L->ci->func[1] ==> table for sort
// L->ci->func[2] ==> compare function
static int sort_comp(lua_State* L, int a, int b) {
  if (lua_isnil(L, 2)) /* no function? */
    return lua_compare(L, a, b, LUA_OPLT); /* a < b */
  else { /* function */
    int res;
    lua_pushvalue(L, 2); /* push function */
    lua_pushvalue(L, a - 1); /* -1 to compensate function */
    lua_pushvalue(L, b - 2); /* -2 to compensate function and 'a' */
    lua_call(L, 2, 1); /* call function */
    res = lua_toboolean(L, -1); /* get result */
    lua_pop(L, 1); /* pop result */
    return res;
  }
}

/*
** Does the partition: Pivot P is at the top of the stack.
** precondition: a[lo] <= P == a[up-1] <= a[up],
** so it only needs to do the partition from lo + 1 to up - 2.
** Pos-condition: a[lo .. i - 1] <= a[i] == P <= a[i + 1 .. up]
** returns 'i'.
*/
static IdxT partition(lua_State* L, IdxT lo, IdxT up) {
  IdxT i = lo; /* will be incremented before first use */
  IdxT j = up - 1; /* will be decremented before first use */
  /* loop invariant: a[lo .. i] <= P <= a[j .. up] */
  for (;;) {
    /* next loop: repeat ++i while a[i] < P */
    while ((void)(lua_geti(L, 1, ++i)), sort_comp(L, -1, -2)) {
      if (i == up - 1) /* a[i] < P  but a[up - 1] == P  ?? */
        luaL_error(L, "invalid order function for sorting");
      lua_pop(L, 1); /* remove a[i] */
    }
    /* after the loop, a[i] >= P and a[lo .. i - 1] < P */
    /* next loop: repeat --j while P < a[j] */
    while ((void)(lua_geti(L, 1, --j)), sort_comp(L, -3, -1)) {
      if (j < i) /* j < i  but  a[j] > P ?? */
        luaL_error(L, "invalid order function for sorting");
      lua_pop(L, 1); /* remove a[j] */
    }
    /* after the loop, a[j] <= P and a[j + 1 .. up] >= P */
    if (j < i) { /* no elements out of place? */
      /* a[lo .. i - 1] <= P <= a[j + 1 .. i .. up] */
      lua_pop(L, 1); /* pop a[j] */
      /* swap pivot (a[up - 1]) with a[i] to satisfy pos-condition */
      set2(L, up - 1, i);
      return i;
    }
    /* otherwise, swap a[i] - a[j] to restore invariant and repeat */
    set2(L, i, j);
  }
}

/*
** Choose an element in the middle (2nd-3th quarters) of [lo,up]
** "randomized" by 'rnd'
*/
static IdxT choosePivot(IdxT lo, IdxT up, unsigned int rnd) {
  IdxT r4 = (up - lo) / 4; /* range/4 */
  IdxT p = rnd % (r4 * 2) + (lo + r4);
  lua_assert(lo + r4 <= p && p <= up - r4);
  return p;
}

/*
** QuickSort algorithm (recursive function)
*/
// L->ci->func[1] ==> table for sort
// L->ci->func[2] ==> compare function
// Incremental ordering, rnd ==> random pivot
static void auxsort(lua_State* L, IdxT lo, IdxT up, unsigned int rnd) {
  while (lo < up) { /* loop for tail recursion */
    IdxT p; /* Pivot index */
    IdxT n; /* to be used later */
    /* sort elements 'lo', 'p', and 'up' */
    lua_geti(L, 1, lo);
    lua_geti(L, 1, up);
    if (sort_comp(L, -1, -2)) /* a[up] < a[lo]? */
      set2(L, lo, up); /* swap a[lo] - a[up] */
    else
      lua_pop(L, 2); /* remove both values */
    if (up - lo == 1) /* only 2 elements? */
      return; /* already sorted */
    if (up - lo < RANLIMIT || rnd == 0) /* small interval or no randomize? */
      p = (lo + up) / 2; /* middle element is a good pivot */
    else /* for larger intervals, it is worth a random pivot */
      p = choosePivot(lo, up, rnd);
    lua_geti(L, 1, p);
    lua_geti(L, 1, lo);
    if (sort_comp(L, -2, -1)) /* a[p] < a[lo]? */
      set2(L, p, lo); /* swap a[p] - a[lo] */
    else {
      lua_pop(L, 1); /* remove a[lo] */
      lua_geti(L, 1, up);
      if (sort_comp(L, -1, -2)) /* a[up] < a[p]? */
        set2(L, p, up); /* swap a[up] - a[p] */
      else
        lua_pop(L, 2);
    }
    if (up - lo == 2) /* only 3 elements? */
      return; /* already sorted */
    lua_geti(L, 1, p); /* get middle element (Pivot) */
    lua_pushvalue(L, -1); /* push Pivot */
    lua_geti(L, 1, up - 1); /* push a[up - 1] */
    set2(L, p, up - 1); /* swap Pivot (a[p]) with a[up - 1] */
    p = partition(L, lo, up);
    /* a[lo .. p - 1] <= a[p] == P <= a[p + 1 .. up] */
    if (p - lo < up - p) { /* lower interval is smaller? */
      auxsort(L, lo, p - 1, rnd); /* call recursively for lower interval */
      n = p - lo; /* size of smaller interval */
      lo = p + 1; /* tail call for [p + 1 .. up] (upper interval) */
    } else {
      auxsort(L, p + 1, up, rnd); /* call recursively for upper interval */
      n = up - p; /* size of smaller interval */
      up = p - 1; /* tail call for [lo .. p - 1]  (lower interval) */
    }
    if ((up - lo) / 128 > n) /* partition too imbalanced? */
      rnd = l_randomizePivot(); /* try a new randomization */
  } /* tail call auxsort(L, lo, up, rnd) */
}

// table.sort (list [, comp])
static int sort(lua_State* L) {
  lua_Integer n = aux_getn(L, 1, TAB_RW);
  if (n > 1) { /* non-trivial interval? */
    luaL_argcheck(L, n < INT_MAX, 1, "array too big");
    if (!lua_isnoneornil(L, 2)) /* is there a 2nd argument? */
      luaL_checktype(L, 2, LUA_TFUNCTION); /* must be a function */
    lua_settop(L, 2); /* make sure there are two arguments */
    auxsort(L, 1, (IdxT)n, 0);
  }
  return 0;
}

static int create(lua_State* L) {
  int narr = (int)luaL_optinteger(L, 1, 0);
  int nrec = (int)luaL_optinteger(L, 2, 0);
  lua_createtable(L, narr, nrec);
  return 1;
}

static int copy(lua_State* L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  int copykv = luaL_optboolean(L, 2, 1);
  lua_copytable(L, 1, copykv);
  return 1;
}

static int rehash(lua_State* L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_rehashtable(L, 1);
  return 0;
}

static int every(lua_State* L) {
  lua_settop(L, 3);
  int bList = luaL_optboolean(L, 3, 1);
  if (bList) {
    lua_Integer last = aux_getn(L, 1, TAB_R);
    for (lua_Integer i = 1; i <= last; i++) {
      lua_pushvalue(L, 2);
      lua_geti(L, 1, i);
      lua_pushinteger(L, i);
      lua_call(L, 2, 1);
      if (!lua_toboolean(L, -1)) {
        return 1;
      }
      lua_pop(L, 1);
    }
  } else {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_pushnil(L);
    int keyIdx = lua_gettop(L);
    while (lua_next(L, 1)) {
      lua_pushvalue(L, 2);
      lua_pushvalue(L, keyIdx + 1);
      lua_pushvalue(L, keyIdx);
      lua_call(L, 2, 1);
      if (!lua_toboolean(L, -1)) {
        return 1;
      }
      lua_pop(L, 2); // pop the ret and value
    }
  }
  lua_pushboolean(L, 1);
  return 1;
}

static int filter(lua_State* L) {
#define TABLE_IDX 1
#define FUNC_IDX 2
  luaL_checktype(L, TABLE_IDX, LUA_TTABLE);
  int bList = luaL_optboolean(L, 3, 1);
  lua_copytable(L, TABLE_IDX, 0);
  int newIdx = lua_gettop(L);
  if (bList) {
    lua_Integer len = luaL_len(L, TABLE_IDX);
    lua_Integer idx = 1;
    for (lua_Integer i = 1; i <= len; i++) {
      lua_rawgeti(L, TABLE_IDX, i);
      lua_pushvalue(L, FUNC_IDX);
      lua_pushvalue(L, -2);
      lua_pushinteger(L, i);
      lua_call(L, 2, 1);
      if (lua_toboolean(L, -1)) {
        lua_pop(L, 1); // pop the return boolean
        lua_rawseti(L, newIdx, idx++);
      } else {
        lua_pop(L, 2); // pop the return boolean and value
      }
    }
  } else {
    lua_pushnil(L);
    int cacheIdx = lua_gettop(L);
    lua_pushnil(L);
    int keyIdx = lua_gettop(L);
    while (lua_next(L, TABLE_IDX)) {
      lua_pushvalue(L, FUNC_IDX);
      lua_pushvalue(L, keyIdx + 1);
      lua_pushvalue(L, keyIdx);
      lua_call(L, 2, 1);
      if (lua_toboolean(L, -1)) {
        lua_pop(L, 1); // pop the return boolean
        lua_copy(L, keyIdx, cacheIdx);
        lua_rawset(L, newIdx);
        lua_pushvalue(L, cacheIdx);
      } else {
        lua_pop(L, 2); // pop the ret and value
      }
    }
    lua_pop(L, 1); // pop the cache
  }
  return 1;
#undef FUNC_IDX
#undef TABLE_IDX
}

static int indexOf(lua_State* L) {
  if (lua_isnoneornil(L, 2)) {
    return luaL_argerror(L, 2, "value expected");
  }
  lua_settop(L, 3);
  int bList = luaL_optboolean(L, 3, 1);
  if (bList) {
    lua_Integer last = aux_getn(L, 1, TAB_R);
    for (lua_Integer i = 1; i <= last; i++) {
      lua_geti(L, 1, i);
      if (lua_compare(L, 2, -1, LUA_OPEQ)) {
        lua_pushinteger(L, i);
        return 1;
      }
      lua_pop(L, 1);
    }
  } else {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_pushnil(L);
    while (lua_next(L, 1)) {
      if (lua_compare(L, 2, -1, LUA_OPEQ)) {
        lua_pop(L, 1);
        return 1;
      }
      lua_pop(L, 1);
    }
  }
  return 0;
}

static int map(lua_State* L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_settop(L, 3);
  if (lua_isnoneornil(L, 3)) {
    lua_copytable(L, 1, 0);
    lua_replace(L, 3);
  } else {
    checktab(L, 3, TAB_W);
  }

  lua_pushnil(L);
  int keyIdx = lua_gettop(L);
  while (lua_next(L, 1)) {
    lua_pushvalue(L, keyIdx);
    lua_pushvalue(L, 2); // push the callback function
    lua_pushvalue(L, keyIdx + 1);
    lua_pushvalue(L, keyIdx);
    lua_call(L, 2, 1); // key value key func value key
    lua_rawset(L, 3); // set to the dst
    lua_pop(L, 1); // pop the value
  }
  return 1;
}

static int reduce(lua_State* L) {
  lua_Integer cnt = aux_getn(L, 1, TAB_R);
  lua_Integer start = 1;
  lua_settop(L, 3);
  if (lua_isnoneornil(L, 3)) { // accumulator
    lua_geti(L, 1, start);
    start++;
    lua_replace(L, 3);
  }
  for (lua_Integer idx = start; idx <= cnt; idx++) {
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3); // accumulator
    lua_geti(L, 1, idx);
    lua_pushinteger(L, idx);
    lua_pushvalue(L, 1);
    lua_call(L, 4, 1);
    lua_replace(L, 3);
  }
  return 1;
}

#define INC_BETWEEN(x, a, b) ((a) <= (x) && (x) <= (b))
static lua_Integer findAbsIndex(lua_Integer len, lua_Integer idx, int bStart) {
  if (INC_BETWEEN(idx, -len, -1)) {
    return idx + len + 1;
  } else if (INC_BETWEEN(idx, 1, len)) {
    return idx;
  }
  lua_Integer add = bStart ? 1 : 0;
  if (idx < -len || idx == 0) { // left
    return add;
  } else { // right
    return len + add;
  }
}

static int reverse(lua_State* L) {
  lua_Integer n = aux_getn(L, 1, TAB_R);
  lua_Integer f = luaL_optinteger(L, 2, 1);
  lua_Integer e = luaL_optinteger(L, 3, n);
  int tt = !lua_isnoneornil(L, 4) ? 4 : 1; /* destination table */
  checktab(L, tt, TAB_W);
  f = findAbsIndex(n, f, 1);
  e = findAbsIndex(n, e, 0);
  for (; f < e; f++, e--) {
    lua_geti(L, 1, f);
    lua_geti(L, 1, e);
    lua_seti(L, tt, f);
    lua_seti(L, tt, e);
  }
  lua_pushvalue(L, tt); /* return destination table */
  return 1;
}

static int slice(lua_State* L) {
#define TABLE_IDX 1
  luaL_checktype(L, TABLE_IDX, LUA_TTABLE);
  lua_Integer len = luaL_len(L, TABLE_IDX);
  lua_Integer s = luaL_optinteger(L, 2, 1);
  lua_Integer e = luaL_optinteger(L, 3, len);
  s = findAbsIndex(len, s, 1);
  e = findAbsIndex(len, e, 0);

  lua_copytable(L, TABLE_IDX, 0);
  int newIdx = lua_gettop(L);
  for (lua_Integer i = s, j = 1; i <= e; i++, j++) {
    lua_rawgeti(L, TABLE_IDX, i);
    lua_rawseti(L, newIdx, j);
  }
  return 1;
#undef TABLE_IDX
}

#define CLAMP(val, min, max) (val < min ? min : (val > max ? max : val))
static int splice(lua_State* L) {
#define TABLE_IDX 1
#define START_IDX 4
  luaL_checktype(L, TABLE_IDX, LUA_TTABLE);
  lua_Integer len = luaL_len(L, TABLE_IDX);
  lua_Integer start = findAbsIndex(len, luaL_optinteger(L, 2, len), 1);
  lua_Integer cnt = luaL_optinteger(L, 3, 1);
  lua_Integer max = len - start + 1;
  cnt = CLAMP(cnt, 0, max);
  int endIdx = lua_gettop(L);

  lua_copytable(L, TABLE_IDX, 0);
  int newIdx = endIdx + 1;
  if (START_IDX <= endIdx) {
    lua_reverse(L, START_IDX, endIdx + 1);
    newIdx = START_IDX;
  }

  lua_Integer j = 1;
  for (lua_Integer i = 1; i < start; i++, j++) {
    lua_rawgeti(L, TABLE_IDX, i);
    lua_rawseti(L, newIdx, j);
  }
  for (lua_Integer i = START_IDX; i <= endIdx; i++, j++) {
    lua_rawseti(L, newIdx, j);
  }
  lua_assert(newIdx == lua_gettop(L));
  for (lua_Integer i = start + cnt; i <= len; i++, j++) {
    lua_rawgeti(L, TABLE_IDX, i);
    lua_rawseti(L, newIdx, j);
  }
  return 1;
#undef START_IDX
#undef TABLE_IDX
}

/* }====================================================== */

static const luaL_Reg tab_funcs[] = {
    {"concat", tconcat},
#if defined(LUA_COMPAT_MAXN)
    {"maxn", maxn},
#endif
    {"insert", tinsert},
    {"pack", pack},
    {"unpack", unpack},
    {"remove", tremove},
    {"move", tmove},
    {"sort", sort},
    {"create", create},
    {"copy", copy},
    {"rehash", rehash},
    {"every", every},
    {"filter", filter},
    {"indexOf", indexOf},
    {"map", map},
    {"reduce", reduce},
    {"reverse", reverse},
    {"slice", slice},
    {"splice", splice},
    {NULL, NULL},
};

LUAMOD_API int luaopen_table(lua_State* L) {
  luaL_newlib(L, tab_funcs);
#if defined(LUA_COMPAT_UNPACK)
  /* _G.unpack = table.unpack */
  lua_getfield(L, -1, "unpack");
  lua_setglobal(L, "unpack");
#endif
  METATABLE_INDEX_TO_SELF(-1);
  return 1;
}
