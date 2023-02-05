#version 410 core

in vec3 TexCoords;

uniform samplerCube CubeMapSampler;

out vec4 FragColor;

void main()
{
    FragColor = texture(CubeMapSampler, TexCoords);
}