#version 330 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in mat4 instanceMatrix;

out vec2 TexCoord;

uniform mat4 proj;
uniform mat4 view;

void main()
{
	gl_Position = proj * view * instanceMatrix * vec4(position, 1.0f);
	TexCoord = texCoord;
}