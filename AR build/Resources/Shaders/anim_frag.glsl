#version 330

uniform sampler2D texture;
in vec2 TexCoord0;
out vec4 FragColor;

void main()
{
    FragColor = texture2D(texture, TexCoord0.xy);
}