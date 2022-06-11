#include <common_gl.h>

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
  int idx = 0;
  for (int i = 0; i < BCFX_CONFIG_MAX_INSTANCE_DATA; i++) {
    if (pa->instanceAttr[i] != -1 && i > idx) {
      pa->instanceAttr[idx] = pa->instanceAttr[i];
      pa->instanceAttr[i] = -1;
      idx++;
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
        bcfx_VertexLayout* layout = &glCtx->vertexLayouts[handle_index(vb->layout)];
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
        glCtx->curVertexCount = vb->size / layout->stride;
      }
      if (curId != vb->id) {
        curId = vb->id;
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vb->id)); // glVertexAttribPointer will save reference to this buffer
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
  VertexBufferGL* vb = &glCtx->vertexBuffers[handle_index(draw->instanceDataBuffer)];
  GLsizei stride = sizeof(GLfloat) * 4 * draw->numAttrib;
  PredefinedAttrib* pa = &prog->pa;
  for (uint8_t i = 0; pa->instanceAttr[i] != -1; i++) {
    GLint loc = pa->instanceAttr[i];
    if (i < draw->numAttrib) {
      GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vb->id));
      GL_CHECK(glEnableVertexAttribArray(loc));
      GL_CHECK(glVertexAttribDivisor(loc, 1)); // indicated it's per instance, not per vertex
      void* offset = (void*)((long)draw->instanceDataOffset + sizeof(GLfloat) * 4 * i);
      GL_CHECK(glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, stride, offset));
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

static const char* uniformNames[] = {
    "u_viewRect",
    "u_viewTexel",
    "u_view",
    "u_invView",
    "u_proj",
    "u_invProj",
    "u_viewProj",
    "u_invViewProj",
    "u_model",
    "u_modelView",
    "u_modelViewProj",
    "u_alphaRef4",
    NULL,
};
static bcfx_EUniformBuiltin findUniformBuiltinEnum(const char* name) {
  for (uint8_t i = 0; uniformNames[i] != NULL; i++) {
    if (strcmp(uniformNames[i], name) == 0) {
      return (bcfx_EUniformBuiltin)i;
    }
  }
  return UB_Count;
}
static uint16_t findUniformUserDefined(RendererContextGL* glCtx, const char* name) {
  uint16_t idx = 0;
  for (uint16_t i = 0; i < glCtx->uniformCount; i++, idx++) {
    UniformGL* uniform = &glCtx->uniforms[idx];
    while (uniform->name == NULL) {
      idx++;
      uniform = &glCtx->uniforms[idx];
    }
    if (strcmp(uniform->name, name) == 0) {
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
    bcfx_EUniformBuiltin eub = findUniformBuiltinEnum(name);
    if (eub != UB_Count) {
      assert(loc != -1);
      pu->used[cnt] = eub;
      cnt++;
      UniformProperty* prop = &pu->properties[eub];
      SET_UNIFORM_PROPERTY(prop, loc, num, gltype, 0);
    } else {
      uint16_t idx = findUniformUserDefined(glCtx, name);
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
      printf_err("Uniform type mismatch: %s, In shader: %d, In app: %d\n", uniform->name, prop->type, ub->type);
    }
    switch (ub->type) {
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
