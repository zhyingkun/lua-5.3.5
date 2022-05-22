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
#define UNPACK_TYPE(handle) ((bcfx_EHandleType)((((uint16_t)handle) >> TYPE_OFFSET) & TYPE_MASK))

void handle_init(HandleAlloc* allocator, uint16_t max, bcfx_EHandleType type) {
  assert(max <= (1 << INDEX_BIT));
  allocator->num = 0;
  allocator->max = max;
  allocator->type = type;
  uint16_t* ptr = (uint16_t*)mem_malloc(sizeof(uint16_t) * max * 2);
  allocator->dense = ptr;
  allocator->sparse = ptr + max;
  for (uint16_t i = 0; i < max; i++) {
    allocator->dense[i] = i;
    allocator->sparse[i] = i;
  }
}

void handle_destroy(HandleAlloc* allocator) {
  mem_free((void*)allocator->dense);
  allocator->dense = NULL;
  allocator->sparse = NULL;
  allocator->max = 0;
  allocator->num = 0;
  allocator->type = HT_None;
}

Handle handle_alloc(HandleAlloc* allocator) {
  if (allocator->num < allocator->max) {
    uint16_t denseIdx = allocator->num;
    allocator->num++;
    uint16_t sparseIdx = allocator->dense[denseIdx];
    assert(allocator->sparse[sparseIdx] == denseIdx);
    return (Handle)PACK_HANDLE(sparseIdx, allocator->type);
  }
  return kInvalidHandle;
}

static void handle_swapDenseElement(HandleAlloc* allocator, uint16_t denseIdx1, uint16_t denseIdx2) {
  uint16_t sparseIdx1 = allocator->dense[denseIdx1];
  uint16_t sparseIdx2 = allocator->dense[denseIdx2];

  allocator->dense[denseIdx1] = sparseIdx2;
  allocator->sparse[sparseIdx2] = denseIdx1;
  allocator->dense[denseIdx2] = sparseIdx1;
  allocator->sparse[sparseIdx1] = denseIdx2;
}

void handle_free(HandleAlloc* allocator, Handle handle) {
  uint16_t sparseIdx = UNPACK_INDEX(handle);
  uint16_t denseIdx = allocator->sparse[sparseIdx];
  assert(allocator->num > 0);
  allocator->num--;
  uint16_t denseIdxTop = allocator->num;

  handle_swapDenseElement(allocator, denseIdx, denseIdxTop);
}

bool handle_isvalid(HandleAlloc* allocator, Handle handle) {
  uint16_t sparseIdx = UNPACK_INDEX(handle);
  uint16_t denseIdx = allocator->sparse[sparseIdx];
  assert(allocator->dense[denseIdx] == sparseIdx);
  return denseIdx < allocator->num;
}

uint16_t handle_index(Handle handle) {
  return UNPACK_INDEX(handle);
}

bcfx_EHandleType handle_type(Handle handle) {
  return UNPACK_TYPE(handle);
}

const char* handle_typename(bcfx_EHandleType type) {
  // clang-format off
  static const char* handle_name[] = {
      "None",
#define XX(name, config_max) #name,
      BCFX_RESOURCE_MAP(XX)
#undef XX
      NULL,
  };
  // clang-format on
  if (type < HT_Count) {
    return handle_name[(uint8_t)type];
  }
  return "InvalidType";
}
