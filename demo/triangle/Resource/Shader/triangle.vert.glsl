#version 410 core

in vec3 a_position;
in vec3 a_color0;

in vec3 i_data0;

uniform mat4 u_model;

out vec3 v_color0;

void main() {
	v_color0 = a_color0 + i_data0;
	float offset = float(gl_InstanceID) / 2.0;
	gl_Position = u_model * vec4(a_position + vec3(offset, offset, offset), 1.0);
}
