#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D Tex1;
uniform sampler2D Tex2;
void main()
{
    FragColor = mix(texture(Tex1, TexCoord), texture(Tex2, TexCoord), 0.5);
}