#version 410 core

in vec3 a_position;
in vec3 a_color0;

uniform mat4 u_model;

out vec3 v_color0;

void main() {
	v_color0 = a_color0;
	gl_Position = u_model * vec4(a_position, 1.0);
}
