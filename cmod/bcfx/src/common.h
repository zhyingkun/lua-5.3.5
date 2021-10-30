#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#ifdef _WIN32
#include <malloc.h>
#define alloca _alloca
#else
#include <alloca.h>
#endif

#define BCFX_LIB

#include <bcfx.h>

// max bits is 12
#define BCFX_CONFIG_VIEW_ID_BITS 8
#define BCFX_CONFIG_PROGRAM_BITS 9

// max number of render resource is 4096
// because current implementation using 12 bits for handle index
#define BCFX_CONFIG_MAX_VERTEX_LAYOUT 64
#define BCFX_CONFIG_MAX_VERTEX_BUFFER 4096
#define BCFX_CONFIG_MAX_INDEX_BUFFER 4096
#define BCFX_CONFIG_MAX_SHADER 512
#define BCFX_CONFIG_MAX_PROGRAM (1 << BCFX_CONFIG_PROGRAM_BITS)
#define BCFX_CONFIG_MAX_UNIFORM 512
#define BCFX_CONFIG_MAX_TEXTURE 4096
#define BCFX_CONFIG_MAX_FRAME_BUFFER 128
#define BCFX_CONFIG_MAX_OCCLUSION_QUERY 256
#define BCFX_CONFIG_MAX_DYNAMIC_INDEX_BUFFER 4096
#define BCFX_CONFIG_MAX_DYNAMIC_VERTEX_BUFFER 4096

#define BCFX_CONFIG_MAX_VIEWS (1 << BCFX_CONFIG_VIEW_ID_BITS)
#define BCFX_CONFIG_MAX_DRAW_CALLS 65536
#define BCFX_CONFIG_MAX_VERTEX_STREAMS 4

#define BCFX_CONFIG_MAX_WINDOW 32

#define CHECK_VIEWID(id) assert(id < BCFX_CONFIG_MAX_VIEWS)
#define CHECK_STREAMID(id) assert(id < BCFX_CONFIG_MAX_VERTEX_STREAMS)

#define printf_err(...) (fprintf(stderr, ##__VA_ARGS__), fflush(stderr))

#define BCFX_RESOURCE_MAP(XX) \
  XX(VertexLayout, BCFX_CONFIG_MAX_VERTEX_LAYOUT) \
  XX(VertexBuffer, BCFX_CONFIG_MAX_VERTEX_BUFFER) \
  XX(IndexBuffer, BCFX_CONFIG_MAX_INDEX_BUFFER) \
  XX(Shader, BCFX_CONFIG_MAX_SHADER) \
  XX(Program, BCFX_CONFIG_MAX_PROGRAM) \
  XX(Uniform, BCFX_CONFIG_MAX_UNIFORM) \
  XX(Texture, BCFX_CONFIG_MAX_TEXTURE)
/*
  XX(FrameBuffer, BCFX_CONFIG_MAX_FRAME_BUFFER) \
  XX(OcclusionQuery, BCFX_CONFIG_MAX_OCCLUSION_QUERY) \
  XX(DynamicIndexBuffer, BCFX_CONFIG_MAX_DYNAMIC_INDEX_BUFFER) \
  XX(DynamicVertexBuffer, BCFX_CONFIG_MAX_DYNAMIC_VERTEX_BUFFER)
*/

typedef enum {
  UB_ViewRect,
  UB_ViewTexel,
  UB_View,
  UB_InvView,
  UB_Proj,
  UB_InvProj,
  UB_ViewProj,
  UB_InvViewProj,
  UB_Model,
  UB_ModelView,
  UB_ModelViewProj,
  UB_AlphaRef,
  UB_Count,
} bcfx_EUniformBuiltin;

#endif /* _COMMON_H_ */
