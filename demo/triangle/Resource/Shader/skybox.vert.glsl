#pragma include<stdlib.vert>

out vec3 TexCoords;

void main()
{
    vec4 position = u_viewProj * vec4(a_position, 1.0);
    gl_Position = position.xyww;
    // gl_Position = vec4(a_position, 1.0);

    vec3 cubeSpace = WorldSpaceToCubeMapSpace(a_position);
    TexCoords = vec3(-cubeSpace.x, cubeSpace.yz); // negative x asix for Flip left and right
}
