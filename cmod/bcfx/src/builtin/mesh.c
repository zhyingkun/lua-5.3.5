#define _mesh_c_
#include <common.h>

#define SET_BUFFER2(a, b) *(buffer++) = a, *(buffer++) = b
#define SET_BUFFER3(a, b, c) *(buffer++) = a, *(buffer++) = b, *(buffer++) = c

/*
** {======================================================
** Triangle
** =======================================================
*/

void triangle_makeVertexBuffer(luaL_MemBuffer* mb, bcfx_VertexLayout* vl) {
  bcfx_vertexLayoutInit(vl);
  bcfx_vertexLayoutAddAttrib(vl, VA_Position, 3, AT_Float, false);
  bcfx_vertexLayoutAddAttrib(vl, VA_Normal, 3, AT_Float, false);
  bcfx_vertexLayoutAddAttrib(vl, VA_TexCoord0, 2, AT_Float, false);
  bcfx_vertexLayoutAddAttrib(vl, VA_Color0, 3, AT_Float, false);

  // WorldSpace: +x to right, +y to front, +z to top, Current LocalSpace equals WorldSpace
  // clang-format off
  static float Vertex[] = {
    // Position         Normal          TexCoord(uv) Color
      -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,    1.0f, 0.0f, 0.0f,
       1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,    0.0f, 1.0f, 0.0f,
       0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.5f, 1.0f,    0.0f, 0.0f, 1.0f,
  };
  // clang-format on
  MEMBUFFER_SETREPLACE_STATIC(mb, Vertex, sizeof(Vertex));
}

/* }====================================================== */

/*
** {======================================================
** Square
** =======================================================
*/

void square_makeVertexBuffer(luaL_MemBuffer* mb, bcfx_VertexLayout* vl) {
  bcfx_vertexLayoutInit(vl);
  bcfx_vertexLayoutAddAttrib(vl, VA_Position, 3, AT_Float, false);
  bcfx_vertexLayoutAddAttrib(vl, VA_Normal, 3, AT_Float, false);
  bcfx_vertexLayoutAddAttrib(vl, VA_TexCoord0, 2, AT_Float, false);

  // WorldSpace: +x to right, +y to front, +z to top, Current LocalSpace equals WorldSpace
  // clang-format off
  static float Vertex[] = {
    // Position         Normal          TexCoord(uv)
      -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
       1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
       1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
      -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
       1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
      -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
  };
  // clang-format on
  MEMBUFFER_SETREPLACE_STATIC(mb, Vertex, sizeof(Vertex));
}

/* }====================================================== */

/*
** {======================================================
** Circle
** =======================================================
*/

#define CirclePointCount 360
#define SET_ONE_VERTEX(x, y) \
  SET_BUFFER3((x), (y), 0.0f); \
  SET_BUFFER3(0.0f, 0.0f, 1.0f); \
  SET_BUFFER2(((x) + 1.0f) / 2.0f, ((y) + 1.0f) / 2.0f)
static void FillCircle(float* buffer) {
  SET_ONE_VERTEX(0.0f, 0.0f);
  for (int i = 0; i < CirclePointCount; i++) {
    float radian = 2.0f * M_PI * i / CirclePointCount;
    float x = sinf(radian);
    float y = cosf(radian);
    SET_ONE_VERTEX(x, y);
  }
}
void circle_makeVertexBuffer(luaL_MemBuffer* mb, bcfx_VertexLayout* vl) {
  bcfx_vertexLayoutInit(vl);
  bcfx_vertexLayoutAddAttrib(vl, VA_Position, 3, AT_Float, false);
  bcfx_vertexLayoutAddAttrib(vl, VA_Normal, 3, AT_Float, false);
  bcfx_vertexLayoutAddAttrib(vl, VA_TexCoord0, 2, AT_Float, false);

  // WorldSpace: +x to right, +y to front, +z to top, Current LocalSpace equals WorldSpace
  static float Vertex[(3 + 3 + 2) * 360];
  FillCircle(Vertex);
  MEMBUFFER_SETREPLACE_STATIC(mb, Vertex, sizeof(Vertex));
}

/* }====================================================== */

/*
** {======================================================
** Tetrahedron
** =======================================================
*/

#define SQRT3 1.7321f
#define SQRT6 2.4495f
#define SET_VEC3(vec) SET_BUFFER3(VEC3_X(vec), VEC3_Y(vec), VEC3_Z(vec))
// clang-format off
#define TetrahedronFace(p1, p2, p3) \
  do { \
    ALLOCA_VEC3(n); \
    ALLOCA_VEC3(s); \
    ALLOCA_VEC3(t); \
    VEC_SUBTRACT(p2, p1, s); \
    VEC_SUBTRACT(p3, p1, t); \
    VEC3_CROSS_PRODUCT(s, t, n); \
    VEC_NORMALIZE_(n); \
    SET_VEC3(p1); SET_VEC3(n); SET_BUFFER2(0.0f, 0.0f); \
    SET_VEC3(p2); SET_VEC3(n); SET_BUFFER2(1.0f, 0.0f); \
    SET_VEC3(p3); SET_VEC3(n); SET_BUFFER2(0.5f, 1.0f); \
  } while(0)
// clang-format on
static void FillTetrahedron(float* buffer) {
  /*
   *    b
   *    d(top)
   *  c    a
   */
  static Vec3 a[1] = {VEC3(1.0f, -SQRT3 / 3.0f, 0.0f)};
  static Vec3 b[1] = {VEC3(0.0f, SQRT3 * 2.0f / 3.0f, 0.0f)};
  static Vec3 c[1] = {VEC3(-1.0f, -SQRT3 / 3.0f, 0.0f)};
  static Vec3 d[1] = {VEC3(0.0f, 0.0f, SQRT6 * 2.0f / 3.0f)};
  TetrahedronFace(a, b, d);
  TetrahedronFace(b, c, d);
  TetrahedronFace(c, a, d);
  TetrahedronFace(c, b, a);
}
void tetrahedron_makeVertexBuffer(luaL_MemBuffer* mb, bcfx_VertexLayout* vl) {
  bcfx_vertexLayoutInit(vl);
  bcfx_vertexLayoutAddAttrib(vl, VA_Position, 3, AT_Float, false);
  bcfx_vertexLayoutAddAttrib(vl, VA_Normal, 3, AT_Float, false);
  bcfx_vertexLayoutAddAttrib(vl, VA_TexCoord0, 2, AT_Float, false);

  // WorldSpace: +x to right, +y to front, +z to top, LocalSpace equals WorldSpace
  static float Vertex[(3 + 3 + 2) * 3 * 4];
  FillTetrahedron(Vertex);
  MEMBUFFER_SETREPLACE_STATIC(mb, Vertex, sizeof(Vertex));
}

/* }====================================================== */

/*
** {======================================================
** Cube
** =======================================================
*/

typedef enum {
  X,
  Y,
  Z,
} Axis;
typedef enum {
  Positive,
  Negative,
} Sign;
typedef struct {
  Axis axis;
  Sign sign;
} CubeFace;
static CubeFace CubeFaceList[] = {
    {X, Positive},
    {X, Negative},
    {Y, Positive},
    {Y, Negative},
    {Z, Positive},
    {Z, Negative},
};
typedef struct {
  float posU;
  float posV;
  float u;
  float v;
} FacePointAttrib;
// clang-format off
static FacePointAttrib FacePointAttribList[] = {
    {-1.0f, -1.0f,  0.0f, 0.0f},
    { 1.0f, -1.0f,  1.0f, 0.0f},
    { 1.0f,  1.0f,  1.0f, 1.0f},
    {-1.0f, -1.0f,  0.0f, 0.0f},
    { 1.0f,  1.0f,  1.0f, 1.0f},
    {-1.0f,  1.0f,  0.0f, 1.0f},
};
// clang-format on
#define SET_VECTOR3(face, u, v) \
  do { \
    switch (axis) { \
      case X: \
        SET_BUFFER3(face, u, v); \
        break; \
      case Y: \
        SET_BUFFER3(u, face, v); \
        break; \
      case Z: \
        SET_BUFFER3(u, v, face); \
        break; \
    } \
  } while (0)
static float* FillFace(Axis axis, Sign sign, float* buffer) {
  float face = sign == Positive ? 1.0f : -1.0f;
  for (int i = 0; i < 6; i++) { // 6 vertex point per face
    float posU = face * FacePointAttribList[i].posU;
    float posV = face * FacePointAttribList[i].posV;
    SET_VECTOR3(face, posU, posV); // Position
    SET_VECTOR3(face, 0.0f, 0.0f); // Normal
    float u = FacePointAttribList[i].u;
    float v = FacePointAttribList[i].v;
    SET_BUFFER2(u, v);
  }
  return buffer;
}
static void FillCube(float* buffer) {
  for (int i = 0; i < 6; i++) { // 6 face per cube
    CubeFace cf = CubeFaceList[i];
    buffer = FillFace(cf.axis, cf.sign, buffer);
  }
}
void cube_makeVertexBuffer(luaL_MemBuffer* mb, bcfx_VertexLayout* vl) {
  bcfx_vertexLayoutInit(vl);
  bcfx_vertexLayoutAddAttrib(vl, VA_Position, 3, AT_Float, false);
  bcfx_vertexLayoutAddAttrib(vl, VA_Normal, 3, AT_Float, false);
  bcfx_vertexLayoutAddAttrib(vl, VA_TexCoord0, 2, AT_Float, false);

  // WorldSpace: +x to right, +y to front, +z to top, Current LocalSpace equals WorldSpace
  static float Vertex[(3 + 3 + 2) * 6 * 6];
  FillCube(Vertex);
  MEMBUFFER_SETREPLACE_STATIC(mb, Vertex, sizeof(Vertex));
}

/* }====================================================== */

typedef void (*MakeMeshVertexBuffer)(luaL_MemBuffer* mb, bcfx_VertexLayout* vl);
typedef bcfx_EIndexType (*MakeMeshIndexBuffer)(luaL_MemBuffer* mb);
typedef struct {
  bcfx_EPrimitiveType primitive;
  MakeMeshVertexBuffer makeVertexBuffer;
  MakeMeshIndexBuffer makeIndexBuffer;
} MeshConfig;

const MeshConfig BuiltinMeshList[] = {
    {PT_Triangles, triangle_makeVertexBuffer, NULL}, // Triangle
    {PT_Triangles, square_makeVertexBuffer, NULL}, // Square
    {PT_TriangleFan, circle_makeVertexBuffer, NULL}, // Circle
    {PT_Triangles, tetrahedron_makeVertexBuffer, NULL}, // Tetrahedron
    {PT_Triangles, cube_makeVertexBuffer, NULL}, // Cube
    {PT_Triangles, NULL, NULL}, // Sphere
    {PT_Triangles, NULL, NULL}, // Cylinder
    {PT_Triangles, NULL, NULL}, // Cone
    {PT_Default, NULL, NULL}, // None
};

bcfx_BuiltinMesh builtin_createMesh(bcfx_EBuiltinMeshType type) {
  const MeshConfig* config = &BuiltinMeshList[type];

  bcfx_BuiltinMesh mesh = {0};
  mesh.primitive = config->primitive;
  if (config->makeVertexBuffer) {
    luaL_MemBuffer VertexBuffer = MEMBUFFER_NULL;
    bcfx_VertexLayout VertexLayout = {0};
    config->makeVertexBuffer(&VertexBuffer, &VertexLayout);
    mesh.vertex = bcfx_createVertexBuffer(&VertexBuffer, &VertexLayout);
  }
  if (config->makeIndexBuffer) {
    luaL_MemBuffer IndexBuffer = MEMBUFFER_NULL;
    bcfx_EIndexType type = config->makeIndexBuffer(&IndexBuffer);
    mesh.index = bcfx_createIndexBuffer(&IndexBuffer, type);
  }

  return mesh;
}
