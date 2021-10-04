#define _handle_c_
#include <handle.h>
#include <utils.h>

#define HANDLE_BIT 12
#define TYPE_BIT 4
#define HANDLE_OFFSET 0
#define TYPE_OFFSET HANDLE_BIT

#define HANDLE_MASK ((1 << HANDLE_BIT) - 1)
#define TYPE_MASK ((1 << TYPE_BIT) - 1)

#define PACK_HANDLE(handle, type) ((((handle)&HANDLE_MASK) << HANDLE_OFFSET) | (((type)&TYPE_MASK) << TYPE_OFFSET))
#define UNPACK_HANDLE(packed_handle) ((((uint16_t)packed_handle) >> HANDLE_OFFSET) & HANDLE_MASK)
#define UNPACK_TYPE(packed_handle) ((((uint16_t)packed_handle) >> TYPE_OFFSET) & TYPE_MASK)

void handle_init(HandleAlloc* allocator, uint16_t max, HandleType type) {
  allocator->num = 0;
  allocator->max = max;
  allocator->type = (uint8_t)type;
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
    uint16_t index = allocator->num;
    allocator->num++;
    uint16_t handle = allocator->dense[index];
    allocator->sparse[handle] = index;
    return (Handle)PACK_HANDLE(handle, allocator->type);
  }
  return kInvalidHandle;
}

void handle_free(HandleAlloc* allocator, Handle packed_handle) {
  uint16_t handle = UNPACK_HANDLE(packed_handle);
  uint16_t index = allocator->sparse[handle];
  uint16_t num = allocator->num;
  num--;
  allocator->num = num;
  uint16_t temp = allocator->dense[num];
  allocator->dense[num] = handle;
  allocator->sparse[temp] = index;
  allocator->dense[index] = temp;
}

bool handle_isvalid(HandleAlloc* allocator, Handle packed_handle) {
  uint16_t handle = UNPACK_HANDLE(packed_handle);
  uint16_t index = allocator->sparse[handle];
  return index < allocator->num && allocator->dense[index] == handle;
}

uint16_t handle_index(Handle packed_handle) {
  return (uint16_t)UNPACK_HANDLE(packed_handle);
}

HandleType handle_type(Handle packed_handle) {
  return (HandleType)UNPACK_TYPE(packed_handle);
}

const char* handle_typename(HandleType type) {
  // clang-format off
  static const char* handle_name[] = {
#define XX(name, config_max) #name,
      HANDLE_TYPE_MAP(XX)
#undef XX
      NULL,
  };
  // clang-format on
  if (type < HT_MAX) {
    return handle_name[(uint8_t)type];
  }
  return "InvalidType";
}
