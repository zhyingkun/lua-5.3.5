#version 410 core

in vec3 a_position;

out vec3 TexCoords;

uniform mat4 u_viewProj;

vec3 WorldSpaceToCubeMapSpace(vec3 pos) {
/*
 * ASpace translate to BSpace, we should find 4 vectors according to BSpace:
 * AOrigin, AX, AY, AZ
 * point a in ASpace, the correspond point b in BSpace, we can measure:
 * RotateMatrix = (AX, AY, AZ, I) in column vector
 * TranslateMatrix = (I, I, I, AOrigin) in column vector
 * b = TranslateMatrix * RotateMatrix * a // Usually scale first, then rotate and finally translate
 * 
 * WorldSpace:   +x to right, +y to front,  +z to top
 * CubeMapSpace: +x to right, +y to bottom, +z to back
 * Origin is the same
 * In CubeMapSpace, (0, 0, -1) means to front, equals WorldSpace's y axis
 */
    vec3 WorldX = vec3(1.0, 0.0, 0.0);
    vec3 WorldY = vec3(0.0, 0.0, -1.0);
    vec3 WorldZ = vec3(0.0, -1.0, 0.0);
    mat3 WorldToCubeMap = mat3(WorldX, WorldY, WorldZ); // in column vector
    return WorldToCubeMap * pos;
    // return vec3(pos.x, -pos.z, -pos.y);
}

void main()
{
    vec4 position = u_viewProj * vec4(a_position, 1.0);
    gl_Position = position.xyww;
    // gl_Position = vec4(a_position, 1.0);

    vec3 cubeSpace = WorldSpaceToCubeMapSpace(a_position);
    TexCoords = vec3(-cubeSpace.x, cubeSpace.yz); // negative x asix for Flip left and right
}
