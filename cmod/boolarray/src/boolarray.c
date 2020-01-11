#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <stdio.h>
#include <limits.h>

#include <lauxlib.h>
#include <lua.h>

// the number of bits in an unsigned integer
#define BITS_PER_WORD (CHAR_BIT * sizeof(unsigned int))
// computes the word that stores the bit corresponding to a given index
// for n bit array, i: [0, n-1]
#define I_WORD(i) ((unsigned int)(i) / BITS_PER_WORD)
// computes a mask to access the correct bit inside this word
#define I_BIT(i) (1 << ((unsigned int)(i) % BITS_PER_WORD))
// mask for bits higher then i in this word, include i
#define I_HIGHER(i) ((unsigned int)(-1) << (((unsigned int)(i) % BITS_PER_WORD)))
// get position of the first 1 in a binary unsigned int
#define FIRST_ONE(w) binpos((w & (w - 1)) ^ w)

static int binpos(unsigned int index) {
  int cnt = 0;
  while (index) { // position of first bit is 1
    index >>= 1;
    cnt++;
  }
  return cnt;
}

#define CHECK_ARRAY(L, idx) (BitArray*)luaL_checkudata(L, idx, "LuaBook.array")

typedef struct BitArray {
  int size;
  unsigned int values[1]; /* variable part */
} BitArray;

static BitArray* allocarray(lua_State* L, int n) {
  // n bit, index: [0, n-1], last bit is n-1
  // index of word for last bit: I_WORD(n-1)
  // size of array: (I_WORD(n-1) + 1) * sizeof(unsigned int)
  size_t nbytes = sizeof(BitArray) + I_WORD(n - 1) * sizeof(unsigned int);
  BitArray* a = (BitArray*)lua_newuserdata(L, nbytes);
  a->size = n;
  luaL_getmetatable(L, "LuaBook.array");
  lua_setmetatable(L, -2);
  return a;
}

static int newarray(lua_State* L) {
  int i;
  BitArray* a;

  int n = (int)luaL_checkinteger(L, 1); /* number of bits */
  luaL_argcheck(L, n >= 1, 1, "invalid size");

  a = allocarray(L, n);

  for (i = 0; i <= (int)I_WORD(n - 1); i++)
    a->values[i] = 0; /* initialize array */

  return 1; /* new userdata is already on the stack */
}

static int getsize(lua_State* L) {
  lua_pushinteger(L, (CHECK_ARRAY(L, 1))->size);
  return 1;
}

static unsigned int* getparams(lua_State* L, unsigned int* mask) {
  BitArray* a = CHECK_ARRAY(L, 1);
  int index = (int)luaL_checkinteger(L, 2) - 1;
  luaL_argcheck(L, 0 <= index && index < a->size, 2, "index out of range");

  *mask = I_BIT(index); /* mask to access correct bit */
  return &a->values[I_WORD(index)]; /* word address */
}

static int setarray(lua_State* L) {
  unsigned int mask;
  unsigned int* entry = getparams(L, &mask);
  luaL_checkany(L, 3);
  if (lua_toboolean(L, 3))
    *entry |= mask;
  else
    *entry &= ~mask;

  return 0;
}

static int getarray(lua_State* L) {
  unsigned int mask;
  unsigned int* entry = getparams(L, &mask);
  lua_pushboolean(L, *entry & mask);
  return 1;
}

#define CHECK_TWO_ARRAY(large, small) \
  do { \
    BitArray* a = CHECK_ARRAY(L, 1); \
    BitArray* b = CHECK_ARRAY(L, 2); \
    if (a->size > b->size) { \
      large = a; \
      small = b; \
    } else { \
      large = b; \
      small = a; \
    } \
  } while (0)

static int makeintersection(lua_State* L) {
  BitArray* large = NULL;
  BitArray* small = NULL;
  CHECK_TWO_ARRAY(large, small);

  BitArray* u = allocarray(L, small->size);

  for (int i = 0; i <= (int)I_WORD(small->size - 1); i++) {
    u->values[i] = large->values[i] & small->values[i];
  }

  return 1;
}

static int makeunion(lua_State* L) {
  BitArray* large = NULL;
  BitArray* small = NULL;
  CHECK_TWO_ARRAY(large, small);

  BitArray* u = allocarray(L, large->size);

  for (int i = 0; i <= (int)I_WORD(small->size - 1); i++) {
    u->values[i] = large->values[i] | small->values[i];
  }
  for (int i = (int)I_WORD(small->size - 1) + 1; i <= (int)I_WORD(large->size - 1); i++) {
    u->values[i] = large->values[i];
  }

  return 1;
}

static int nextforarray(lua_State* L) {
  BitArray* a = CHECK_ARRAY(L, 1);
  int index = luaL_checkinteger(L, 2);
  luaL_argcheck(L, 0 <= index && index <= a->size, 2, "index out of range");
  if (index == a->size) { // last one?
    return 0;
  }
  // for Lua: index is "current index of value"
  // for C: index is "next index of value"
  int fwindex = I_WORD(index); // first word index
  int lwindex = I_WORD(a->size - 1); // last word index
  unsigned int mask = I_HIGHER(index); // include index
  unsigned int mask_zero = I_HIGHER(0); // mark all
  for (int i = fwindex; i <= lwindex; i++) { // for each word
    unsigned int word = a->values[i] & mask;
    if (word) {
      lua_pushinteger(L, i * BITS_PER_WORD + FIRST_ONE(word));
      lua_pushboolean(L, 1);
      return 2;
    }
    mask = mask_zero;
  }
  return 0;
}

static int pairsarray(lua_State* L) {
  CHECK_ARRAY(L, 1);
  lua_pushcfunction(L, nextforarray);
  lua_pushvalue(L, 1);
  lua_pushinteger(L, 0);
  return 3;
}

static int array2string(lua_State* L) {
  lua_pushfstring(L, "array(%d)", (CHECK_ARRAY(L, 1))->size);
  return 1;
}

static const struct luaL_Reg arraylib_f[] = {
    {"new", newarray},
    {"next", nextforarray},
    {NULL, NULL},
};

static const struct luaL_Reg arraylib_m[] = {
    {"__newindex", setarray},
    {"__index", getarray},
    {"__len", getsize},
    {"__mul", makeintersection},
    {"__add", makeunion},
    {"__pairs", pairsarray},
    {"__tostring", array2string},
    {NULL, NULL},
};

LUAMOD_API int luaopen_libboolarray(lua_State* L) {
  luaL_newmetatable(L, "LuaBook.array");
  luaL_setfuncs(L, arraylib_m, 0);
  luaL_newlib(L, arraylib_f);
  return 1;
}
