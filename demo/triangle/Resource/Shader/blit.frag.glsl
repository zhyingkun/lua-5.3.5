#version 410 core

in vec2 v_texcoord0;

out vec4 FragColor;

uniform sampler2D blit_texture;

void main() {
	FragColor = texture(blit_texture, v_texcoord0);
}
