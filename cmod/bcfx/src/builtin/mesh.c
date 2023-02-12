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

  // WorldSpace: +x to right, +y to front, +z to top
  // clang-format off
  float Vertex[] = {
    // Position         Normal          TexCoord(uv) Color
      -1.0, -1.0, 0.0,  0.0, 0.0, 1.0,  0.0, 0.0,    1.0, 0.0, 0.0,
       1.0, -1.0, 0.0,  0.0, 0.0, 1.0,  1.0, 0.0,    0.0, 1.0, 0.0,
       0.5,  1.0, 0.0,  0.0, 0.0, 1.0,  0.5, 1.0,    0.0, 0.0, 1.0,
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

  // WorldSpace: +x to right, +y to front, +z to top
  // clang-format off
  float Vertex[] = {
    // Position         Normal          TexCoord(uv)
      -1.0, -1.0, 0.0,  0.0, 0.0, 1.0,  0.0, 0.0,
       1.0, -1.0, 0.0,  0.0, 0.0, 1.0,  1.0, 0.0,
       1.0,  1.0, 0.0,  0.0, 0.0, 1.0,  1.0, 1.0,
      -1.0, -1.0, 0.0,  0.0, 0.0, 1.0,  0.0, 0.0,
       1.0,  1.0, 0.0,  0.0, 0.0, 1.0,  1.0, 1.0,
      -1.0,  1.0, 0.0,  0.0, 0.0, 1.0,  0.0, 1.0,
  };
  // clang-format on
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
    {PT_Triangles, NULL, NULL}, // Cube
    {PT_Triangles, NULL, NULL}, // Sphere
    {PT_Triangles, NULL, NULL}, // Cylinder
    {PT_Triangles, NULL, NULL}, // Cone
    {PT_Default, NULL, NULL}, // Cone
};

bcfx_BuiltinMesh builtin_createMesh(bcfx_EBuiltinMeshType type) {
  const MeshConfig* config = &BuiltinMeshList[type];

  bcfx_BuiltinMesh mesh = {};
  mesh.primitive = config->primitive;
  if (config->makeVertexBuffer) {
    luaL_MemBuffer VertexBuffer = {};
    bcfx_VertexLayout VertexLayout = {};
    config->makeVertexBuffer(&VertexBuffer, &VertexLayout);
    mesh.vertex = bcfx_createVertexBuffer(&VertexBuffer, &VertexLayout);
  }
  if (config->makeIndexBuffer) {
    luaL_MemBuffer IndexBuffer = {};
    bcfx_EIndexType type = config->makeIndexBuffer(&IndexBuffer);
    mesh.index = bcfx_createIndexBuffer(&IndexBuffer, type);
  }

  return mesh;
}
