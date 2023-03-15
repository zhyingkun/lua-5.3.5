#define _shader_c_
#include <common.h>
#include <utils.h>

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
  const String vertex;
  const String fragment;
} ShaderConfig;

const ShaderConfig BuiltinShaderList[] = {
    {STRING_LITERAL(DefaultVertex), STRING_LITERAL(DefaultFragment)},
    {STRING_LITERAL_NULL(), STRING_LITERAL_NULL()},
};

static bcfx_Handle _doCreateShader(const String* shaderCode, bcfx_EShaderType type) {
  if (str_isNull(shaderCode)) {
    return kInvalidHandle;
  }
  luaL_MemBuffer mb[1];
  MEMBUFFER_SETINIT_STATIC(mb, shaderCode->str, shaderCode->sz);
  return bcfx_createShader(mb, type);
}
bcfx_Handle builtin_createShader(bcfx_EBuiltinShaderType type) {
  const ShaderConfig* config = &BuiltinShaderList[type];
  const bcfx_Handle vsh = _doCreateShader(&config->vertex, ST_Vertex);
  const bcfx_Handle fsh = _doCreateShader(&config->fragment, ST_Fragment);
  return bcfx_createProgram(vsh, fsh);
}
