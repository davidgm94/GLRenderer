#version 330 core
out vec4 fragmentColor;
in vec3 VSColor;

void main()
{
	fragmentColor = vec4(VSColor, 1.0f);
}