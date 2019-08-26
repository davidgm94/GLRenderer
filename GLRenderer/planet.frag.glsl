#version 330 core
out vec4 FragmentColor;

in vec2 TexCoord;

uniform sampler2D texture_diffuse1;

void main()
{
	FragmentColor = texture(texture_diffuse1, TexCoord);
}