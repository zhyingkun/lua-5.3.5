in vec2 TexCoord;
flat in float Layer;

uniform sampler2DArray Sampler2DArray;

out vec4 FragColor;

void main()
{
    FragColor = texture(Sampler2DArray, vec3(TexCoord, Layer));
}
