in vec3 a_position;
in vec3 a_normal;
in vec2 a_texcoord0;

uniform mat4 u_modelViewProj;

flat out float Layer;
out vec2 TexCoord;

float GetLayer(vec3 n) {
    vec3 a = abs(n);
    vec3 t = (-n + vec3(1.0f, 1.0f, 1.0f)) / 2.0f + vec3(0.0f, 2.0f, 4.0f);
    return floor(dot(a, t) + 0.5f);
}

void main() {
    gl_Position = u_modelViewProj * vec4(a_position, 1.0);
    Layer = GetLayer(a_normal);
    TexCoord = a_texcoord0;
}
