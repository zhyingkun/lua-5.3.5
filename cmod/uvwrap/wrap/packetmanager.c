#define packetmanager_c
#include <uvwrap.h>

/*
** {======================================================
** Packet Manager for C
** =======================================================
*/

typedef struct {
  luaL_ByteBuffer tmp[1];
  luaL_ByteBuffer b[1];
} PacketManager;

typedef enum {
  PS_OK,
  PS_NeedMore,
  PS_ErrorLength,
} PacketStatus;

static void _packLengthToBuffer(luaL_ByteBuffer* b, size_t len) {
  do {
    luaBB_addbyte(b, (uint8_t)(len | 0x80));
    len >>= 7;
  } while (len >= 0x80);
  luaBB_addbyte(b, (uint8_t)len);
}

static PacketStatus _unpackLengthFromBuffer(luaL_ByteBuffer* b, size_t* plen) {
  size_t len = 0;
  for (int i = 0; i < 10; i++) {
    const uint8_t* byte = luaBB_getbytes(b, 1);
    if (byte == NULL) {
      return PS_NeedMore;
    }
    len |= (size_t)(*byte & 0x7F) << (7 * i);
    if ((*byte & 0x80) == 0) {
      if (plen) {
        *plen = len;
      }
      return PS_OK;
    }
  }
  return PS_ErrorLength;
}

static void pm_init(PacketManager* pm) {
  luaBB_init(pm->tmp, BASE_BUFFER_SIZE);
  luaBB_init(pm->b, BASE_BUFFER_SIZE);
}

static void pm_destroy(PacketManager* pm) {
  luaBB_destroy(pm->tmp);
  luaBB_destroy(pm->b);
}

static uint8_t* pm_packPacket(PacketManager* pm, const uint8_t* raw, size_t len, size_t* sz) {
  luaBB_clear(pm->tmp);
  _packLengthToBuffer(pm->tmp, len);
  luaBB_addbytes(pm->tmp, raw, len);
  if (sz) {
    *sz = pm->tmp->n;
  }
  return pm->tmp->b;
}

static void pm_addPackData(PacketManager* pm, const uint8_t* data, size_t len) {
  luaBB_flush(pm->b);
  luaBB_addbytes(pm->b, data, len);
}

static PacketStatus pm_nextPacket(PacketManager* pm, const uint8_t** pptr, size_t* plen) {
  size_t len;
  const uint8_t* ptr;
  PacketStatus status = _unpackLengthFromBuffer(pm->b, &len);
  switch (status) {
    case PS_OK:
      ptr = luaBB_getbytes(pm->b, len);
      if (ptr == NULL) {
        status = PS_NeedMore;
        luaBB_undoread(pm->b);
      } else {
        luaBB_flush(pm->b);
        if (pptr) {
          *pptr = ptr;
        }
        if (plen) {
          *plen = len;
        }
      }
      break;
    case PS_NeedMore:
      luaBB_undoread(pm->b);
      break;
    case PS_ErrorLength:
      /* fall through */
    default:
      luaBB_clear(pm->b);
      break;
  }
  return status;
}

/* }====================================================== */

/*
** {======================================================
** PacketManager
** =======================================================
*/

#define PM_FUNCTION(name) UVWRAP_FUNCTION(pm, name)

#define luaL_checkpacketmanager(L, idx) (PacketManager*)luaL_checkudata(L, idx, PACKET_MANATER_TYPE)

static int PM_FUNCTION(packPacket)(lua_State* L) {
  PacketManager* pm = luaL_checkpacketmanager(L, 1);
  size_t len;
  const char* data = luaL_checklstring(L, 2, &len);

  size_t sz;
  uint8_t* ptr = pm_packPacket(pm, (uint8_t*)data, len, &sz);
  lua_pushlstring(L, (const char*)ptr, sz);
  return 1;
}
static int PM_FUNCTION(addPackData)(lua_State* L) {
  PacketManager* pm = luaL_checkpacketmanager(L, 1);
  size_t len;
  const char* data = luaL_checklstring(L, 2, &len);

  pm_addPackData(pm, (uint8_t*)data, len);
  return 0;
}
static int PM_FUNCTION(getPacket)(lua_State* L) {
  PacketManager* pm = luaL_checkpacketmanager(L, 1);

  const uint8_t* ptr = NULL;
  size_t sz = 0;
  PacketStatus status = pm_nextPacket(pm, &ptr, &sz);
  lua_pushinteger(L, (int)status);
  if (status == PS_OK) {
    lua_pushlstring(L, (const char*)ptr, sz);
  } else {
    lua_pushnil(L);
  }
  return 2;
}
static int _nextPacket(lua_State* L) {
  PacketManager* pm = luaL_checkpacketmanager(L, 1);

  const uint8_t* ptr;
  size_t sz;
  PacketStatus status = pm_nextPacket(pm, &ptr, &sz);
  switch (status) {
    case PS_OK:
      lua_pushlstring(L, (const char*)ptr, sz);
      break;
    case PS_NeedMore:
      lua_pushnil(L);
      break;
    case PS_ErrorLength:
      /* fall through */
    default:
      luaL_error(L, "Parse Packet Error: %d", status);
      break;
  }
  return 1;
}
static int PM_FUNCTION(eachPacket)(lua_State* L) {
  luaL_checkpacketmanager(L, 1);
  lua_pushcfunction(L, _nextPacket);
  lua_pushvalue(L, 1);
  return 2;
}
static int PM_FUNCTION(__gc)(lua_State* L) {
  PacketManager* pm = luaL_checkpacketmanager(L, 1);
  pm_destroy(pm);
  return 1;
}

#define EMPLACE_PM_FUNCTION(name) \
  { #name, PM_FUNCTION(name) }
static const luaL_Reg PM_FUNCTION(metafuncs)[] = {
    EMPLACE_PM_FUNCTION(packPacket),
    EMPLACE_PM_FUNCTION(addPackData),
    EMPLACE_PM_FUNCTION(getPacket),
    EMPLACE_PM_FUNCTION(eachPacket),
    EMPLACE_PM_FUNCTION(__gc),
    {NULL, NULL},
};

static void PM_FUNCTION(init_metatable)(lua_State* L) {
  luaL_newmetatable(L, PACKET_MANATER_TYPE);
  luaL_setfuncs(L, PM_FUNCTION(metafuncs), 0);

  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1);
}

static int PM_FUNCTION(PacketManager)(lua_State* L) {
  PacketManager* pm = (PacketManager*)lua_newuserdata(L, sizeof(PacketManager));
  luaL_setmetatable(L, PACKET_MANATER_TYPE);
  pm_init(pm);
  return 1;
}

static const luaL_Reg PM_FUNCTION(funcs)[] = {
    EMPLACE_PM_FUNCTION(PacketManager),
    {NULL, NULL},
};

static const luaL_Enum UVWRAP_ENUM(packet_status)[] = {
    {"OK", PS_OK},
    {"NeedMore", PS_NeedMore},
    {"ErrorLength", PS_ErrorLength},
    {NULL, 0},
};

void PM_FUNCTION(init)(lua_State* L) {
  luaL_setfuncs(L, PM_FUNCTION(funcs), 0);
  REGISTE_ENUM_UVWRAP(packet_status);
  INVOKE_INIT_METATABLE(pm);
}

/* }====================================================== */
