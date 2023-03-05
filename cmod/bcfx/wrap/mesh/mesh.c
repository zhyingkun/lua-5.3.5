#define _mesh_c_
#include <bcfx_wrap.h>
#include <objloader.h>

/* }====================================================== */

/*
** {======================================================
** MeshParse
** =======================================================
*/

static void _releaseMeshMemory(const luaL_MemBuffer* mb) {
  free(mb->ptr);
}
static void luaL_pushmesh(lua_State* L, Mesh* mesh) {
  void* vertexPtr = (void*)mesh->vertexArray.arr;
  mesh->vertexArray.arr = NULL; // move array data, not copy reference
  size_t vertexSize = mesh->vertexArray.num * sizeof(Vertex);
  void* indexPtr = (void*)mesh->indexArray.arr;
  mesh->indexArray.arr = NULL;
  size_t indexSize = mesh->indexArray.num * sizeof(uint32_t);

  lua_createtable(L, 0, 2);
  luaL_MemBuffer* vmb = luaL_newmembuffer(L);
  MEMBUFFER_SETREPLACE(vmb, vertexPtr, vertexSize, _releaseMeshMemory, NULL);
  lua_setfield(L, -2, "vertex");
  luaL_MemBuffer* imb = luaL_newmembuffer(L);
  MEMBUFFER_SETREPLACE(imb, indexPtr, indexSize, _releaseMeshMemory, NULL);
  lua_setfield(L, -2, "index");
}

static int MESH_FUNCTION(packMeshParseParam)(lua_State* L) {
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);

  luaL_MemBuffer* membuf = (luaL_MemBuffer*)malloc(sizeof(luaL_MemBuffer));
  MEMBUFFER_MOVEINIT(mb, membuf);

  lua_pushlightuserdata(L, (void*)membuf);
  return 1;
}
static void* MESH_FUNCTION(meshParsePtr)(void* arg) {
  luaL_MemBuffer* mb = (luaL_MemBuffer*)arg;

  MeshLoaded* loaded = (MeshLoaded*)malloc(sizeof(MeshLoaded));
  *loaded = objloader_loadMesh(mb->ptr, mb->sz);
  MEMBUFFER_RELEASE(mb);
  free((void*)mb);

  return (void*)loaded;
}
static int _dealMeshParseReult(lua_State* L, MeshLoaded* loaded) {
  MeshArray* ma = &loaded->ma;
  lua_createtable(L, ma->num, 0);
  for (uint32_t i = 0; i < ma->num; i++) {
    luaL_pushmesh(L, &ma->arr[i]);
    lua_rawseti(L, -2, i + 1);
  }

  MaterialLibraryArray* mla = &loaded->mla;
  lua_createtable(L, mla->num, 0);
  for (uint32_t i = 0; i < mla->num; i++) {
    lua_pushstring(L, mla->arr[i]);
    lua_rawseti(L, -2, i + 1);
  }

  objloader_destroyMesh(loaded);
  return 2;
}
static int MESH_FUNCTION(unpackMeshParseResult)(lua_State* L) {
  MeshLoaded* loaded = (MeshLoaded*)luaL_checklightuserdata(L, 1);
  int ret = _dealMeshParseReult(L, loaded);
  free((void*)loaded);
  return ret;
}
static int MESH_FUNCTION(meshParse)(lua_State* L) {
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);

  MeshLoaded meshLoaded = objloader_loadMesh(mb->ptr, mb->sz);
  MEMBUFFER_RELEASE(mb);

  return _dealMeshParseReult(L, &meshLoaded);
}

/* }====================================================== */

/*
** {======================================================
** MaterialParse
** =======================================================
*/

#define SET_MATERIAL_STR(field_) \
  lua_pushstring(L, mtl->field_); \
  lua_setfield(L, -2, #field_)
#define SET_MATERIAL_FLOAT3(field_) \
  do { \
    Vec3* vec = luaL_newvec3(L); \
    VEC3_X(vec) = mtl->field_.x; \
    VEC3_Y(vec) = mtl->field_.y; \
    VEC3_Z(vec) = mtl->field_.z; \
    lua_setfield(L, -2, #field_); \
  } while (0)
#define SET_MATERIAL_FLOAT(field_) \
  lua_pushnumber(L, mtl->field_); \
  lua_setfield(L, -2, #field_)
#define SET_MATERIAL_INT(field_) \
  lua_pushinteger(L, mtl->field_); \
  lua_setfield(L, -2, #field_)
static void luaL_pushmaterial(lua_State* L, Material* mtl) {
  lua_createtable(L, 0, 14);
  SET_MATERIAL_STR(name);
  SET_MATERIAL_FLOAT3(ka);
  SET_MATERIAL_FLOAT3(kd);
  SET_MATERIAL_FLOAT3(ks);
  SET_MATERIAL_FLOAT(ns);
  SET_MATERIAL_FLOAT(ni);
  SET_MATERIAL_FLOAT(d);
  SET_MATERIAL_INT(illum);
  SET_MATERIAL_STR(mapKa);
  SET_MATERIAL_STR(mapKd);
  SET_MATERIAL_STR(mapKs);
  SET_MATERIAL_STR(mapNs);
  SET_MATERIAL_STR(mapD);
  SET_MATERIAL_STR(mapBump);
}

static int MESH_FUNCTION(packMaterialParseParam)(lua_State* L) {
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);

  luaL_MemBuffer* membuf = (luaL_MemBuffer*)malloc(sizeof(luaL_MemBuffer));
  MEMBUFFER_MOVEINIT(mb, membuf);

  lua_pushlightuserdata(L, (void*)membuf);
  return 1;
}
static void* MESH_FUNCTION(materialParsePtr)(void* arg) {
  luaL_MemBuffer* mb = (luaL_MemBuffer*)arg;

  MaterialArray* mtla = (MaterialArray*)malloc(sizeof(MaterialArray));
  *mtla = objloader_loadMaterial(mb->ptr, mb->sz);
  MEMBUFFER_RELEASE(mb);
  free((void*)mb);

  return (void*)mtla;
}
static int _dealMaterialParseReult(lua_State* L, MaterialArray* mtla) {
  lua_createtable(L, mtla->num, 0);
  for (uint32_t i = 0; i < mtla->num; i++) {
    luaL_pushmaterial(L, &mtla->arr[i]);
    lua_rawseti(L, -2, i + 1);
  }
  objloader_destroyMaterial(mtla);
  return 1;
}
static int MESH_FUNCTION(unpackMaterialParseResult)(lua_State* L) {
  MaterialArray* mtla = (MaterialArray*)luaL_checklightuserdata(L, 1);
  int ret = _dealMaterialParseReult(L, mtla);
  free((void*)mtla);
  return ret;
}
static int MESH_FUNCTION(materialParse)(lua_State* L) {
  luaL_MemBuffer* mb = luaL_checkmembuffer(L, 1);

  MaterialArray materialArray = objloader_loadMaterial(mb->ptr, mb->sz);
  MEMBUFFER_RELEASE(mb);

  return _dealMaterialParseReult(L, &materialArray);
}

/* }====================================================== */

#define EMPLACE_MESH_FUNCTION(name) \
  { "" #name, MESH_FUNCTION(name) }
static const luaL_Reg MESH_FUNCTION(funcs)[] = {
    EMPLACE_MESH_FUNCTION(packMeshParseParam),
    EMPLACE_MESH_FUNCTION(unpackMeshParseResult),
    EMPLACE_MESH_FUNCTION(meshParse),
    EMPLACE_MESH_FUNCTION(packMaterialParseParam),
    EMPLACE_MESH_FUNCTION(unpackMaterialParseResult),
    EMPLACE_MESH_FUNCTION(materialParse),
    {NULL, NULL},
};

#define REGISTER_FUNC_MESH(name) \
  REGISTER_LIGHTUSERDATA(name, MESH_FUNCTION(name))

void MESH_FUNCTION(init)(lua_State* L) {
  luaL_newlib(L, MESH_FUNCTION(funcs));

  REGISTER_FUNC_MESH(meshParsePtr);
  REGISTER_FUNC_MESH(materialParsePtr);

  lua_setfield(L, -2, "mesh");
}
