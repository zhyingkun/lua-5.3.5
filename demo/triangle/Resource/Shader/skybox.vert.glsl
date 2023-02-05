#version 410 core

in vec3 a_position;

out vec3 TexCoords;

uniform mat4 u_viewProj;

void main()
{
    gl_Position = u_viewProj * vec4(a_position, 1.0);
    TexCoords = -a_position;
}