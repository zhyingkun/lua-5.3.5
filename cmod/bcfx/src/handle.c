#define _handle_c_
#include <handle.h>
#include <utils.h>

#define INDEX_BIT 12
#define TYPE_BIT 4
#define INDEX_OFFSET 0
#define TYPE_OFFSET INDEX_BIT

#define INDEX_MASK ((1 << INDEX_BIT) - 1)
#define TYPE_MASK ((1 << TYPE_BIT) - 1)

#define PACK_HANDLE(index, type) (((((uint16_t)(index)) & INDEX_MASK) << INDEX_OFFSET) | ((((uint16_t)(type)) & TYPE_MASK) << TYPE_OFFSET))
#define UNPACK_INDEX(handle) ((((uint16_t)handle) >> INDEX_OFFSET) & INDEX_MASK)
#define UNPACK_TYPE(handle) ((HandleType)((((uint16_t)handle) >> TYPE_OFFSET) & TYPE_MASK))

void handle_init(HandleAlloc* allocator, uint16_t max, HandleType type) {
  assert(max <= (1 << INDEX_BIT));
  allocator->num = 0;
  allocator->max = max;
  allocator->type = type;
  uint16_t* ptr = (uint16_t*)mem_malloc(sizeof(uint16_t) * max * 2);
  allocator->dense = ptr;
  allocator->sparse = ptr + max;
  for (uint16_t i = 0; i < max; i++) {
    allocator->dense[i] = i;
    allocator->sparse[i] = 0;
  }
}

Handle handle_alloc(HandleAlloc* allocator) {
  if (allocator->num < allocator->max) {
    uint16_t idx = allocator->num;
    allocator->num++;
    uint16_t index = allocator->dense[idx];
    allocator->sparse[index] = idx;
    return (Handle)PACK_HANDLE(index, allocator->type);
  }
  return kInvalidHandle;
}

void handle_free(HandleAlloc* allocator, Handle handle) {
  uint16_t index = UNPACK_INDEX(handle);
  uint16_t idx = allocator->sparse[index];
  allocator->num--;
  uint16_t temp = allocator->dense[allocator->num];
  allocator->dense[allocator->num] = index;
  allocator->sparse[temp] = idx;
  allocator->dense[idx] = temp;
}

bool handle_isvalid(HandleAlloc* allocator, Handle handle) {
  uint16_t index = UNPACK_INDEX(handle);
  uint16_t idx = allocator->sparse[index];
  return idx < allocator->num && allocator->dense[idx] == index;
}

uint16_t handle_index(Handle handle) {
  return UNPACK_INDEX(handle);
}

HandleType handle_type(Handle handle) {
  return UNPACK_TYPE(handle);
}

const char* handle_typename(HandleType type) {
  // clang-format off
  static const char* handle_name[] = {
      "None",
#define XX(name, config_max) #name,
      HANDLE_TYPE_MAP(XX)
#undef XX
      NULL,
  };
  // clang-format on
  if (type < HT_Count) {
    return handle_name[(uint8_t)type];
  }
  return "InvalidType";
}
