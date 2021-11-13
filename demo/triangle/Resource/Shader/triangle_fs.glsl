#version 410 core

in vec3 v_color0;

out vec4 FragColor;

void main() {
	FragColor = vec4(v_color0, 1.0);
}
