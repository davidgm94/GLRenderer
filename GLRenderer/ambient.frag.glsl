#version 330 core

out vec4 fragColor;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
	float ambientConstant = 0.1f;
	vec3 ambientLight = ambientConstant * lightColor;
	vec3 result = ambientLight * objectColor;
	fragColor = vec4(result, 1.0f);
}