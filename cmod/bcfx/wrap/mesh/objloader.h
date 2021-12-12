#ifndef _OBJLOADER_H_
#define _OBJLOADER_H_

#include <stdint.h>
#include <stddef.h>

/*
** {======================================================
** Private API
** =======================================================
*/

typedef struct {
  void* arr;
  uint32_t num;
  uint32_t max;
} Array;
void array_init(Array* arr, uint32_t max, size_t itemSize);
void* array_add(Array* arr, size_t itemSize);
void array_clear(Array* arr);
void array_destroy(Array* arr);

typedef struct {
  float x;
  float y;
  float z;
} Float3;
void f3_crossProduct(Float3* src1, Float3* src2, Float3* dst);
void f3_subtract(Float3* src1, Float3* src2, Float3* dst);

typedef struct {
  Float3* arr;
  uint32_t num; // fill num
  uint32_t max; // slot num
} Float3Array;
void f3a_init(Float3Array* f3a);
void f3a_add(Float3Array* f3a, const Float3* f3);
void f3a_clear(Float3Array* f3a);
void f3a_destroy(Float3Array* f3a);

typedef struct {
  float x;
  float y;
} Float2;
typedef struct {
  Float2* arr;
  uint32_t num; // fill num
  uint32_t max; // slot num
} Float2Array;
void f2a_init(Float2Array* f2a);
void f2a_add(Float2Array* f2a, const Float2* f2);
void f2a_clear(Float2Array* f2a);
void f2a_destroy(Float2Array* f2a);

typedef struct {
  Float3 position;
  Float3 normal;
  Float2 texCoord;
} Vertex;
typedef struct {
  Vertex* arr;
  uint32_t num;
  uint32_t max;
} VertexArray;
void va_init(VertexArray* va);
void va_add(VertexArray* va, Vertex* v);
Vertex* va_addUninitialized(VertexArray* va);
void va_clear(VertexArray* va);
void va_destroy(VertexArray* va);

typedef struct {
  uint32_t* arr;
  uint32_t num;
  uint32_t max;
} IndexArray;
void ia_init(IndexArray* ia);
void ia_add(IndexArray* ia, uint32_t i);
void ia_clear(VertexArray* ia);
void ia_destroy(IndexArray* ia);

typedef struct {
  const char* name;
  VertexArray vertexArray;
  IndexArray indexArray;
  const char* materialName;
} Mesh;
void mesh_init(Mesh* mesh);
void mesh_destroy(Mesh* mesh);

typedef struct {
  Mesh* arr;
  uint32_t num;
  uint32_t max;
} MeshArray;
void ma_init(MeshArray* ma);
Mesh* ma_addUninitialized(MeshArray* ma);
void ma_destroy(MeshArray* ma);

typedef struct {
  const char** arr;
  uint32_t num;
  uint32_t max;
} MaterialLibraryArray;
void mla_init(MaterialLibraryArray* mla);
void mla_add(MaterialLibraryArray* mla, const char* ml);
void mla_destroy(MaterialLibraryArray* mla);

typedef struct {
  const char* name;
  Float3 ka;
  Float3 kd;
  Float3 ks;
  float ns;
  float ni;
  float d;
  int illum;
  const char* mapKa;
  const char* mapKd;
  const char* mapKs;
  const char* mapNs;
  const char* mapD;
  const char* mapBump;
} Material;
void material_init(Material* m);
void material_destroy(Material* m);

typedef struct {
  Material* arr;
  uint32_t num;
  uint32_t max;
} MaterialArray;
void mtla_init(MaterialArray* ma);
Material* mtla_addUninitialized(MaterialArray* ma);
void mtla_destroy(MaterialArray* ma);

typedef struct {
  MeshArray ma[1];
  MaterialLibraryArray mla[1];
  Mesh* curMesh;
  Float3Array positions[1];
  Float2Array texCoords[1];
  Float3Array normals[1];
} ObjLoader;

/* }====================================================== */

/*
** {======================================================
** Public API
** =======================================================
*/

typedef struct {
  MeshArray ma;
  MaterialLibraryArray mla;
} MeshLoaded;

MeshLoaded objloader_loadMesh(const char* buf, size_t sz);
void objloader_destroyMesh(MeshLoaded* ma);

MaterialArray objloader_loadMaterial(const char* buf, size_t sz);
void objloader_destroyMaterial(MaterialArray* ma);

/* }====================================================== */

#endif /* _OBJLOADER_H_ */
