#version 410 core
in vec3 a_position;
in vec3 a_color0;
in vec2 a_texcoord0;

uniform mat4 u_modelViewProj;

out vec3 v_color0;
out vec2 v_texcoord0;

void main() {
	gl_Position = u_modelViewProj * vec4(a_position, 1.0);
	v_color0 = a_color0;
	v_texcoord0 = a_texcoord0;
}
