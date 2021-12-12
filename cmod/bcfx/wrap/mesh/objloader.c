#define _objloader_c_

#include <objloader.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
** {======================================================
** Private API
** =======================================================
*/

void array_init(Array* arr, uint32_t max, size_t itemSize) {
  arr->num = 0;
  arr->max = max;
  arr->arr = malloc(itemSize * arr->max);
}
void* array_add(Array* arr, size_t itemSize) {
  if (arr->num == arr->max) {
    arr->max *= 2;
    arr->arr = realloc(arr->arr, itemSize * arr->max);
  }
  void* ptr = (void*)(((char*)(arr->arr)) + itemSize * arr->num);
  arr->num++;
  return ptr;
}
void array_clear(Array* arr) {
  arr->num = 0;
}
void array_destroy(Array* arr) {
  arr->num = 0;
  arr->max = 0;
  free((void*)arr->arr);
  arr->arr = NULL;
}

void f3_crossProduct(Float3* src1, Float3* src2, Float3* dst) {
  dst->x = src1->y * src2->z - src2->y * src1->z;
  dst->y = src1->z * src2->x - src2->z * src1->x;
  dst->z = src1->x * src2->y - src2->x * src1->y;
}
void f3_subtract(Float3* src1, Float3* src2, Float3* dst) {
  dst->x = src1->x - src2->x;
  dst->y = src1->y - src2->y;
  dst->z = src1->z - src2->z;
}

void f3a_init(Float3Array* f3a) {
  array_init((Array*)f3a, 1024, sizeof(Float3));
}
void f3a_add(Float3Array* f3a, const Float3* f3) {
  Float3* item = (Float3*)array_add((Array*)f3a, sizeof(Float3));
  *item = *f3;
}
void f3a_clear(Float3Array* f3a) {
  array_clear((Array*)f3a);
}
void f3a_destroy(Float3Array* f3a) {
  array_destroy((Array*)f3a);
}

void f2a_init(Float2Array* f2a) {
  array_init((Array*)f2a, 1024, sizeof(Float2));
}
void f2a_add(Float2Array* f2a, const Float2* f2) {
  Float2* item = (Float2*)array_add((Array*)f2a, sizeof(Float2));
  *item = *f2;
}
void f2a_clear(Float2Array* f2a) {
  array_clear((Array*)f2a);
}
void f2a_destroy(Float2Array* f2a) {
  array_destroy((Array*)f2a);
}

void va_init(VertexArray* va) {
  array_init((Array*)va, 128, sizeof(Vertex));
}
void va_add(VertexArray* va, Vertex* v) {
  Vertex* item = (Vertex*)array_add((Array*)va, sizeof(Vertex));
  *item = *v;
}
Vertex* va_addUninitialized(VertexArray* va) {
  return (Vertex*)array_add((Array*)va, sizeof(Vertex));
}
void va_clear(VertexArray* va) {
  array_clear((Array*)va);
}
void va_destroy(VertexArray* va) {
  array_destroy((Array*)va);
}

void ia_init(IndexArray* ia) {
  array_init((Array*)ia, 128, sizeof(uint32_t));
}
void ia_add(IndexArray* ia, uint32_t i) {
  uint32_t* item = (uint32_t*)array_add((Array*)ia, sizeof(uint32_t));
  *item = i;
}
void ia_clear(VertexArray* ia) {
  array_clear((Array*)ia);
}
void ia_destroy(IndexArray* ia) {
  array_destroy((Array*)ia);
}

void mesh_init(Mesh* mesh) {
  mesh->name = NULL;
  va_init(&mesh->vertexArray);
  ia_init(&mesh->indexArray);
  mesh->materialName = NULL;
}
void mesh_destroy(Mesh* mesh) {
  free((void*)mesh->name);
  va_destroy(&mesh->vertexArray);
  ia_destroy(&mesh->indexArray);
  free((void*)mesh->materialName);
}

void ma_init(MeshArray* ma) {
  array_init((Array*)ma, 4, sizeof(Mesh));
}
Mesh* ma_addUninitialized(MeshArray* ma) {
  return (Mesh*)array_add((Array*)ma, sizeof(Mesh));
}
void ma_destroy(MeshArray* ma) {
  for (uint32_t i = 0; i < ma->num; i++) {
    mesh_destroy(&ma->arr[i]);
  }
  array_destroy((Array*)ma);
}

void mla_init(MaterialLibraryArray* mla) {
  array_init((Array*)mla, 1, sizeof(const char*));
}
void mla_add(MaterialLibraryArray* mla, const char* mlName) {
  const char** item = (const char**)array_add((Array*)mla, sizeof(const char*));
  *item = mlName; // material library name
}
void mla_destroy(MaterialLibraryArray* mla) {
  for (uint32_t i = 0; i < mla->num; i++) {
    free((void*)mla->arr[i]);
  }
  array_destroy((Array*)mla);
}

void material_init(Material* m) {
}
void material_destroy(Material* m) {
}
void mtla_init(MaterialArray* ma) {
}
Material* mtla_addUninitialized(MaterialArray* ma) {
  return NULL;
}
void mtla_destroy(MaterialArray* ma) {
}

size_t _getLine(const char* buf, size_t sz) {
  for (size_t i = 0; i < sz; i++) {
    if (buf[i] == '\r' || buf[i] == '\n') {
      return i;
    }
  }
  return sz;
}

#define MAX_TOKEN 8

// maybe empty string in return list
int _split(const char* buf, size_t sz, char c, const char* list[], size_t count[]) {
  int idx = 0;
  count[idx] = 0;
  list[idx] = buf;
  for (size_t i = 0; i < sz; i++) {
    if (buf[i] == c) {
      if (i < sz - 1) {
        if (idx >= MAX_TOKEN) {
          // error;
        }
        idx++;
        count[idx] = 0;
      }
      list[idx] = buf + i + 1;
    } else {
      count[idx]++;
    }
  }
  idx++;
  return idx;
}

// no empty string in return list
int _splitSpace(const char* buf, size_t sz, const char* list[], size_t count[]) {
  int idx = 0;
  list[idx] = buf;
  count[idx] = 0;
  for (size_t i = 0; i < sz; i++) {
    switch (buf[i]) {
      case ' ':
      case '\f':
      case '\t':
      case '\v':
        if (i < sz - 1 && count[idx] != 0) {
          if (idx >= MAX_TOKEN) {
            // error;
          }
          idx++;
          count[idx] = 0;
        }
        list[idx] = buf + i + 1;
        break;
      default:
        count[idx]++;
        break;
    }
  }
  if (count[idx] != 0) {
    idx++;
  }
  return idx;
}

// token must end with '\0'
int _isToken(const char* ptr, size_t sz, const char* token) {
  for (size_t i = 0; i < sz; i++) {
    if (ptr[i] != token[i]) {
      return 0;
    }
  }
  return 1;
}

#define IS_TOKEN(list_, count_, num_, idx_, token_) (idx_ < num_ && _isToken(list_[0], count_[0], token_))
#define IS_FIRST_TOKEN(token_) IS_TOKEN(strList, szList, num, 0, token_)

float _stringToFloat(const char* str, size_t sz) {
  // char* temp = (char*)alloca(sz + 1);
  // memcpy(temp, str, sz);
  // temp[sz] = '\0';
  // return atof(temp);
  return atof(str);
}

int _stringToInt(const char* str, size_t sz) {
  return atoi(str);
}

Float3* _getElementF3(Float3Array* f3a, int idx) {
  idx = idx % f3a->num;
  return &f3a->arr[idx];
}
Float2* _getElementF2(Float2Array* f2a, int idx) {
  idx = idx % f2a->num;
  return &f2a->arr[idx];
}
#define GET_INDEX(n) (_stringToInt(strList[n], szList[n]) - 1)

#define FACE_MAX_VERTEX 4

int _parseOneVertex(ObjLoader* loader, const char* buf, size_t sz) {
  const char* strList[MAX_TOKEN];
  size_t szList[MAX_TOKEN];
  int num = _split(buf, sz, '/', strList, szList);
  assert(num > 0);
  Vertex* vert = va_addUninitialized(&loader->curMesh->vertexArray);
  vert->position = *_getElementF3(loader->positions, GET_INDEX(0));
  switch (num) {
    case 1: // only position, v1
      vert->texCoord.x = 0.0;
      vert->texCoord.y = 0.0;
      return 1;
    case 2: // position & texture, v1/vt1
      vert->texCoord = *_getElementF2(loader->texCoords, GET_INDEX(1));
      return 1;
    case 3:
      if (szList[1] == 0) { // position & normal, v1//vn1
        vert->texCoord.x = 0.0;
        vert->texCoord.y = 0.0;
      } else { // position & texture & normal, v1/vt1/vn1
        vert->texCoord = *_getElementF2(loader->texCoords, GET_INDEX(1));
      }
      vert->normal = *_getElementF3(loader->normals, GET_INDEX(2));
      return 0;
  }
  return FACE_MAX_VERTEX + 1;
}

#define ATOF_STRLIST(idx) _stringToFloat(strList[idx], szList[idx])

void _parseOneLine(ObjLoader* loader, const char* buf, size_t sz) {
  const char* strList[MAX_TOKEN];
  size_t szList[MAX_TOKEN];
  int num = _splitSpace(buf, sz, strList, szList);

  if (IS_FIRST_TOKEN("o") || IS_FIRST_TOKEN("g")) {
    loader->curMesh = ma_addUninitialized(loader->ma);
    mesh_init(loader->curMesh);
    if (num > 1) {
      loader->curMesh->name = strndup(strList[1], szList[1]);
    }
  } else if (IS_FIRST_TOKEN("v")) {
    Float3 f3[1];
    f3->x = ATOF_STRLIST(1);
    f3->y = ATOF_STRLIST(2);
    f3->z = ATOF_STRLIST(3);
    f3a_add(loader->positions, f3);
  } else if (IS_FIRST_TOKEN("vt")) {
    Float2 f2[1];
    f2->x = ATOF_STRLIST(1);
    f2->y = ATOF_STRLIST(2);
    f2a_add(loader->texCoords, f2);
  } else if (IS_FIRST_TOKEN("vn")) {
    Float3 f3[1];
    f3->x = ATOF_STRLIST(1);
    f3->y = ATOF_STRLIST(2);
    f3->z = ATOF_STRLIST(3);
    f3a_add(loader->normals, f3);
  } else if (IS_FIRST_TOKEN("f")) {
    assert(num - 1 >= 3);
    if (loader->curMesh == NULL) {
      loader->curMesh = ma_addUninitialized(loader->ma);
      mesh_init(loader->curMesh);
    }
    Mesh* curMesh = loader->curMesh;
    int cnt = 0;
    uint32_t start = curMesh->vertexArray.num;
    for (int i = 1; i < num; i++) {
      cnt += _parseOneVertex(loader, strList[i], szList[i]);
    }
    if (cnt == num - 1) {
      Float3 a[1], b[1], norm[1];
      Vertex* arr = curMesh->vertexArray.arr;
      f3_subtract(&arr[start + 0].position, &arr[start + 1].position, a);
      f3_subtract(&arr[start + 2].position, &arr[start + 1].position, b);
      f3_crossProduct(a, b, norm);
      for (int i = 0; i < cnt; i++) {
        arr[start + i].normal = norm[0];
      }
    } else {
      assert(cnt == 0);
      cnt = num - 1;
    }
    if (cnt == 3) {
      ia_add(&curMesh->indexArray, start + 0);
      ia_add(&curMesh->indexArray, start + 1);
      ia_add(&curMesh->indexArray, start + 2);
    } else if (cnt == 4) {
      ia_add(&curMesh->indexArray, start + 0);
      ia_add(&curMesh->indexArray, start + 1);
      ia_add(&curMesh->indexArray, start + 3);
      ia_add(&curMesh->indexArray, start + 1);
      ia_add(&curMesh->indexArray, start + 2);
      ia_add(&curMesh->indexArray, start + 3);
    } else {
      assert(0); // support triangle and quadrangulate only
    }
  } else if (IS_FIRST_TOKEN("usemtl")) {
    loader->curMesh->materialName = (const char*)strndup(strList[1], szList[1]);
  } else if (IS_FIRST_TOKEN("mtllib")) {
    mla_add(loader->mla, (const char*)strndup(strList[1], szList[1]));
  }
}

/* }====================================================== */

/*
** {======================================================
** Public API
** =======================================================
*/

MeshLoaded objloader_loadMesh(const char* buf, size_t sz) {
  ObjLoader loader[1];
  ma_init(loader->ma);
  mla_init(loader->mla);
  loader->curMesh = NULL;
  f3a_init(loader->positions);
  f2a_init(loader->texCoords);
  f3a_init(loader->normals);

  while (sz > 0) {
    size_t len = _getLine(buf, sz);
    if (len > 0 && buf[0] != '#') {
      _parseOneLine(loader, buf, len);
    }
    len++; // include '\n'
    sz -= len;
    buf += len;
  }

  loader->curMesh = NULL;
  f3a_destroy(loader->positions);
  f2a_destroy(loader->texCoords);
  f3a_destroy(loader->normals);

  MeshLoaded loaded;
  loaded.ma = loader->ma[0];
  loaded.mla = loader->mla[0];
  return loaded;
}

void objloader_destroyMesh(MeshLoaded* ml) {
  ma_destroy(&ml->ma);
  mla_destroy(&ml->mla);
}

MaterialArray objloader_loadMaterial(const char* buf, size_t sz) {
  MaterialArray ma = {0};
  return ma;
}
void objloader_destroyMaterial(MaterialArray* ma) {
  mtla_destroy(ma);
}

/* }====================================================== */
