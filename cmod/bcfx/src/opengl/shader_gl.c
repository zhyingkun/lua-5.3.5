#include <common_gl.h>
#include <ctype.h>

/*
** {======================================================
** Shader Attributes
** =======================================================
*/

static const char* glslTypeName(GLuint type) {
#define GLSL_TYPE(ty) \
  case ty: \
    return #ty
  switch (type) {
    GLSL_TYPE(GL_BOOL);
    GLSL_TYPE(GL_INT);
    GLSL_TYPE(GL_INT_VEC2);
    GLSL_TYPE(GL_INT_VEC3);
    GLSL_TYPE(GL_INT_VEC4);
    GLSL_TYPE(GL_UNSIGNED_INT);
    GLSL_TYPE(GL_UNSIGNED_INT_VEC2);
    GLSL_TYPE(GL_UNSIGNED_INT_VEC3);
    GLSL_TYPE(GL_UNSIGNED_INT_VEC4);
    GLSL_TYPE(GL_FLOAT);
    GLSL_TYPE(GL_FLOAT_VEC2);
    GLSL_TYPE(GL_FLOAT_VEC3);
    GLSL_TYPE(GL_FLOAT_VEC4);
    GLSL_TYPE(GL_FLOAT_MAT2);
    GLSL_TYPE(GL_FLOAT_MAT3);
    GLSL_TYPE(GL_FLOAT_MAT4);

    GLSL_TYPE(GL_SAMPLER_2D);
    GLSL_TYPE(GL_SAMPLER_2D_ARRAY);
    GLSL_TYPE(GL_SAMPLER_2D_MULTISAMPLE);

    GLSL_TYPE(GL_INT_SAMPLER_2D);
    GLSL_TYPE(GL_INT_SAMPLER_2D_ARRAY);
    GLSL_TYPE(GL_INT_SAMPLER_2D_MULTISAMPLE);

    GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_2D);
    GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_2D_ARRAY);
    GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE);

    GLSL_TYPE(GL_SAMPLER_2D_SHADOW);
    GLSL_TYPE(GL_SAMPLER_2D_ARRAY_SHADOW);

    GLSL_TYPE(GL_SAMPLER_3D);
    GLSL_TYPE(GL_INT_SAMPLER_3D);
    GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_3D);

    GLSL_TYPE(GL_SAMPLER_CUBE);
    GLSL_TYPE(GL_INT_SAMPLER_CUBE);
    GLSL_TYPE(GL_UNSIGNED_INT_SAMPLER_CUBE);

    GLSL_TYPE(GL_IMAGE_1D);
    GLSL_TYPE(GL_INT_IMAGE_1D);
    GLSL_TYPE(GL_UNSIGNED_INT_IMAGE_1D);

    GLSL_TYPE(GL_IMAGE_2D);
    GLSL_TYPE(GL_IMAGE_2D_ARRAY);
    GLSL_TYPE(GL_INT_IMAGE_2D);
    GLSL_TYPE(GL_UNSIGNED_INT_IMAGE_2D);

    GLSL_TYPE(GL_IMAGE_3D);
    GLSL_TYPE(GL_INT_IMAGE_3D);
    GLSL_TYPE(GL_UNSIGNED_INT_IMAGE_3D);

    GLSL_TYPE(GL_IMAGE_CUBE);
    GLSL_TYPE(GL_INT_IMAGE_CUBE);
    GLSL_TYPE(GL_UNSIGNED_INT_IMAGE_CUBE);
  }
#undef GLSL_TYPE
  return "<Unknown GLSL type?>";
}

static const char* attribNames[] = {
    "a_position",
    "a_normal",
    "a_tangent",
    "a_bitangent",
    "a_color0",
    "a_color1",
    "a_color2",
    "a_color3",
    "a_indices",
    "a_weight",
    "a_texcoord0",
    "a_texcoord1",
    "a_texcoord2",
    "a_texcoord3",
    "a_texcoord4",
    "a_texcoord5",
    "a_texcoord6",
    "a_texcoord7",
    NULL,
};
static bcfx_EVertexAttrib findVertexAttributeEnum(const char* name) {
  for (uint8_t i = 0; attribNames[i] != NULL; i++) {
    if (strcmp(attribNames[i], name) == 0) {
      return (bcfx_EVertexAttrib)i;
    }
  }
  return VA_Count;
}
static const char* instanceAttribNames[] = {
    "i_data0",
    "i_data1",
    "i_data2",
    "i_data3",
    "i_data4",
    NULL,
};
static int findInstanceAttributeIndex(const char* name) {
  for (int i = 0; instanceAttribNames[i] != NULL; i++) {
    if (strcmp(instanceAttribNames[i], name) == 0) {
      return i;
    }
  }
  return -1;
}
void prog_collectAttributes(ProgramGL* prog) {
  GLint activeAttribs = 0;
  GL_CHECK(glGetProgramiv(prog->id, GL_ACTIVE_ATTRIBUTES, &activeAttribs));
  GLint maxLength = 0;
  GL_CHECK(glGetProgramiv(prog->id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength));
  char* name = (char*)alloca(maxLength + 1);
  PredefinedAttrib* pa = &prog->pa;
  for (int i = 0; i < BCFX_CONFIG_MAX_INSTANCE_DATA; i++) {
    pa->instanceAttr[i] = -1;
  }
  uint8_t cnt = 0;
  for (GLint i = 0; i < activeAttribs; i++) {
    GLenum gltype;
    GLint num, loc;
    GL_CHECK(glGetActiveAttrib(prog->id, i, maxLength + 1, NULL, &num, &gltype, name));
    GL_CHECK(loc = glGetAttribLocation(prog->id, name));
    // printf_err("Attribute %s %s is at location %d\n", glslTypeName(gltype), name, glGetAttribLocation(prog->id, name));
    bcfx_EVertexAttrib eva = findVertexAttributeEnum(name);
    if (eva != VA_Count) {
      pa->used[cnt] = eva;
      pa->attributes[eva] = loc;
      cnt++;
    } else {
      int idx = findInstanceAttributeIndex(name);
      if (idx != -1) {
        pa->instanceAttr[idx] = loc;
      } else if (strstr(name, "gl_") != name) {
        // start with 'gl_' are OpenGL Builtin Attribute, such as gl_InstanceID
        printf_err("VertexAttribute %s %s is at location %d, size %d, Does Not Find In Pre Defined Attribute\n", glslTypeName(gltype), name, loc, num);
      }
    }
  }
  pa->usedCount = cnt;
  int emptyIdx = 0;
  for (int i = 0; i < BCFX_CONFIG_MAX_INSTANCE_DATA; i++) {
    if (pa->instanceAttr[i] != -1) {
      if (i > emptyIdx) {
        // Move attr to the empty one
        pa->instanceAttr[emptyIdx] = pa->instanceAttr[i];
        pa->instanceAttr[i] = -1;
      }
      emptyIdx++;
    }
  }
}

static bcfx_VertexLayout* find_vertexLayout(RendererContextGL* glCtx, RenderDraw* draw, bcfx_EVertexAttrib attr, VertexBufferGL** pvb) {
  bcfx_VertexLayout* target = NULL;
  uint8_t streamIdx = 0;
  for (uint8_t i = 0; i < BCFX_CONFIG_MAX_VERTEX_STREAMS; i++) {
    if (HAS_BIT(draw->streamMask, i)) {
      Stream* stream = &draw->streams[i];
      if (HAS_BIT(stream->attribMask, (uint8_t)attr)) {
        VertexBufferGL* vb = &glCtx->vertexBuffers[handle_index(stream->vertexBuffer)];
        bcfx_VertexLayout* layout = &vb->layout;
        if (layout->attributes[attr].num > 0) {
          if (target == NULL) {
            target = layout;
            *pvb = vb;
            streamIdx = i;
          } else {
            printf_err("Duplicate binding for VertexAttribute: %d, stream: %d, previous stream: %d\n", (uint8_t)attr, i, streamIdx);
          }
        } else if (stream->attribMask != VAM_All) {
          printf_err("AttribMask indicate VertexAttribute does not exist: %d, stream: %d\n", (uint8_t)attr, i);
        }
      }
    }
  }
  return target;
}
void gl_bindProgramAttributes(RendererContextGL* glCtx, ProgramGL* prog, RenderDraw* draw) {
  glCtx->curVertexCount = 0;
  GLuint curId = 0;
  PredefinedAttrib* pa = &prog->pa;
  for (uint8_t i = 0; i < pa->usedCount; i++) {
    bcfx_EVertexAttrib attr = (bcfx_EVertexAttrib)pa->used[i];
    GLint loc = pa->attributes[attr];
    VertexBufferGL* vb = NULL;
    bcfx_VertexLayout* layout = find_vertexLayout(glCtx, draw, attr, &vb);
    if (layout == NULL) {
      GL_CHECK(glDisableVertexAttribArray(loc));
    } else {
      if (attr == VA_Position) {
        // glCtx->curVertexCount = vb->count;
        glCtx->curVertexCount = (uint32_t)(vb->buffer.size / layout->stride);
      }
      if (curId != vb->buffer.id) {
        curId = vb->buffer.id;
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, curId)); // glVertexAttribPointer will save reference to this buffer
      }
      GL_CHECK(glEnableVertexAttribArray(loc));
      GL_CHECK(glVertexAttribDivisor(loc, 0)); // indicated it's per vertex, not per instance
      uint8_t offset = layout->offset[attr];
      bcfx_Attrib* attrib = &layout->attributes[attr];
      if (attrib->normal == 0 && (attrib->type == AT_Uint8 || attrib->type == AT_Int16)) {
        GL_CHECK(glVertexAttribIPointer(loc, attrib->num, attrib_glType[attrib->type], layout->stride, (void*)(long)offset));
      } else {
        GL_CHECK(glVertexAttribPointer(loc, attrib->num, attrib_glType[attrib->type], attrib->normal, layout->stride, (void*)(long)offset));
      }
    }
  }
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
void gl_bindInstanceAttributes(RendererContextGL* glCtx, ProgramGL* prog, RenderDraw* draw) {
  if (draw->instanceDataBuffer == kInvalidHandle) {
    return;
  }
  InstanceDataBufferGL* idb = &glCtx->instanceDataBuffers[handle_index(draw->instanceDataBuffer)];
  PredefinedAttrib* pa = &prog->pa;
  if (draw->startInstance >= idb->numInstance) {
    printf_err("Draw startInstance greater than buffer numInstance, startInstance: %d, numInstance: %d\n", draw->startInstance, idb->numInstance);
    for (uint8_t i = 0; pa->instanceAttr[i] != -1; i++) {
      GLint loc = pa->instanceAttr[i];
      GL_CHECK(glDisableVertexAttribArray(loc));
    }
    return;
  }
  if (draw->startInstance + draw->numInstance > idb->numInstance) {
    printf_err("Draw instance count greater than the rest buffer instance, startInstance: %d, numInstanceDraw: %d, numInstanceBuffer: %d\n", draw->startInstance, draw->numInstance, idb->numInstance);
  }
  const uint8_t numBytePerFloat = sizeof(GLfloat);
  const uint8_t numFloatPerVec4 = 4;
  const uint8_t numBytePerVec4 = numFloatPerVec4 * numBytePerFloat;
  GLsizei numBytePerInstance = idb->numVec4PerInstance * numBytePerVec4;
  GLsizei startByte = draw->startInstance * numBytePerInstance;
  for (uint8_t i = 0; pa->instanceAttr[i] != -1; i++) {
    GLint loc = pa->instanceAttr[i];
    if (i < idb->numVec4PerInstance) {
      GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, idb->buffer.id));
      GL_CHECK(glEnableVertexAttribArray(loc));
      GL_CHECK(glVertexAttribDivisor(loc, 1)); // indicated it's per instance, not per vertex
      void* offset = (void*)((long)startByte + numBytePerVec4 * i);
      GL_CHECK(glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, numBytePerInstance, offset));
    } else {
      GL_CHECK(glDisableVertexAttribArray(loc));
    }
  }
  GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

/* }====================================================== */

/*
** {======================================================
** Shader Uniforms
** =======================================================
*/

static const String uniformNames[] = {
    STRING_LITERAL("u_viewRect"),
    STRING_LITERAL("u_viewTexel"),
    STRING_LITERAL("u_view"),
    STRING_LITERAL("u_invView"),
    STRING_LITERAL("u_proj"),
    STRING_LITERAL("u_invProj"),
    STRING_LITERAL("u_viewProj"),
    STRING_LITERAL("u_invViewProj"),
    STRING_LITERAL("u_model"),
    STRING_LITERAL("u_modelView"),
    STRING_LITERAL("u_modelViewProj"),
    STRING_LITERAL("u_alphaRef4"),
    STRING_LITERAL_NULL(),
};
static bcfx_EUniformBuiltin findUniformBuiltinEnum(const String* name) {
  for (uint8_t i = 0; !str_isNull(&uniformNames[i]); i++) {
    if (str_isEqual(&uniformNames[i], name)) {
      return (bcfx_EUniformBuiltin)i;
    }
  }
  return UB_Count;
}
static uint16_t findUniformUserDefined(RendererContextGL* glCtx, const String* name) {
  uint16_t idx = 0;
  for (uint16_t i = 0; i < glCtx->uniformCount; i++, idx++) {
    UniformGL* uniform = &glCtx->uniforms[idx];
    while (uniform->name == NULL) {
      idx++;
      uniform = &glCtx->uniforms[idx];
    }
    if (str_isEqual(uniform->name, name)) {
      return idx;
    }
  }
  return UINT16_MAX;
}
#define SET_UNIFORM_PROPERTY(prop, loc_, num_, gltype_, idx_) \
  prop->loc = loc_; \
  prop->num = num_; \
  prop->type = gltype_; \
  prop->index = idx_
void prog_collectUniforms(ProgramGL* prog, RendererContextGL* glCtx) {
  GLint activeUniforms = 0;
  GL_CHECK(glGetProgramiv(prog->id, GL_ACTIVE_UNIFORMS, &activeUniforms));
  GLint maxLength = 0;
  GL_CHECK(glGetProgramiv(prog->id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength));
  char* name = (char*)alloca(maxLength + 1);
  uint8_t cnt = 0;
  uint16_t cntUD = 0;
  PredefinedUniform* pu = &prog->pu;
  for (GLint i = 0; i < activeUniforms; i++) {
    GLenum gltype;
    GLint num, loc;
    GL_CHECK(glGetActiveUniform(prog->id, i, maxLength + 1, NULL, &num, &gltype, name));
    GL_CHECK(loc = glGetUniformLocation(prog->id, name));
    // printf_err("Uniform %s %s is at location %d, size %d\n", glslTypeName(gltype), name, loc, num);
    String nameStr[1];
    nameStr->str = name;
    nameStr->sz = strlen(name);
    bcfx_EUniformBuiltin eub = findUniformBuiltinEnum(nameStr);
    if (eub != UB_Count) {
      assert(loc != -1);
      pu->used[cnt] = eub;
      cnt++;
      UniformProperty* prop = &pu->properties[eub];
      SET_UNIFORM_PROPERTY(prop, loc, num, gltype, 0);
    } else {
      uint16_t idx = findUniformUserDefined(glCtx, nameStr);
      if (idx != UINT16_MAX) {
        assert(loc != -1);
        UniformProperty* prop = &pu->propertiesUD[cntUD];
        cntUD++;
        SET_UNIFORM_PROPERTY(prop, loc, num, gltype, idx);
      } else {
        printf_err("Uniform %s %s is at location %d, size %d, Does Not Find In User Defined Uniform\n", glslTypeName(gltype), name, loc, num);
      }
    }
  }
  pu->usedCount = cnt;
  pu->usedCountUD = cntUD;
}

static void gl_bindTextureUnit(RendererContextGL* glCtx, RenderBind* bind, uint8_t stage) {
  Binding* b = &bind->binds[stage];
  if (b->handle != kInvalidHandle) {
    TextureGL* texture = &glCtx->textures[handle_index(b->handle)];
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + stage));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, texture->id));

    SamplerGL* sampler = &glCtx->samplers[handle_index(b->sampler)];
    GL_CHECK(glBindSampler(stage, sampler->id));
  } else {
    printf_err("Bind texture unit %d with invalid handle\n", stage);
  }
}
void gl_setProgramUniforms(RendererContextGL* glCtx, ProgramGL* prog, RenderDraw* draw, View* view, RenderBind* bind) {
  PredefinedUniform* pu = &prog->pu;
  for (uint8_t i = 0; i < pu->usedCount; i++) {
    bcfx_EUniformBuiltin eub = (bcfx_EUniformBuiltin)pu->used[i];
    UniformProperty* prop = &pu->properties[eub];
    GLint loc = prop->loc;
    switch (eub) {
      case UB_ViewRect: {
        float frect[4];
        frect[0] = view->rect.x;
        frect[1] = view->rect.y;
        frect[2] = view->rect.width;
        frect[3] = view->rect.height;
        GL_CHECK(glUniform4fv(loc, 1, frect));
      } break;
      case UB_ViewTexel: {
        float ftexel[4];
        ftexel[0] = 1.0 / (float)(view->rect.width);
        ftexel[1] = 1.0 / (float)(view->rect.height);
        ftexel[2] = 0.0;
        ftexel[3] = 0.0;
        GL_CHECK(glUniform4fv(loc, 1, ftexel));
      } break;
      case UB_View:
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, view->viewMat.element));
        break;
      case UB_InvView: {
        ALLOCA_MAT4x4(invView);
        MAT_INVERSE(&view->viewMat, invView);
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, invView->element));
      } break;
      case UB_Proj:
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, view->projMat.element));
        break;
      case UB_InvProj: {
        ALLOCA_MAT4x4(invProj);
        MAT_INVERSE(&view->projMat, invProj);
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, invProj->element));
      } break;
      case UB_ViewProj: {
        ALLOCA_MAT4x4(viewProj);
        MAT_MULTIPLY(&view->projMat, &view->viewMat, viewProj);
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, viewProj->element));
      } break;
      case UB_InvViewProj: {
        ALLOCA_MAT4x4(viewProj);
        MAT_MULTIPLY(&view->projMat, &view->viewMat, viewProj);
        ALLOCA_MAT4x4(invViewProj);
        MAT_INVERSE(viewProj, invViewProj);
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, invViewProj->element));
      } break;
      case UB_Model:
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, draw->model.element));
        break;
      case UB_ModelView: {
        ALLOCA_MAT4x4(modelView);
        MAT_MULTIPLY(&view->viewMat, &draw->model, modelView);
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, modelView->element));
      } break;
      case UB_ModelViewProj: {
        ALLOCA_MAT4x4(modelView);
        MAT_MULTIPLY(&view->viewMat, &draw->model, modelView);
        ALLOCA_MAT4x4(modelViewProj);
        MAT_MULTIPLY(&view->projMat, modelView, modelViewProj);
        GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, modelViewProj->element));
      } break;
      case UB_AlphaRef: {
        GL_CHECK(glUniform1f(loc, ((float)draw->state.alphaRef) / 255.0));
      } break;
      default:
        break;
    }
  }
  for (uint16_t i = 0; i < pu->usedCountUD; i++) {
    UniformProperty* prop = &pu->propertiesUD[i];
    UniformGL* uniform = &glCtx->uniforms[prop->index];
    UniformBase* ub = uniform->base;
    if (uniform_glType[ub->type] != prop->type) {
      printf_err("Uniform type mismatch: %s, In shader: %d, In app: %d\n", uniform->name->str, prop->type, ub->type);
    }
    switch (ub->type) {
      case UT_Float:
        GL_CHECK(glUniform1fv(prop->loc, ub->num, (const GLfloat*)uniform->data.ptr));
        break;
      case UT_Int:
        GL_CHECK(glUniform1iv(prop->loc, ub->num, (const GLint*)uniform->data.ptr));
        break;
      case UT_Bool:
        GL_CHECK(glUniform1uiv(prop->loc, ub->num, (const GLuint*)uniform->data.ptr));
        break;
      case UT_Vec4:
        GL_CHECK(glUniform4fv(prop->loc, ub->num, (const GLfloat*)uniform->data.ptr));
        break;
      case UT_Mat3x3:
        GL_CHECK(glUniformMatrix3fv(prop->loc, ub->num, GL_FALSE, (const GLfloat*)uniform->data.ptr));
        break;
      case UT_Mat4x4:
        GL_CHECK(glUniformMatrix4fv(prop->loc, ub->num, GL_FALSE, (const GLfloat*)uniform->data.ptr));
        break;
      case UT_Sampler1D:
      case UT_Sampler1DArray:
      case UT_Sampler2D:
      case UT_Sampler2DArray:
      case UT_Sampler3D:
      case UT_SamplerCubeMap:
      case UT_SamplerBuffer:
        assert(ub->num == 1);
        GL_CHECK(glUniform1i(prop->loc, (GLint)uniform->data.stage));
        gl_bindTextureUnit(glCtx, bind, uniform->data.stage);
        break;
      default:
        break;
    }
  }
}

/* }====================================================== */

/*
** {======================================================
** Parse Shader Source
** =======================================================
*/

#define islalpha(c) (isalpha(c) || (c) == '_')

typedef enum {
  TK_PRAGMA = 257,
  TK_INCLUDE,
  TK_OTHER,
  TK_EOS,
} Token;

static String StrPragma[1] = {STRING_LITERAL("pragma")};
static String StrInclude[1] = {STRING_LITERAL("include")};

static uint32_t nextToken(luaL_ByteBuffer* b) {
  const uint8_t* pCh = luaBB_readbytes(b, 1);
  while (pCh != NULL) {
    switch (*pCh) {
      case ' ':
      case '\f':
      case '\t':
      case '\v': { /* spaces */
        pCh = luaBB_readbytes(b, 1);
        break;
      }
      case '#':
      case '<': {
        return *pCh;
      }
      default: {
        if (islalpha(*pCh)) { /* identifier or reserved word? */
          String tmp[1];
          tmp->str = (const char*)pCh;
          tmp->sz = 0;
          do {
            tmp->sz++;
            pCh = luaBB_readbytes(b, 1);
          } while (pCh != NULL && islalpha(*pCh));
          if (pCh != NULL) {
            luaBB_unreadbytes(b, 1);
          }
          if (str_isEqual(tmp, StrPragma)) {
            return TK_PRAGMA;
          }
          if (str_isEqual(tmp, StrInclude)) {
            return TK_INCLUDE;
          }
        }
        return TK_OTHER;
      }
    }
  }
  return TK_EOS;
}
static const char* readPath(luaL_ByteBuffer* b, char del, size_t* sz) {
  const uint8_t* pCh = luaBB_readbytes(b, 1);
  const char* path = (const char*)pCh;
  while (pCh != NULL && *pCh != del) {
    switch (*pCh) {
      case '\n':
      case '\r':
        return NULL; // unfinished path
      default:
        pCh = luaBB_readbytes(b, 1);
        break;
    }
  }
  if (pCh != NULL && *pCh == del) {
    *sz = (const char*)pCh - path;
    return path;
  }
  return NULL;
}
typedef void (*OnFindIncludePath)(void* ud, const char* path, size_t len);
static void parseOneLine(const char* line, size_t sz, OnFindIncludePath cb, void* ud) {
  luaL_ByteBuffer b[1];
  luaBB_static(b, (uint8_t*)line, (uint32_t)sz, true, false);
  // include statement
  static uint32_t tokens[] = {
      '#',
      TK_PRAGMA,
      TK_INCLUDE,
      '<',
      TK_EOS,
  };
  for (int i = 0; tokens[i] != TK_EOS; i++) {
    if (nextToken(b) != tokens[i]) {
      return; // not a include statement
    }
  }
  size_t len = 0;
  const char* path = readPath(b, '>', &len);
  if (path != NULL && len != 0) {
    cb(ud, path, len);
  }
}
static void parseShaderSource(const char* buf, size_t sz, OnFindIncludePath cb, void* ud) {
  size_t lineIdx = 0;
  for (size_t i = 0; i < sz; i++) {
    const char ch = buf[i];
    if (ch == '\r' || ch == '\n') {
      if (lineIdx < i) {
        parseOneLine(&buf[lineIdx], i - lineIdx, cb, ud);
      }
      lineIdx = i + 1;
    }
  }
}

/* }====================================================== */

/*
** {======================================================
** Shader Dependence
** =======================================================
*/

void gl_initShaderInclude(RendererContextGL* glCtx) {
  IncludeNodeArray* ina = &glCtx->ina;
  ina->n = 0;
  ina->sz = 64;
  ina->arr = (IncludeNode*)mem_malloc(ina->sz * sizeof(IncludeNode));
}
void gl_destroyShaderInclude(RendererContextGL* glCtx) {
  IncludeNodeArray* ina = &glCtx->ina;
  for (size_t i = 0; i < ina->n; i++) {
    str_destroy(ina->arr[i].path);
  }
  mem_free((void*)ina->arr);
  ina->n = 0;
  ina->sz = 0;
  ina->arr = NULL;
}
void gl_addShaderIncludeHandle(RendererContextGL* glCtx, const String* path, bcfx_Handle handle) {
  IncludeNodeArray* ina = &glCtx->ina;
  if (ina->n >= ina->sz) {
    ina->sz *= 2;
    ina->arr = (IncludeNode*)mem_realloc(ina->arr, sizeof(IncludeNode) * ina->sz);
  }
  ina->arr[ina->n].path = path;
  ina->arr[ina->n].handle = handle;
  ina->n++;
}
bcfx_Handle gl_findShaderIncludeHandle(RendererContextGL* glCtx, const String* path) {
  IncludeNodeArray* ina = &glCtx->ina;
  for (size_t i = 0; i < ina->n; i++) {
    if (str_isEqual(path, ina->arr[i].path)) {
      return ina->arr[i].handle;
    }
  }
  return kInvalidHandle;
}

typedef struct {
  RendererContextGL* glCtx;
  ShaderGL* shader;
} Param;
static void _onFindIncludePath(void* ud, const char* str, size_t sz) {
  String path[1];
  path->str = str;
  path->sz = sz;
  Param* p = (Param*)ud;
  RendererContextGL* glCtx = p->glCtx;
  bcfx_Handle handle = gl_findShaderIncludeHandle(glCtx, path);
  if (handle == kInvalidHandle) {
    char* tmp = (char*)alloca(sz + 1);
    strncpy(tmp, str, sz);
    tmp[sz] = '\0';
    printf_err("Error: could not find shader include file: %s\n", tmp);
    return;
  }
  ShaderGL* shader = p->shader;
  if (shader->numDep >= BCFX_SHADER_DEPEND_COUNT) {
    printf_err("Error: shader include dependence more than max limit");
    return;
  }
  ShaderGL* depShader = &glCtx->shaders[handle_index(handle)];
  if (shader->type != depShader->type) {
    printf_err("Error: shader include dependence has different shader type");
    return;
  }
  shader->depend[shader->numDep] = handle;
  shader->numDep++;
}
void gl_scanShaderDependence(RendererContextGL* glCtx, ShaderGL* shader, const char* source, size_t len) {
  Param p[1];
  p->glCtx = glCtx;
  p->shader = shader;
  parseShaderSource(source, len, _onFindIncludePath, (void*)p);
}

typedef void (*OnFindShader)(void* ud, bcfx_Handle handle, ShaderGL* s);
static void _forEachDependShader(RendererContextGL* glCtx, bcfx_Handle handle, OnFindShader callback, void* ud) {
  // TODO: check handle invalid
  ShaderGL* shader = &glCtx->shaders[handle_index(handle)];
  callback(ud, handle, shader);
  for (uint16_t i = 0; i < shader->numDep; i++) {
    _forEachDependShader(glCtx, shader->depend[i], callback, ud);
  }
}

static void _doAttachShader(void* ud, bcfx_Handle handle, ShaderGL* shader) {
  ProgramGL* prog = (ProgramGL*)ud;
  (void)handle;
  GL_CHECK(glAttachShader(prog->id, shader->id));
}
void gl_attachShader(RendererContextGL* glCtx, ProgramGL* prog, bcfx_Handle handle) {
  _forEachDependShader(glCtx, handle, _doAttachShader, (void*)prog);
}

static void _doDetachShader(void* ud, bcfx_Handle handle, ShaderGL* shader) {
  ProgramGL* prog = (ProgramGL*)ud;
  (void)handle;
  GL_CHECK(glDetachShader(prog->id, shader->id));
}
void gl_detachShader(RendererContextGL* glCtx, ProgramGL* prog, bcfx_Handle handle) {
  _forEachDependShader(glCtx, handle, _doDetachShader, (void*)prog);
}

static bool bHasDepend = false;
static void _doDependenceCheck(void* ud, bcfx_Handle handle, ShaderGL* shader) {
  bcfx_Handle shaderHandle = *(bcfx_Handle*)ud;
  if (shaderHandle == handle) {
    bHasDepend = true;
  }
}
static bool gl_isProgramDependsShader(RendererContextGL* glCtx, bcfx_Handle prog, bcfx_Handle shaderHandle) {
  bHasDepend = false;
  _forEachDependShader(glCtx, prog, _doDependenceCheck, (void*)&shaderHandle);
  return bHasDepend;
}
void gl_updateAllProgram(RendererContextGL* glCtx, bcfx_Handle shaderHandle) {
  for (int i = 0; i < BCFX_CONFIG_MAX_PROGRAM; i++) {
    ProgramGL* prog = &glCtx->programs[i];
    if (prog->id != 0) {
      bcfx_Handle progHandle = handle_pack(HT_Program, i);
      if (gl_isProgramDependsShader(glCtx, progHandle, shaderHandle)) {
        glCtx->api.createProgram((RendererContext*)glCtx, progHandle, prog->vs, prog->fs);
      }
    }
  }
}
/* }====================================================== */
