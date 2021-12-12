#define _mesh_c_
#include <bcfx_wrap.h>
#include <objloader.h>

static void _freeMeshMemory(void* ud, void* ptr) {
  (void)ud;
  free(ptr);
}
static int MESH_FUNCTION(MeshParse)(lua_State* L) {
  bcfx_MemBuffer* mb = luaL_checkmembuffer(L, 1);

  MeshLoaded meshLoaded = objloader_loadMesh(mb->ptr, mb->sz);
  MeshArray* ma = &meshLoaded.ma;
  MEMBUFFER_RELEASE(mb);

  void* vertexPtr = (void*)ma->arr[0].vertexArray.arr;
  ma->arr[0].vertexArray.arr = NULL;
  size_t vertexSize = ma->arr[0].vertexArray.num * sizeof(Vertex);
  void* indexPtr = (void*)ma->arr[0].indexArray.arr;
  ma->arr[0].indexArray.arr = NULL;
  size_t indexSize = ma->arr[0].indexArray.num * sizeof(uint32_t);

  ma_destroy(ma);

  bcfx_MemBuffer* vmb = luaL_newmembuffer(L);
  MEMBUFFER_SET(vmb, vertexPtr, vertexSize, _freeMeshMemory, NULL);
  bcfx_MemBuffer* imb = luaL_newmembuffer(L);
  MEMBUFFER_SET(imb, indexPtr, indexSize, _freeMeshMemory, NULL);

  return 2;
}

#define EMPLACE_MESH_FUNCTION(name) \
  { #name, MESH_FUNCTION(name) }
static const luaL_Reg MESH_FUNCTION(funcs)[] = {
    // EMPLACE_MESH_FUNCTION(PackMeshParseParam),
    // EMPLACE_MESH_FUNCTION(UnpackMeshParseResult),
    EMPLACE_MESH_FUNCTION(MeshParse),
    {NULL, NULL},
};

#define REGISTE_FUNC_MESH(name) \
  REGISTE_LIGHTUSERDATA(name, MESH_FUNCTION(name))

void MESH_FUNCTION(init)(lua_State* L) {
  luaL_newlib(L, MESH_FUNCTION(funcs));

  //   REGISTE_FUNC_MESH(MeshParsePtr);

  lua_setfield(L, -2, "mesh");
}
