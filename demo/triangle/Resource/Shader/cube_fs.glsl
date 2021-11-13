#version 410 core
in vec3 v_color0;
in vec2 v_texcoord0;

out vec4 FragColor;

uniform sampler2D my_texture;

void main() {
	// FragColor = vec4(v_color0, 1.0);
	FragColor = texture(my_texture, v_texcoord0);
	// FragColor = vec4(v_texcoord0, 0.0, 1.0);
}
