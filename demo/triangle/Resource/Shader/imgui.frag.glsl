#version 410 core

// precision mediump float;
uniform sampler2D Texture;

in vec2 v_texcoord0;
in vec4 v_color0;

out vec4 FragColor;

void main(){
   FragColor = v_color0 * texture(Texture, v_texcoord0.st);
   // FragColor = vec4(1.0, 1.0, 1.0, 1.0);
   // FragColor = v_color0;
   // FragColor = texture(Texture, v_texcoord0.st);
   // FragColor = vec4(v_texcoord0.st, 0.0, 1.0);
   // FragColor = vec4(texture(Texture, vec2(v_texcoord0.s, v_texcoord0.t)).rgb, 1.0);
}
