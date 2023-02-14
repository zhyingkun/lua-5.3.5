#define _mesh_c_
#include <common.h>

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
  MEMBUFFER_SETREPLACE(mb, Vertex, sizeof(Vertex), NULL, NULL);
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
  MEMBUFFER_SETREPLACE(mb, Vertex, sizeof(Vertex), NULL, NULL);
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
#define SET_BUFFER2(a, b) *(buffer++) = a, *(buffer++) = b
#define SET_BUFFER3(a, b, c) *(buffer++) = a, *(buffer++) = b, *(buffer++) = c
#define SET_VECTOR3(face, u, v) \
  do { \
    switch (axis) { \
      case X: \
        SET_BUFFER3(face, posU, posV); \
        break; \
      case Y: \
        SET_BUFFER3(posU, face, posV); \
        break; \
      case Z: \
        SET_BUFFER3(posU, posV, face); \
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
  MEMBUFFER_SETREPLACE(mb, Vertex, sizeof(Vertex), NULL, NULL);
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
    {PT_Triangles, NULL, NULL}, // Circle
    {PT_Triangles, NULL, NULL}, // Tetrahedron
    {PT_Triangles, cube_makeVertexBuffer, NULL}, // Cube
    {PT_Triangles, NULL, NULL}, // Sphere
    {PT_Triangles, NULL, NULL}, // Cylinder
    {PT_Triangles, NULL, NULL}, // Cone
    {PT_Default, NULL, NULL}, // Cone
};

bcfx_BuiltinMesh builtin_createMesh(bcfx_EBuiltinMeshType type) {
  const MeshConfig* config = &BuiltinMeshList[type];

  bcfx_BuiltinMesh mesh = {0};
  mesh.primitive = config->primitive;
  if (config->makeVertexBuffer) {
    luaL_MemBuffer VertexBuffer = {0};
    bcfx_VertexLayout VertexLayout = {0};
    config->makeVertexBuffer(&VertexBuffer, &VertexLayout);
    mesh.vertex = bcfx_createVertexBuffer(&VertexBuffer, &VertexLayout);
  }
  if (config->makeIndexBuffer) {
    luaL_MemBuffer IndexBuffer = {0};
    bcfx_EIndexType type = config->makeIndexBuffer(&IndexBuffer);
    mesh.index = bcfx_createIndexBuffer(&IndexBuffer, type);
  }

  return mesh;
}
