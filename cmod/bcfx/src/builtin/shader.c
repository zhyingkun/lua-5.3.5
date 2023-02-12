#define _shader_c_
#include <common.h>

/*
** {======================================================
** Default Shader
** =======================================================
*/

static const char DefaultVertex[] = "\
in vec3 a_position;                                        \n\
uniform mat4 u_modelViewProj;                              \n\
void main() {                                              \n\
	gl_Position = u_modelViewProj * vec4(a_position, 1.0); \n\
}";
static const char DefaultFragment[] = "\
out vec4 FragColor;                       \n\
void main() {                             \n\
	FragColor = vec4(1.0, 0.0, 0.0, 1.0); \n\
}";

/* }====================================================== */

typedef struct {
  const char* vertexCode;
  size_t vertexSize;
  const char* fragmentCode;
  size_t fragmentSize;
} ShaderConfig;

#define CONFIG_SHADER(vertex, fragment) \
  { vertex, sizeof(vertex) - 1, fragment, sizeof(fragment) - 1 }

const ShaderConfig BuiltinShaderList[] = {
    CONFIG_SHADER(DefaultVertex, DefaultFragment),
    {NULL, 0, NULL, 0},
};

bcfx_Handle builtin_createShader(bcfx_EBuiltinShaderType type) {
  const ShaderConfig* config = &BuiltinShaderList[type];
  bcfx_Handle vertexHandle = kInvalidHandle;
  bcfx_Handle fragmentHandle = kInvalidHandle;
  if (config->vertexCode) {
    luaL_MemBuffer mb = {(void*)config->vertexCode, config->vertexSize, NULL, NULL};
    vertexHandle = bcfx_createShader(&mb, ST_Vertex);
  }
  if (config->fragmentCode) {
    luaL_MemBuffer mb = {(void*)config->fragmentCode, config->fragmentSize, NULL, NULL};
    fragmentHandle = bcfx_createShader(&mb, ST_Fragment);
  }
  return bcfx_createProgram(vertexHandle, fragmentHandle);
}
