#version 410 core

in vec3 a_position;

out vec3 TexCoords;

uniform mat4 u_viewProj;

vec3 WorldSpaceToCubeMapSpace(vec3 pos) {
    return vec3(pos.x, -pos.z, -pos.y);
}

void main()
{
    vec4 position = u_viewProj * vec4(a_position, 1.0);
    gl_Position = position.xyww;
    // gl_Position = vec4(a_position, 1.0);

    vec3 cubeSpace = WorldSpaceToCubeMapSpace(a_position);
    TexCoords = vec3(-cubeSpace.x, cubeSpace.yz); // negative x asix for Flip left and right
}
