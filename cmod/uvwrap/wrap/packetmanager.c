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
  luaL_ByteBuffer bc[1]; // byte buffer for cache
  luaL_MemBuffer cache[1];
} PacketManager;

typedef enum {
  PS_OK,
  PS_NeedMore,
  PS_ErrorLength,
} PacketStatus;

static void _packLengthToBuffer(luaL_ByteBuffer* b, uint32_t len) {
  do {
    luaBB_addbyte(b, (uint8_t)(len | 0x80));
    len >>= 7;
  } while (len >= 0x80);
  luaBB_addbyte(b, (uint8_t)len);
}

static PacketStatus _unpackLengthFromBuffer(luaL_ByteBuffer* b, uint32_t* outLen) {
  uint32_t len = 0;
  const int maxByte = (sizeof(*outLen) * 8 + 6) / 7;
  for (int i = 0; i < maxByte; i++) {
    const uint8_t* byte = luaBB_readbytes(b, 1);
    if (byte == NULL) {
      return PS_NeedMore;
    }
    len |= (uint32_t)(*byte & 0x7F) << (7 * i);
    if ((*byte & 0x80) == 0) {
      *outLen = len;
      return PS_OK;
    }
  }
  return PS_ErrorLength;
}

static void pm_init(PacketManager* pm) {
  luaBB_init(pm->tmp, BASE_BUFFER_SIZE);
  luaBB_init(pm->b, BASE_BUFFER_SIZE);
  luaBB_static(pm->bc, NULL, 0, true, false);
  MEMBUFFER_SETNULL(pm->cache);
}

static void pm_destroy(PacketManager* pm) {
  luaBB_destroy(pm->tmp);
  luaBB_destroy(pm->b);
  luaBB_destroy(pm->bc);
  MEMBUFFER_RELEASE(pm->cache);
}

typedef struct {
  const char* typeName;
  const uint8_t* ptrBuffer;
  uint32_t typeLen;
  uint32_t ptrLen;
} Packet;

static const uint8_t* pm_packPacket(PacketManager* pm, const Packet* packet, size_t* sz) {
  luaBB_clear(pm->tmp);
  _packLengthToBuffer(pm->tmp, packet->typeLen);
  luaBB_addbytes(pm->tmp, (const uint8_t*)packet->typeName, packet->typeLen);
  _packLengthToBuffer(pm->tmp, packet->ptrLen);
  luaBB_addbytes(pm->tmp, packet->ptrBuffer, packet->ptrLen);
  if (sz) {
    *sz = pm->tmp->n;
  }
  return pm->tmp->b;
}

static void pm_appendCache(PacketManager* pm) {
  uint32_t count = luaBB_getremainforread(pm->bc);
  if (count > 0) {
    const uint8_t* ptr = luaBB_readbytes(pm->bc, count);
    luaBB_addbytes(pm->b, (const uint8_t*)ptr, (uint32_t)count);
  }
  MEMBUFFER_RELEASE(pm->cache);
}
static void pm_addPackData(PacketManager* pm, luaL_MemBuffer* mb) {
  pm_appendCache(pm);
  if (MEMBUFFER_CAN_CACHE(mb)) {
    luaBB_static(pm->bc, (uint8_t*)mb->ptr, (uint32_t)mb->sz, true, false);
    MEMBUFFER_MOVEINIT(mb, pm->cache);
  } else {
    luaBB_addbytes(pm->b, (const uint8_t*)mb->ptr, (uint32_t)mb->sz);
    MEMBUFFER_RELEASE(mb);
  }
}

static PacketStatus _unpackTuple(luaL_ByteBuffer* b, const uint8_t** outPtr, uint32_t* outLen) {
  uint32_t len = 0;
  PacketStatus status = _unpackLengthFromBuffer(b, &len);
  if (status == PS_OK) {
    const uint8_t* ptr = luaBB_readbytes(b, (uint32_t)len);
    if (ptr == NULL) {
      status = PS_NeedMore;
    } else {
      *outPtr = ptr;
      *outLen = len;
    }
  }
  return status;
}
static PacketStatus pm_nextPacketByteBuffer(luaL_ByteBuffer* b, Packet* outPacket) {
  luaBB_flushread(b);
  PacketStatus status = _unpackTuple(b, (const uint8_t**)&outPacket->typeName, &outPacket->typeLen);
  if (status == PS_OK) {
    status = _unpackTuple(b, (const uint8_t**)&outPacket->ptrBuffer, &outPacket->ptrLen);
  }
  switch (status) {
    case PS_OK:
      break;
    case PS_NeedMore:
      luaBB_undoread(b);
      break;
    case PS_ErrorLength:
      /* fall through */
    default:
      luaBB_clear(b);
      break;
  }
  return status;
}
static PacketStatus pm_nextPacket(PacketManager* pm, Packet* outPacket) {
again:
  if (!luaBB_isemptyforread(pm->b)) {
    const PacketStatus status = pm_nextPacketByteBuffer(pm->b, outPacket);
    if (status == PS_NeedMore && !luaBB_isemptyforread(pm->bc)) {
      pm_appendCache(pm);
      goto again;
    }
    return status;
  }
  if (!luaBB_isemptyforread(pm->bc)) {
    return pm_nextPacketByteBuffer(pm->bc, outPacket);
  }
  MEMBUFFER_RELEASE(pm->cache);
  return PS_NeedMore;
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
  size_t typeLen = 0;
  const char* typeName = luaL_checklstring(L, 2, &typeLen);
  size_t ptrLen;
  const uint8_t* ptrBuffer = (const uint8_t*)luaL_checklbuffer(L, 3, &ptrLen);
  const bool bUseString = luaL_optboolean(L, 4, false);

  const Packet packet = {typeName, ptrBuffer, (uint32_t)typeLen, (uint32_t)ptrLen};
  size_t sz;
  const uint8_t* ptr = pm_packPacket(pm, &packet, &sz);

  luaL_releasebuffer(L, 2);
  if (bUseString) {
    lua_pushlstring(L, (const char*)ptr, sz);
  } else {
    luaL_MemBuffer* mb = luaL_newmembuffer(L);
    MEMBUFFER_SETINIT_REF(mb, ptr, sz);
  }
  return 1;
}
static int PM_FUNCTION(addPackData)(lua_State* L) {
  PacketManager* pm = luaL_checkpacketmanager(L, 1);
  luaL_MemBuffer membuf = MEMBUFFER_NULL;
  luaL_MemBuffer* mb = luaL_tomembuffer(L, 2, &membuf);

  pm_addPackData(pm, mb);
  return 0;
}
static int _pushPacket(lua_State* L, Packet* packet, bool bUseString) {
  lua_pushlstring(L, packet->typeName, packet->typeLen);
  if (bUseString) {
    lua_pushlstring(L, (const char*)packet->ptrBuffer, packet->ptrLen);
  } else {
    luaL_MemBuffer* mb = luaL_newmembuffer(L);
    MEMBUFFER_SETINIT_REF(mb, packet->ptrBuffer, packet->ptrLen);
  }
  return 2;
}
static int PM_FUNCTION(getPacket)(lua_State* L) {
  PacketManager* pm = luaL_checkpacketmanager(L, 1);
  const bool bUseString = luaL_optboolean(L, 2, false);

  Packet packet[1];
  PacketStatus status = pm_nextPacket(pm, packet);
  lua_pushinteger(L, (int)status);
  if (status == PS_OK) {
    return _pushPacket(L, packet, bUseString) + 1;
  }
  return 1;
}
static int _nextPacket(lua_State* L) {
  PacketManager* pm = luaL_checkpacketmanager(L, 1);

  Packet packet[1];
  PacketStatus status = pm_nextPacket(pm, packet);
  if (status == PS_OK) {
    const bool bUseString = luaL_optboolean(L, lua_upvalueindex(1), false);
    return _pushPacket(L, packet, bUseString);
  }
  if (status == PS_ErrorLength) {
    return luaL_error(L, "Parse Packet Error: %d", status);
  }
  return 0;
}
static int PM_FUNCTION(eachPacket)(lua_State* L) {
  luaL_checkpacketmanager(L, 1);
  lua_settop(L, 2);
  lua_pushcclosure(L, _nextPacket, 1);
  lua_pushvalue(L, 1);
  return 2;
}
static int PM_FUNCTION(getRemainForRead)(lua_State* L) {
  PacketManager* pm = luaL_checkpacketmanager(L, 1);
  uint32_t n1 = luaBB_getremainforread(pm->b);
  uint32_t n2 = luaBB_getremainforread(pm->bc);
  lua_pushinteger(L, n1 + n2);
  return 1;
}
static int PM_FUNCTION(__gc)(lua_State* L) {
  PacketManager* pm = luaL_checkpacketmanager(L, 1);
  pm_destroy(pm);
  return 0;
}

#define EMPLACE_PM_FUNCTION(name) \
  { "" #name, PM_FUNCTION(name) }
static const luaL_Reg PM_FUNCTION(metafuncs)[] = {
    EMPLACE_PM_FUNCTION(packPacket),
    EMPLACE_PM_FUNCTION(addPackData),
    EMPLACE_PM_FUNCTION(getPacket),
    EMPLACE_PM_FUNCTION(eachPacket),
    EMPLACE_PM_FUNCTION(getRemainForRead),
    EMPLACE_PM_FUNCTION(__gc),
    {NULL, NULL},
};

static void PM_FUNCTION(init_metatable)(lua_State* L) {
  REGISTER_METATABLE(PACKET_MANATER_TYPE, PM_FUNCTION(metafuncs));
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
  REGISTER_ENUM_UVWRAP(packet_status);
  INVOKE_INIT_METATABLE(pm);
}

/* }====================================================== */
