#version 410 core

in vec2 a_position;

in vec2 a_texcoord0;
in vec4 a_color0;

uniform mat4 u_proj;

out vec2 v_texcoord0;
out vec4 v_color0;

void main() {
   v_texcoord0 = a_texcoord0;
   v_color0 = a_color0;
   gl_Position = u_proj * vec4(a_position.xy, 0, 1);
}
