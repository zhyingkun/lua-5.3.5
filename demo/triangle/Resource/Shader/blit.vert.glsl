#version 410 core

in vec2 a_position;
in vec2 a_texcoord0;

out vec2 v_texcoord0;

void main() {
	v_texcoord0 = a_texcoord0;
	gl_Position = vec4(a_position, 0.0, 1.0);
}
